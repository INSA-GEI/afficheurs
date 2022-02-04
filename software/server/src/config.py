# config.py
#
#

from datetime import date, datetime, time
import yaml
from common import Room
from network import Gateway

class Configuration():
    MAJOR_VERSION = 1
    MINOR_VERSION = 0

    # Configuration files and location
    configFileName = "./software/server/src/sdsd.yml"
    logFileName = "./software/server/src/sdsd.log"
    logFile = ""
    verboseFlag = False
    
    #rooms = []
    #gateways = []
    
    def parseCommandLine(self, cmdline: str) -> None:
        status = False
    
        self.startupPrompt()
        
        for arg in cmdline:
            print ("arg = %s" % (arg))
        
            if arg == '-v':
                self.verboseFlag = True
            elif arg == '-h' or arg == '--help':
                status =True    
            else:
                status = True
            
        if status == True:
            self.helpPrompt()
        
        if status == True :
            raise Exception("Command Line Error","Help requested or invalid parameter")
     
    def parseConfigurationFile(self):
        # parse config file
        try:
            with open(self.configFileName) as f:
                config = yaml.load(f, Loader=yaml.FullLoader)       
            
        except:
            print ("Unable to open configuration file %s" % (self.configFileName))
            exit (-2) 
        
        try:
            self.serverAddr = str(config["server"])
        except:
            print ("Error : missing 'server' entry in configuration file")
            exit(-2)

        try:
            self.serverLogin = str(config["login"])
        except:
            print ("Error : missing 'login' entry in configuration file")
            exit(-2)
        
        try:
            self.serverPassword = str(config["password"])
        except:
            print ("Warning : missing 'password' entry in configuration file, set to '' ")
            self.serverPassword = ""
        
        if self.verboseFlag:      
            print ("Server = ", self.serverAddr)
            print ("Login = ", self.serverLogin)
            print ("Password = ", self.serverPassword)
            
        # Recuperation la liste des ecrans
        rooms = []
        
        for room in config["rooms"]:
            rooms.append(Room(room['name'], room['ade pattern'], [], room['id']))
        
        # Recuperation de la liste des gateways
        gateways= []
        
        i=0   
        for gw in config["gateways"]:
            gateways.append(Gateway(i, gw['name'], gw['address'], None))
            i=i+1        
              
        return  (rooms, gateways)
    
    def startupPrompt(self):
        print ("sdsd ver %d.%d" % (self.MAJOR_VERSION, self.MINOR_VERSION))
        print ()
   
    def helpPrompt(self):
        print ("Command line options:")
        print ("\t-h/--help: this help message")
        print ("\t-v: verbose output")
        print ("\t-c <file>: use <file> as configuration file")
        print ("\t-l <file>: use <file> as log file")
      
        
        