#!/usr/bin/env python3

from calendar import calendar
from datetime import datetime
import time
from threading import Thread
from queue import Queue

import logging, os
import statistics

import argparse
import configparser
import json

from common import Datetool, Calendar, Display, Room, __TESTS__
from ade2 import Ade2, ADEInvalidJSON,ADERoomNotInList,ADEServerError

from messages import *

MAJOR_VER = 0
MINOR_VER = 2

serverPort = -1
adeServer=""
adeLogin=""
adePassword=""
adeProjectId=""
adeProject=-1

rf_panId = 0x1234
rf_chanId =0x10

refreshTime=15*60  #15 minutes 
refreshStartTime="6:00"
refreshEndTime="19:00"

authorizedGateways=[]        # list of authorized gateways, given in config file
roomsList:Room = []          # list of rooms, given in config file
authentifiedDisplays = {}  # list (dictionnary) of accepted display, with corresponding room

reportLog = ""
errorLog = ""
dictionnaryFile = ""
wordDictionnary = [{}]

messagesManager: MessageMgr = None

DEFAULT_CONFIG_FILE = ["./software/server/src/server.conf",
                       "./server.conf",
                       "/home/dimercur/Travail/git/afficheurs/software/server-new/src/server.conf",
                       "/etc/smartdisplay/server.conf"]

DEFAULT_DICTIONNARY_FILE = ["./software/server/src/dict.conf",
                       "./dict.conf",
                       "/home/dimercur/Travail/git/afficheurs/software/server-new/src/dict.conf",
                       "/etc/smartdisplay/dict.conf"]

log = logging.getLogger("server")
logging.basicConfig(level=os.environ.get("LOGLEVEL", "INFO"))

def parseCommandLine() -> str:
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', metavar='configfile', help='Configuration file')
    
    args = parser.parse_args()
    return args.c
    
def getConfiguration(confFile: str)->bool:
    global serverPort
    global adeServer
    global adeLogin
    global adePassword
    global refreshTime,refreshStartTime,refreshEndTime
    global authorizedGateways
    global roomsList
    global reportLog, errorLog, dictionnaryFile
    global rf_chanId, rf_panId
    global adeProjectId

    config = configparser.ConfigParser()
    if config.read(confFile) == []:
        log.info('Unable to open configuration file "' + confFile + '"')
        return False
    
    if not 'server' in config:
        log.error('No "server" section in configuration file')
        return False
    
    try:
        serverPort = int(config['server']['port'])
        adeServer = config['server']['adeserver']
        adeLogin = config['server']['adelogin']
        adePassword = config['server']['adepassword']
        refreshTime = int(config['server']['refreshtime'])*60
        refreshStartTime = config['server']['refreshstarttime']
        refreshEndTime = config['server']['refreshendtime']
        rf_chanId = int(config['server']['rf_chanid'],base=16)
        rf_panId = int(config['server']['rf_panid'],base=16)
        
        reportLog = config['server']['reportlog'] 
        errorLog = config['server']['errorlog'] 
        dictionnaryFile = config['server']['dictionnary'] 
    except Exception as e:
        log.error('Misformatted configuration file for "server" section(' + str(e) + ')')
        return False
    
    try:
        adeProjectId = str(int(config['server']['adeproject'],base=10))
    except Exception as e:
        pass

    try:
        for key in config:
            if key != "server" and key != "authorized gateways" and key != "DEFAULT":
                room = Room(key, config[key]['type'], config[key]['ade_pattern'],[],[])
                room.calendars=[]
                room.ressourcesId=[]
                
                displays = json.loads(config[key]['displays'])
                if type(displays)==list:
                    for d in displays:
                        room.displayId.append(int(d, 16))
                else:
                    room.displayId.append(int(displays, 16))
                    
                roomsList.append(room)
    except Exception as e:
        log.error('Misformatted configuration file for rooms sections (' + str(e) + ')')
        return False
        
    log.info("Configuration read from " + str(confFile)+ ": Ok")
    return True

def updateCalendars():
    global roomsList
    global adeServer
    global adeLogin
    global adePassword
    global adeProjectId
    
    counter=10
    listRetrieved = False

    # Retrieve ressources, room information and calendars from ade
    adeSession=Ade2()
    
    while counter!=0 and not listRetrieved:
        try:
            adeSession.getRoomsList()
            listRetrieved=True
        except Exception as e:
            counter=counter-1
            if counter==0:
                raise e
            
    for room in roomsList:
        try:
           room.ressourcesId.append(adeSession.getRoomID(str(room.adePattern)))
        except ADERoomNotInList as e:
            raise(e)
                
    # Get calendar for current week   
    for room in roomsList:
        for res in room.ressourcesId:
            startDay=Datetool.getDateStrForURL(Datetool.getFirstDayofWeek())
            endDay=Datetool.getDateStrForURL(Datetool.getLastDayofWeek())

            # if __TESTS__.testCalendar:
            #    print ("Get calendar from " + startDay + " to " + endDay)

            cal = adeSession.getRessourceCalendarByDate(res, startDay, endDay)
            
            # if __TESTS__.testCalendar:
            #    print ("")
            #    print ("Calendar for room " + room.name + " :")

            for c in cal:
            #    if __TESTS__.testCalendar:
            #        print (c)
                room.calendars.append(c)
            
