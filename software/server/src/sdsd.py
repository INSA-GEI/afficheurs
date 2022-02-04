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
from network import Gateway,GatewayEvents
from state_machine import StateMachineMessage,StateMachine
from common import Log

# Creer la machine a etat qui va gerer les differents messages et changement d'etat
mae = StateMachine()

# configuration object
config = None

# rooms and displays list
rooms= []

# gateways list
gateways= []

# Callback appelé lors de la reception de donnée envoyé par une gateway
def receiveCallback(id:int, data: str) -> None:
    print ("Data received from gateway " + Configuration.gateways[id].name +
           ": " + data)
  
# Callback appelé lors d'un evenement reçu ou produit par une gateway  
def eventCallback(id:int, event: int) -> None:
    if event == GatewayEvents.EVENT_CONNECTED:
        mae.sendMessage(StateMachineMessage.GATEWAY_CONNECTED, 
                                 (gateways[id], "no calendar"))
        
    elif event == GatewayEvents.EVENT_CONNECTION_LOST:
        mae.sendMessage(StateMachineMessage.GATEWAY_DISCONNECTED, 
                                 (gateways[id], ))
    else:
        mae.sendMessage(StateMachineMessage.UNKNOWN_EVENT, ())
           
# Programme principal   
if __name__ == "__main__":
   
    config = Configuration()
    
    try:
        config.parseCommandLine(sys.argv[1:])
    except:
        print ("Error: Parsing of command line parameters failed")
        exit(-1)
    
    try:
        (rooms, gateways) = config.parseConfigurationFile()
    except:
        print ("error: Parsing of " + config.configFileName + " failed")
        exit(-2)
    
    Log.openLogFile(config)
    ade=Ade()
    
    ade.connect(config.serverAddr,config.serverLogin,config.serverPassword)
    print ("Connect to ADE: OK")
    Log.writeLogFile("Connect to ADE: OK")
    Log.writeVerbose("SessionId = %s" % ade.sessionId)
        
    for gw in gateways:
        gw.receiveCallback= receiveCallback
        gw.eventCallback = eventCallback
        gw.startReceptionThread()
      
    try:  
        gateways[0].threadId.join()
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

