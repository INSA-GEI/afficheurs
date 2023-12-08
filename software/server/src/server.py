#!/usr/bin/env python3

from calendar import calendar
from datetime import datetime
import time
from threading import Thread
#from queue import Queue

import logging, os

from common import Datetool, Calendar, Display, Room, __TESTS__
from ade2 import Ade2, ADEInvalidJSON,ADERoomNotInList,ADEServerError

from messages import *
from zigbee import Zigbee
from config import Config

from logs import DisplayReport

MAJOR_VER = 0
MINOR_VER = 2

class Application :
    
    authentifiedDisplays = {}    # dictionnary of accepted display, with corresponding room

    configuration:Config=None
    log =None

    ade:Ade2=None
    zigbee:Zigbee = None
    messagesManager: MessageMgr = None
    displayReport: DisplayReport = None

    def __init__(self):
        self.messagesManager: MessageMgr = None

        self.log = logging.getLogger("server")
        logging.basicConfig(level=os.environ.get("LOGLEVEL", "INFO"))

    # def isDisplayAuthentified(self, display:int)->bool:
    #     if display in self.authentifiedDisplays:
    #         return True
    #     else:
    #         return False

    def getRoomfromDisplayId(self, displayId: int) -> Room:
        for r in self.roomsList:
            if r.hasDisplay(displayId):
                return r
            
        return None

    def __threadTimer(self)-> None:
        while True:
            time.sleep(5*60.0) # Wait 5 minutes
            print ("5 minutes elapsed, ready to check for calendar update")

    def __threadMessages(self)-> None:
        while True:
            msg_ans=None
            print ("Received message in action handler from " + gateway)
            print (str(msg))
            
            # if not isGatewayAuthentified(gateway):
            #     msg_ans=Message.createERR(msg.device_id, Message.ERR_GW_REFUSED)
            # elif not isDisplayAuthentified(msg.device_id) and \
            if not self.isDisplayAuthentified(msg.device_id) and \
                msg.type != Message.CMD_JOIN and \
                msg.type != Message.CMD_CONFIG:
                msg_ans=Message.createERR(msg.device_id, Message.ERR_DISPLAY_REFUSED)
            else:
                if msg.type == Message.CMD_CONFIG:
                    msg_ans = Message.createOK(msg.device_id, 
                                            [hex(self.rf_panId)[2:].upper(), 
                                                hex(self.rf_chanId)[2:].upper(),
                                                " "]) # gateway name is set to " "
                elif msg.type == Message.CMD_JOIN:
                    #timer to be set
                    room = self.getRoomfromDisplay(msg.device_id)
                    if room !=None:
                        if not self.isDisplayAuthentified(msg.device_id):
                            display = Display()
                            display.id = msg.device_id
                            display.room = room
                            display.gw = gateway
                            self.authentifiedDisplays[msg.device_id]=display
                            
                        self.authentifiedDisplays[msg.device_id].joinSignalStrength[gateway] = int(msg.data[0],base=16)
                        msg_ans = Message.createACCEPT(msg.device_id, self.rf_panId)
                    else:
                        msg_ans = Message.createREJECT(msg.device_id)
                elif msg.type == Message.CMD_SETUP:
                    currentHour = datetime.now().strftime("%H:%M:%S")
                    currentDate = Datetool.getDayNumber(datetime.now())
                    display = self.authentifiedDisplays[msg.device_id]
                    
                    msg_ans = Message.createOK(msg.device_id, 
                                            [currentHour,
                                                str(currentDate),
                                                display.room.name,
                                                display.room.type,
                                                datetime.strptime(self.refreshStartTime, '%H:%M').time().strftime("%H:%M:%S"),
                                                datetime.strptime(self.refreshEndTime, '%H:%M').time().strftime("%H:%M:%S"),
                                                str(self.refreshTime),
                                                str(0)]) # update order is set to 0
                elif msg.type == Message.CMD_GET_CALENDAR:
                    cal = self.authentifiedDisplays[msg.device_id].room.calendars # get calendar for associated display'room
                    data = [Datetool.getDaysofWeek()]
                    
                    s = ""
                    for c in cal:
                        s+=str(Datetool.getDayNumber(Datetool.toDate(c.day)))
                        s+=';' + str(Datetool.minutsFromMidnight(c.firstHour))
                        s+=';' + str(Datetool.minutsFromMidnight(c.lastHour))
                        s+=';' + c.title
                        s+=';'
                        
                        for t in c.trainees:
                            s+=t+','
                            
                        if s[-1] == ',':
                            s= s[:-1]
                            
                        s+=';'
                        for i in c.instructors:
                            s+=i+','
                        
                        if s[-1] == ',':
                            s= s[:-1]
                        
                        s+='#'
                        
                    if s[-1] == '#':
                        s= s[:-1]
                    
                    data.append(s)
                    msg_ans = Message.createOK(msg.device_id, data)
                elif msg.type == Message.CMD_GET_UPDATE:
                    if self.authentifiedDisplays[msg.device_id].calendarUpdate:
                        msg_ans = Message.createOK(msg.device_id,[str(1)])
                    else:
                        msg_ans = Message.createOK(msg.device_id,[str(0)])
                elif msg.type == Message.CMD_REPORT:
                    self.authentifiedDisplays[msg.device_id].displayMinRSSI= int(msg.data[0],base=16)
                    self.authentifiedDisplays[msg.device_id].displayMaxRSSI= int(msg.data[1],base=16)
                    self.authentifiedDisplays[msg.device_id].displayMoyRSSI= int(msg.data[2],base=16)
                    self.authentifiedDisplays[msg.device_id].batterylevel= int(msg.data[3])
                    self.authentifiedDisplays[msg.device_id].gwMinRSSI= int(msg.data[4],base=16)
                    self.authentifiedDisplays[msg.device_id].gwMaxRSSI= int(msg.data[5],base=16)
                    self.authentifiedDisplays[msg.device_id].gwMoyRSSI= int(msg.data[6],base=16)
                    
                    # pas de msg_ans, fermeture du socket à la place
                else: # command unknown
                    msg_ans=Message.createERR(msg.device_id, Message.ERR_NO_ACTION)

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
            self.zigbee = Zigbee()
            self.zigbee.open("/dev/ttyUSB0", 230400)
            self.zigbee.getCurrentConfiguration()

            print (str(self.zigbee))
        except Exception as e:
            print ("Unable to start Zigbee\n" + str(e))
            return -1
        
        # TODO: A revoir tout ça

        # try:
        #     messagesManager = MessageMgr('localhost',serverPort,30)
        # except Exception as e:
        #     log.error (str(e))
        #     return 2
        
        # messagesManager.actionHandler = actionHandler
        # messagesManager.start()

        # TODO Lancement des threads
        # self.threadTimerHandler = Thread(target=self.__threadTimer,args=())
        # self.threadMessagesHandler = Thread(target=self.__threadMessages,args=())

        # self.threadTimerHandler.start()
        # self.threadMessagesHandler.start()

        # self.threadTimerHandler.join()
        # self.threadMessagesHandler.join()

        self.stop()
    
    def stop(self)->None:
        try:
            self.zigbee.close()
            self.messagesManager.stop()
        except:
            pass

        self.log.info("Server closed")
        
if __name__ == '__main__':
    app = Application()

    try:
        app.run()
    except KeyboardInterrupt:
        app.stop()
    