#        room.calendars = Calendar.cleanup(room.calendars, wordDictionnary)

def isDisplayAuthentified(display:int)->bool:
    if display in authentifiedDisplays:
        return True
    else:
        return False

def getRoomfromDisplay(display: int) -> Room:
    for r in roomsList:
        if display in r.displayId:
            return r
        
    return None

def actionHandler(msg: Message, gateway:str) -> Message:
    msg_ans=None
    print ("Received message in action handler from " + gateway)
    print (str(msg))
    
    # if not isGatewayAuthentified(gateway):
    #     msg_ans=Message.createERR(msg.device_id, Message.ERR_GW_REFUSED)
    # elif not isDisplayAuthentified(msg.device_id) and \
    if not isDisplayAuthentified(msg.device_id) and \
        msg.type != Message.CMD_JOIN and \
        msg.type != Message.CMD_CONFIG:
        msg_ans=Message.createERR(msg.device_id, Message.ERR_DISPLAY_REFUSED)
    else:
        if msg.type == Message.CMD_CONFIG:
            msg_ans = Message.createOK(msg.device_id, 
                                       [hex(rf_panId)[2:].upper(), 
                                        hex(rf_chanId)[2:].upper(),
                                        " "]) # gateway name is set to " "
        elif msg.type == Message.CMD_JOIN:
            #timer to be set
            room = getRoomfromDisplay(msg.device_id)
            if room !=None:
                if not isDisplayAuthentified(msg.device_id):
                    display = Display()
                    display.id = msg.device_id
                    display.room = room
                    display.gw = gateway
                    authentifiedDisplays[msg.device_id]=display
                    
                authentifiedDisplays[msg.device_id].joinSignalStrength[gateway] = int(msg.data[0],base=16)
                msg_ans = Message.createACCEPT(msg.device_id, rf_panId)
            else:
                msg_ans = Message.createREJECT(msg.device_id)
        elif msg.type == Message.CMD_SETUP:
            currentHour = datetime.now().strftime("%H:%M:%S")
            currentDate = Datetool.getDayNumber(datetime.now())
            display = authentifiedDisplays[msg.device_id]
            
            msg_ans = Message.createOK(msg.device_id, 
                                       [currentHour,
                                        str(currentDate),
                                        display.room.name,
                                        display.room.type,
                                        datetime.strptime(refreshStartTime, '%H:%M').time().strftime("%H:%M:%S"),
                                        datetime.strptime(refreshEndTime, '%H:%M').time().strftime("%H:%M:%S"),
                                        str(refreshTime),
                                        str(0)]) # update order is set to 0
        elif msg.type == Message.CMD_GET_CALENDAR:
            cal = authentifiedDisplays[msg.device_id].room.calendars # get calendar for associated display'room
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
            if authentifiedDisplays[msg.device_id].calendarUpdate:
                msg_ans = Message.createOK(msg.device_id,[str(1)])
            else:
                msg_ans = Message.createOK(msg.device_id,[str(0)])
        elif msg.type == Message.CMD_REPORT:
            authentifiedDisplays[msg.device_id].displayMinRSSI= int(msg.data[0],base=16)
            authentifiedDisplays[msg.device_id].displayMaxRSSI= int(msg.data[1],base=16)
            authentifiedDisplays[msg.device_id].displayMoyRSSI= int(msg.data[2],base=16)
            authentifiedDisplays[msg.device_id].batterylevel= int(msg.data[3])
            authentifiedDisplays[msg.device_id].gwMinRSSI= int(msg.data[4],base=16)
            authentifiedDisplays[msg.device_id].gwMaxRSSI= int(msg.data[5],base=16)
            authentifiedDisplays[msg.device_id].gwMoyRSSI= int(msg.data[6],base=16)
            
            # pas de msg_ans, fermeture du socket à la place
        else: # command unknown
            msg_ans=Message.createERR(msg.device_id, Message.ERR_NO_ACTION)
    
    return msg_ans
    
def main():
    global roomsList
    global wordDictionnary
    global messagesManager
    
    # Get command line information
    configFile = parseCommandLine()
    
    # read configuration file
    configOk=False
    if configFile != None:
        if getConfiguration(configFile):
            configOk=True
    else:
        currentDir = os.getcwd()

        for f in DEFAULT_CONFIG_FILE:
            if f[0] != '/':
                f = currentDir +'/'+ f

            if getConfiguration(f):
                configOk=True
                break
    
    if not configOk:
        return 1
      
    # Initialize calendar information
    updateCalendars()

    if __TESTS__.testCalendar:               
        for room in roomsList:
            print()
            print (str(room))
            for cal in room.calendars:
                print(str(cal))
    
    # TODO: A revoir tout ça
    # try:
    #     messagesManager = MessageMgr('localhost',serverPort,30)
    # except Exception as e:
    #     log.error (str(e))
    #     return 2
    
    # messagesManager.actionHandler = actionHandler
    # messagesManager.start()

if __name__ == '__main__':
    print ("SmartDoors server ver " + str(MAJOR_VER)+"." + str(MINOR_VER))
    
    try:
        main()
    except KeyboardInterrupt:
        messagesManager.stop()
    
    log.info("Server stopped")