#!/usr/bin/env python3

# from calendar import calendar
from datetime import datetime
import time 
from threading import Thread

import logging, os

from common import Datetool, Calendar, Display, Room, __TESTS__
from ade2 import Ade2

from digi.xbee.devices import ZigBeeDevice, XBeeMessage,XBee64BitAddress,TransmitException
from messages import *
from config import Config

from logs import DisplayReport

from unidecode import unidecode

MAJOR_VER = 0
MINOR_VER = 2

class Application :
    configuration:Config=None
    log =None

    ade:Ade2=None
    zigbee:ZigBeeDevice = None
    zigbeeDataLengthMax:int
    displayReport: DisplayReport = None

    def __init__(self):
        self.log = logging.getLogger("server")
        logging.basicConfig(level=os.environ.get("LOGLEVEL", "INFO"))

    def openZigbee(self, port:str, baudrate:int)->None:
        self.zigbee = ZigBeeDevice(port, baudrate)
        self.zigbee.open()

    def getZigbeeConfiguration(self)->None:
        self.device_64bit_addr = self.zigbee.get_64bit_addr()
        self.device_16bit_addr = self.zigbee.get_16bit_addr()
        self.device_panid = self.zigbee.get_pan_id()

    def closeZigbee(self)->None:
        self.zigbee.close()

    def getRoomfromDisplayId(self, displayId: int) -> Room:
        for r in self.roomsList:
            if r.hasDisplay(displayId):
                return r
            
        return None

    def __threadTimer(self)-> None:
        self.log.info("Thread \"Timer\" started")

        while True:
            time.sleep(60.0) # Wait 1 minute
            print ("1 minute elapsed")

            self.displayReport.write()
            

    def __threadMessages(self)-> None:
        self.log.info ("Thread \"Zigbee messages\" started")
        
        while True:
            exceptionRaised = False
            transaction=Transaction()
            receivedMsg:XBeeMessage=None

            try:
                receivedMsg = self.zigbee.read_data(60*60*24) # Wait for 1 day
            except Exception as e:
                self.log.error("Exception raised in __threadMessages: " + str(e))
                exceptionRaised=True

            if not exceptionRaised:
                transaction.senderAddr = receivedMsg.remote_device.get_16bit_addr()
                transaction.senderId = receivedMsg.remote_device.get_64bit_addr()

                transaction.cmd = receivedMsg.data.decode('utf-8') #conversion des data au format bytearray en string
                # self.log.info("[RX] Received data from: {}[{}], data: {}".format(
                #             ''.join('{:02X}'.format(a) for a in transaction.senderAddr),
                #             ''.join('{:02X}'.format(a) for a in transaction.senderId),
                #             transaction.cmd))

                # recherche la room correspondant à l'ID de l'ecran et traite ensuite sa commande
                displayId = int.from_bytes(transaction.senderId.address, byteorder='big', signed=False)
                # print ("Display Id = {}".format(hex(displayId)))
                displayRoom=self.getRoomfromDisplayId(displayId)

                if displayRoom != None:
                    for d in displayRoom.displays:
                        if d.id == displayId:
                            d.lastSeen=0 # on remet le compteur à zero pour cet ecran

                transaction.answer = Message.manage(transaction.cmd, displayRoom)

                # print("[TX] Send data to: {}, data: {}".format(
                #         ''.join('{:02X}'.format(a) for a in transaction.senderAddr),
                #         transaction.answer))
                
                while transaction.retryCnt!=0 and not transaction.txSuccess:
                    try:
                        if len(transaction.answer) > self.zigbeeDataLengthMax:
                            s = transaction.answer[:self.zigbeeDataLengthMax-1]
                        else: 
                            s=transaction.answer

                        s=unidecode(s)

                        #print ("len of s: {}".format(len(s)))
                        #print ("len of answer: {}".format(len(transaction.answer)))

                        self.zigbee.send_data_64_16(XBee64BitAddress.UNKNOWN_ADDRESS,
                                                    transaction.senderAddr, s)
                        
                        if len(transaction.answer) > self.zigbeeDataLengthMax:
                            transaction.answer = transaction.answer[self.zigbeeDataLengthMax:]
                        else:
                            transaction.answer=""

                        if len(transaction.answer) ==0:
                            transaction.txSuccess=True
                    except TransmitException :
                        transaction.retryCnt = transaction.retryCnt-1

                if transaction.retryCnt==0:
                    if displayRoom!=None:
                        self.log.error("Unable to send answer to display [{}] of room {}".format(
                            hex(displayId),
                            displayRoom.name                        
                        ))
                    else:
                        self.log.error("Unable to send answer to display [{}] of unknown room".format(
                            displayId
                        ))

    def run(self):
        print ("SmartDoors server ver " + str(MAJOR_VER)+"." + str(MINOR_VER))

        self.log.info("Server start {}".format(datetime.now()))

        # Get command line information
        self.configuration = Config(self.log)
        configFiles = self.configuration.parseCommandLine() # Get configuration path and files in a list
        
        # read configuration file
        if configFiles != None:
            self.roomsList = self.configuration.getConfiguration(configFiles)
        else:
            raise Exception("Invalid list of path for configuration files")

        # Initialisation of ade
        # self.ade = Ade2(self.roomsList, self.configuration, __TESTS__.testCalendar)
        self.ade = Ade2(self.roomsList, self.configuration, False)

        # Initialize calendar information
        self.ade.getCalendars()

        # Merge calendars for room with multiples ressources
        for room in self.roomsList:
            room.mergeRessourcesCalendars()

        if __TESTS__.testCalendar:               
            for room in self.roomsList:
                print()
                print (str(room))
                for cal in room.calendars:
                    print(str(cal))

        self.displayReport = DisplayReport("report.log", self.roomsList)
        self.displayReport.write()
        
        try:
            self.openZigbee("/dev/ttyUSB0", 230400)
            self.getZigbeeConfiguration()

            print (str(self.zigbee))
        except Exception as e:
            print ("Unable to start Zigbee\n" + str(e))
            return -1
        
        Message.configuration = self.configuration
        Message.log = self.log

        self.zigbeeDataLengthMax= int.from_bytes(self.zigbee.get_parameter('NP'))
        self.zigbeeDataLengthMax=self.zigbeeDataLengthMax-20 # 20 correspond to thers bytes in the payload that are not data

        print ("Zigbee maximum data length: {} bytes".format(self.zigbeeDataLengthMax))

        # Lancement des threads
        self.threadTimerHandler = Thread(target=self.__threadTimer,args=())
        self.threadMessagesHandler = Thread(target=self.__threadMessages,args=())

        self.threadTimerHandler.start()
        self.threadMessagesHandler.start()

        self.threadTimerHandler.join()
        self.threadMessagesHandler.join()

        self.stop()
    
    def stop(self)->None:
        try:
            self.closeZigbee()
        except:
            pass

        self.log.info("Server closed")
        
if __name__ == '__main__':
    app = Application()

    try:
        app.run()
    except KeyboardInterrupt:
        app.stop()
    