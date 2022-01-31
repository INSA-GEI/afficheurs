#!/usr/bin/python3
#
# sdsd : Smart Display service daemon
# Serveur pour interroger la base de donnée ADE
# Version: 1.0
# Auteur: S. DI MERCURIO

#from re import S
#from tkinter.font import names

import sys
from config import Configuration
from ade import Ade
from common import Datetool,Room
from network import Gateway,GatewayEvents
import threading
import yaml

def receiveCallback(id:int, data: str) -> None:
    print ("Data received from gateway " + Configuration.gateways[id].name +
           ": " + data)
    
def eventCallback(id:int, event: int) -> None:
    if event == GatewayEvents.EVENT_CONNECTED:
        print ("EVENT_CONNECTED received from gateway " + Configuration.gateways[id].name)
        Configuration.gateways[id].write("Calendar data")
    elif event == GatewayEvents.EVENT_CONNECTION_LOST:
        print ("EVENT_CONNECTION_LOST received from gateway " + Configuration.gateways[id].name)
    else:
        print ("UNKNOWN_EVENT received from gateway " + Configuration.gateways[id].name)

def parseConfigurationFile():
    # parse config file
    try:
        with open(Configuration.configFileName) as f:
            configuration = yaml.load(f, Loader=yaml.FullLoader)       
        
    except:
        print ("Unable to open configuration file %s" % (Configuration.configFileName))
        exit (-2) 
    
    try:
        Configuration.serverAddr = str(configuration["server"])
    except:
        print ("Error : missing 'server' entry in configuration file")
        exit(-2)

    try:
        Configuration.serverLogin = str(configuration["login"])
    except:
        print ("Error : missing 'login' entry in configuration file")
        exit(-2)
    
    try:
        Configuration.serverPassword = str(configuration["password"])
    except:
        print ("Warning : missing 'password' entry in configuration file, set to '' ")
        Configuration.serverPassword = ""
    
    if Configuration.verboseFlag:      
        print ("Server = ", Configuration.serverAddr)
        print ("Login = ", Configuration.serverLogin)
        print ("Password = ", Configuration.serverPassword)
        
    # Recuperation la liste des ecrans
    Configuration.rooms = []
    
    for room in configuration["rooms"]:
        Configuration.rooms.append(Room(room['name'], room['ade pattern'], [], room['id']))
    
    # Recuperation de la liste des gateways
    Configuration.gateways= []
     
    i=0   
    for gw in configuration["gateways"]:
        Configuration.gateways.append(Gateway(i, gw['name'], gw['address'], None))
        i=i+1        
               
# Programme principal   
if __name__ == "__main__":
   
    try:
        Configuration.parseCommandLine(sys.argv[1:])
    except:
        print ("Error: Parsing of command line parameters failed")
        exit(-1)
    
    try:
        parseConfigurationFile()
    except:
        print ("error: Parsing of " + Configuration.configFileName + " failed")
        exit(-2)
    
    Configuration.openLogFile()
    ade=Ade()
    
    ade.connect(Configuration.serverAddr,Configuration.serverLogin,Configuration.serverPassword)
    print ("Connect to ADE: OK")
    Configuration.writeLogFile("Connect to ADE: OK")
    Configuration.writeVerbose("SessionId = %s" % ade.sessionId)
        
    for gw in Configuration.gateways:
        gw.receiveCallback= receiveCallback
        gw.eventCallback = eventCallback
        gw.threadId = threading.Thread(target=gw.ReceptionThread, args=())
        gw.threadId.daemon=True
        gw.threadId.start()
      
    try:  
        Configuration.gateways[0].threadId.join()
    except KeyboardInterrupt:
        pass
    
    print ("\n\nAll done")
    exit (0)
    
    projectList = ade.getProjectsList()
    
    Configuration.writeVerbose(projectList)
        
    if len(projectList)<=0:
        print ("Error: no project in ADE database")
        exit(-3)
        
    ade.selectProject(projectList[0][0]) # Access projectId of first project
    
    print ("Get ressources list: ", end = '', flush=True)
    ressourcesList = ade.getRessourcesList()
    for r in ressourcesList:
        Configuration.writeVerbose(r.name + " => " + r.id)
        
    if len(ressourcesList)<=0:
        print ("Error: no ressource in ADE database")
        Configuration.writeLogFile("Error: no ressource in ADE database")
        exit(-3)
        
    print ("OK")
    Configuration.writeLogFile("Get ressources list: OK")
    
    # Compare à la liste des ecrans
    for room in Configuration.rooms:
        for res in ressourcesList:
            if room.adePattern in res.name.upper():
                room.ressourcesId.append(res.id)
    
    if Configuration.verboseFlag:
        for r in Configuration.rooms:
            Configuration.writeLogFile(r)
    
    for room in Configuration.rooms:
        if len(room.ressourcesId) == 0:
            print ("Warning: room " + room.name + " has no ressource ID in ADE")
            Configuration.writeLogFile("Warning: room " + room.name + " has no ressource ID in ADE")
            
    print("Get rooms calendar: ", end="", flush=True)
    for room in Configuration.rooms:
        room.getCalendar(ade)
        
    print("OK")
    Configuration.writeLogFile("Get rooms calendar: OK")
    
    Configuration.verboseFlag = True
    
    if Configuration.verboseFlag:
        for r in Configuration.rooms:
            Configuration.writeLogFile("")
            Configuration.writeLogFile(str(r))
            for c in r.calendars:
                Configuration.writeLogFile(str(c))

