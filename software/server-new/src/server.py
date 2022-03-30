#!/usr/bin/env python3

from calendar import calendar
import time
from threading import Thread
from queue import Queue

import logging, os
import statistics

import argparse
import configparser
import json

from common import Datetool, Calendar, Room
from ade import Ade

MAJOR_VER = 0
MINOR_VER = 1

serverPort = -1
adeServer=""
adeLogin=""
adePassword=""
adeProjectId=""

refreshTime=15*60  #15 minutes 

authorizedGateways=[]        # list of authorized gateways, given in config file
roomsList:Room = []          # list of rooms, given in config file
authentifiedDisplays = [{}]  # list (dictionnary) of accepted display, with corresponding room

DEFAULT_CONFIG_FILE = ["./server.conf",
                       "/home/dimercur/Travail/git/afficheurs/software/server-new/src/server.conf",
                       "/etc/smartdisplay/server.conf"]

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
    global refreshTime
    global authorizedGateways
    global roomsList
    
    config = configparser.ConfigParser()
    if config.read(confFile) == []:
        log.error('Unable to open configuration file "' + confFile + '"')
        return False
    
    #print (config.sections())
    if not 'server' in config:
        log.error('No "server" section in configuration file')
        return False
    
    try:
        serverPort = int(config['server']['port'])
        adeServer = config['server']['adeserver']
        adeLogin = config['server']['adelogin']
        adePassword = config['server']['adepassword']
        refreshTime = int(config['server']['refreshtime'])*60
    except Exception as e:
        log.error('Misformatted configuration file for "server" section(' + str(e) + ')')
        return False
    
    try:
        authorizedGateways = json.loads(config.get("authorized gateways","gw_list"))
    except Exception as e:
        log.error('Misformatted configuration file for "authorized gateways" section (' + str(e) + ')')
        return False
    
    try:
        for key in config:
            if key != "server" and key != "authorized gateways" and key != "DEFAULT":
                #print (key)
                #print (config[key]['ade_pattern'])
                #print (config[key]['displays'])
                #print()
                room = Room(key, config[key]['ade_pattern'],[],[])
                room.calendars=[]
                room.ressourcesId=[]
                
                #print (json.loads(config[key]['displays']))
                displays = json.loads(config[key]['displays'])
                if type(displays)==list:
                    for d in displays:
                        room.displayId.append(int(d, 16))
                else:
                    room.displayId.append(int(displays, 16))
                    
                #print (room)
                roomsList.append(room)
    except Exception as e:
        log.error('Misformatted configuration file for rooms sections (' + str(e) + ')')
        return False
    
    #for r in roomsList:
    #    print (r)
        
    log.info("Configuration read from " + str(confFile)+ ": Ok")
    return True

def updateCalendars():
    global roomsList
    global adeServer
    global adeLogin
    global adePassword
    
    #Retrieve ressources, room information and calendars from ade
    adeSession=Ade()
    adeSession.connect(adeServer, adeLogin, adePassword)
    projectsList = adeSession.getProjectsList()
    
    adeProjectId = projectsList[0][0]
    log.info("Ade project id: " + str(adeProjectId))
    
    adeSession.selectProject(adeProjectId)
    ressources = adeSession.getRessourcesList()
    
    for res in ressources:
        for room in roomsList:
            if room.adePattern.upper() in res.name.upper():
                room.ressourcesId.append(res.id)
                
    # Get calendar for current week   
    for room in roomsList:
        for res in room.ressourcesId:
            cal = adeSession.getRessourceCalendarByDate(res,
                                                        Datetool.getFirstDayofWeek(),
                                                        Datetool.getLastDayofWeek())
            
            for c in cal:
                #if not c in room.calendars:
                    room.calendars.append(c)
               
        #room.calendars.sort()
        room.calendars = Calendar.cleanup(room.calendars)
        
def main():
    global roomsList
    
    # Get command line information
    configFile = parseCommandLine()
    
    # read configuration file
    configOk=False
    if configFile != None:
        if getConfiguration(configFile):
            configOk=True
    else:
        for f in DEFAULT_CONFIG_FILE:
            if getConfiguration(f):
                configOk=True
                break
    
    if not configOk:
        exit(1)
      
    # Initialize calendar information
    updateCalendars()
                     
    for room in roomsList:
        print()
        print (str(room))
        for cal in room.calendars:
            print(str(cal))

if __name__ == '__main__':
    print ("SmartDoors server ver " + str(MAJOR_VER)+"." + str(MINOR_VER))
    
    main()
    
    log.info("Server stopped")