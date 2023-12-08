# config.py
#
#

import argparse
import configparser

from common import Room, Display, __TESTS__
import logging
import json

class Config:
    configFiles= []

    adeServer:str
    adeLogin:str
    adePassword:str
    adeProjectId:int

    rf_panId:int

    refreshTime:int
    refreshStartTime:int
    refreshEndTime:int

    reportLog:str
    errorLog:str

    log:None

    DEFAULT_CONFIG_FILE = ["./software/server/src/server.conf",
                        "./server.conf",
                        "/home/dimercur/Travail/git/afficheurs/software/server-new/src/server.conf",
                        "/etc/smartdisplay/server.conf"]

    def __init__ (self, log:logging):
        self.configFiles = self.DEFAULT_CONFIG_FILE
        self.log = log

    def parseCommandLine(self) -> list:
        conf = self.configFiles
        parser = argparse.ArgumentParser()
        parser.add_argument('-c', metavar='configfile', help='Configuration file')
        
        args = parser.parse_args()
        if args.c != None:
            conf.append(args.c)
        
        return conf
        
    def getConfiguration(self, fileList)->list:
        roomsList:Room= []
        
        config = configparser.ConfigParser()

        if config.read(fileList) == []:
            errstr= 'Unable to open configuration files "' + str(fileList) + '"'
            self.log.info(errstr)
            raise Exception(errstr)
        
        if not 'server' in config:
            errstr= "No \"server\" section in configuration files {}".format(str(fileList))
            self.log.error(errstr)
            raise Exception (errstr)
        
        try:
            self.adeServer = config['server']['adeserver']
            self.adeLogin = config['server']['adelogin']
            self.adePassword = config['server']['adepassword']
            self.refreshTime = int(config['server']['refreshtime'],base=10)
            self.refreshStartTime = int(config['server']['refreshstarttime'],base=10)
            self.refreshEndTime = int(config['server']['refreshendtime'],base=10)
            self.rf_panId = int(config['server']['rf_panid'],base=16)
            
            self.reportLog = config['server']['reportlog'] 
            self.errorLog = config['server']['errorlog'] 
        except Exception as e:
            errstr = 'Misformatted configuration file for "server" section(' + str(e) + ')'
            self.log.error(errstr)
            raise Exception(errstr)
        
        try:
            self.adeProjectId = int(config['server']['adeproject'],base=10)
        except Exception as e:
            pass

        try:
            for key in config:
                if key != "server" and key != "DEFAULT":
                    room = Room(key, config[key]['type'], config[key]['ade_pattern'],[],[])
                    room.calendars=[]
                    room.ressources=[]
                    
                    displays = json.loads(config[key]['displays'])
                    if type(displays)==list:
                        for d in displays:
                            room.displays.append(Display(int(d, 16)))
                    else:
                        room.displays.append(Display(int(displays, 16)))
                        
                    roomsList.append(room)
        except Exception as e:
            errstr='Misformatted configuration file for rooms sections (' + str(e) + ')'
            self.log.error(errstr)
            raise (errstr)
            
        self.log.info("Configuration read from " + str(fileList) + ": Ok")

        return roomsList
