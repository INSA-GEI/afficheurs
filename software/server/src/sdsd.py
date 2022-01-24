#!/usr/bin/python3
#
# sdsd : Smart Display service daemon
# Serveur pour interroger la base de donnée ADE
# Version: 1.0
# Auteur: S. DI MERCURIO

from re import S
from tkinter.font import names
import requests
import xml.etree.ElementTree as ET
from datetime import date, datetime, time
import sys

import yaml

class Configuration():
    majorvVersion = 1
    minorVersion = 0

    # Configuration files and location
    configFileName = "./software/server/src/sdsd.yml"
    #logFileName = "/var/log/sdsd.log"
    logFileName = "./software/server/src/sdsd.log"
    logFile = ""
    verboseFlag = False
    
    rooms = []
    
    def __init__(self, cmdline):
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
                configuration = yaml.load(f, Loader=yaml.FullLoader)       
                #print(configuration)
                #print("")
            
        except:
            print ("Unable to open configuration file %s" % (self.configFileName))
            exit (-2) 
        
        try:
            self.serverAddr = str(configuration["server"])
        except:
            print ("Error : missing 'server' entry in configuration file")
            exit(-2)
    
        try:
            self.serverLogin = str(configuration["login"])
        except:
            print ("Error : missing 'login' entry in configuration file")
            exit(-2)
        
        try:
            self.serverPassword = str(configuration["password"])
        except:
            print ("Warning : missing 'password' entry in configuration file, set to '' ")
            self.serverPassword = ""
     
        if self.verboseFlag:      
            print ("Server = ", self.serverAddr)
            print ("Login = ", self.serverLogin)
            print ("Password = ", self.serverPassword)
            
        # Recupere la liste des ecrans
        self.rooms = []
        
        for room in configuration["rooms"]:
            self.rooms.append(Room(room['name'], room['ade pattern'], [], room['id']))
        
    def startupPrompt(self):
        print ("sdsd ver %d.%d" % (self.majorvVersion, self.minorVersion))
        print ()
   
    def helpPrompt(self):
        print ("Command line options:")
        print ("\t-h/--help: this help message")
        print ("\t-v: verbose output")
        print ("\t-c <file>: use <file> as configuration file")
        print ("\t-l <file>: use <file> as log file")
        
    def openLogFile(self):
        try:
            self.logFile = open(self.logFileName, "a")
            self.logFile.write("\n---------------------------------------------------------")
            self.logFile.write("\nsdsd ver %d.%d" % (self.majorvVersion, self.minorVersion))
            self.logFile.write("\nStart: " + str(datetime.now()))
            self.logFile.write("\n\n")
        except:
            print ("Warning: Unable to open log " + self.logFile)
        
    def writeLogFile(self, s):
        try:
            self.logFile.write(s+"\n")
        except:
            pass
        
    def closeLogFile(self):
        try:
            self.logFile.close()
        except:
            pass
        
    def writeVerbose(self, s):
        if self.verboseFlag:
            print (s)
        
class Ressources():
    def __init__(self, id, name, screenId):
        self.id = id
        self.name = name
        self.screenId = screenId

class Calendar():
    firstHour=0
    lastHour=0
    day=""
    title=""
    trainees = []
    instructors = []
    
    def __init__(self, day, title, firstHour, lastHour):
        self.day = day
        self.title = title
        self.firstHour = firstHour
        self.lastHour = lastHour
        self.trainees = []
        self.instructors = []
        
    def addTrainee(self, trainee):
        self.trainees.append(trainee)
        
    def addInstructor(self, instructor):
        self.instructors.append(instructor)
    
    def __str__(self):
        s = self.day + " : " + self.title + " (" + self.firstHour + "-" + self.lastHour + ") \n\tTrainees: [" 
        
        for t in self.trainees:
            s = s + t + ", "
            
        s = s + "]\n\tInstructors: [" 
        
        for i in self.instructors:
            s =s + i + ", "
        
        s= s + "]"
        return s
        
class Ade:
    sessionId = ""
    projectId = ""
    baseUrl = ""
    verboseOutput = False
    
    # misc
    firstWeekNbr = 31
    
    def __init__(self, verboseOutput):
        self.verboseOutput = verboseOutput
        
    def connect(self, serverAddr, login, password):
         # Obtient un sessionId
        self.baseUrl = serverAddr+'/jsp/webapi?'
        cmd = self.baseUrl+'function=connect&login='+login+'&password='+password
        r = requests.get(cmd)
        
        if self.verboseOutput == True:
            #print(cmd)
            print(r)
            print(r.text)

        root = ET.fromstring(r.text)
        self.sessionId = root.attrib['id']
    
    def getProjectsList(self):
        if self.sessionId == "":
            raise Exception("ADE Error","Session ID not defined")
        
        # liste les projets
        cmd = self.baseUrl+'sessionId='+self.sessionId+'&function=getProjects&detail=4'
        r = requests.get(cmd)
            
        if self.verboseOutput:
            #print(cmd)
            print(r)
            print(r.text)

        # a priori, seul le projet 1 existe
        projectsList = []
            
        root = ET.fromstring(r.text)
        for project in root:
            if project.tag == 'project':
                projectsList.append ((project.attrib['id'], project.attrib['name']))
            
        return projectsList
    
    def selectProject(self, projectId):
        if self.sessionId == "":
            raise Exception("ADE Error","Session ID not defined")
        
        self.projectId = projectId
        
        # Selectionne le projet projectId
        cmd = self.baseUrl+'sessionId='+self.sessionId+'&function=setProject&projectId='+self.projectId
        r = requests.get(cmd)
        
        if self.verboseOutput:
            #print(cmd)
            print(r)
            print(r.text)
            
    def getRessourcesList(self):  
        if self.sessionId == "":
            raise Exception("ADE Error","Session ID not defined")
        
        if self.projectId == "":
            raise Exception("ADE Error","Project ID not defined")
        
        # Recupere l'ensemble des ressources (salles)
        cmd = self.baseUrl+'sessionId='+self.sessionId+'&function=getResources&tree=false&detail=3'
        r = requests.get(cmd)
        if self.verboseOutput:
            #print(cmd)
            print(r)
            print(r.text)

        root = ET.fromstring(r.text)

        ressources = []

        # Ne garde que les salles de classe
        for child in root:
            if 'classroom' in child.attrib['category']:
                ressources.append(Ressources(child.attrib['id'], child.attrib['name'], 0))
        
        return ressources
    
    def getRessourceCalendar(self,ressource):
        cmd = self.baseUrl+'sessionId='+self.sessionId + \
            '&function=getActivities&tree=false&resources='+ressource+'&detail=17'
        r = requests.get(cmd,)
        r.encoding='utf-8'

        if self.verboseOutput:
            #print(cmd)
            print(r)
            print(r.text)
        
        root = ET.fromstring(r.text)

        calendar = []
        
        for activity in root.findall('activity'):
            for events in activity.findall('events'):
                for event in events.findall('event'):
                    cal = Calendar(day=event.attrib['date'],
                                   title=event.attrib['name'],
                                   firstHour=event.attrib['startHour'],
                                   lastHour=event.attrib['endHour'])
                    
                    for eventParticipants in event.findall('eventParticipants'):
                        for eventParticipant in eventParticipants.findall('eventParticipant'):
                            participant = eventParticipant.attrib['name']
                            if eventParticipant.attrib['category'] == "trainee":
                                cal.addTrainee(participant)
                            elif eventParticipant.attrib['category'] == "instructor":
                                cal.addInstructor(participant)
                                
                    calendar.append(cal)
                    
        return calendar
    
    def close(self):
        self.sessionId=""
        self.projectId=""
        self.baseUrl = ""
 
class Room():
    name =""
    adePattern = ""
    ressourcesId = []
    displayId = []
    calendars = []
    
    def __init__(self, name, adePattern, ressourcesId, displayId):
        self.name = name
        self.adePattern = adePattern.upper()
        self.ressourcesId = ressourcesId
        self.displayId = displayId
    
    def __str__ (self):
        s = self.name + " [adePattern: " + self.adePattern + "]\n\tAde ressources: ["
        
        for r in self.ressourcesId:
            s = s + r + ", "
        
        s = s + "]\n\tDisplays: ["
        
        for d in self.displayId:
            s =s + d + ", "
            
        s= s + "]"
        return s
    
    def getCalendar(self, ade, days):
        if len(self.ressourcesId) !=0:
            calendar = ade.getRessourceCalendar(self.ressourcesId[0])
    
            #for cal in calendar:
            #    print (cal)
    
    def updateCalendar(self, ade, week):
        if len(self.ressourcesId) !=0:
            pass
       
def main():
    try:
        conf = Configuration(sys.argv[1:])
    except:
        exit(-1)
    
    try:
        conf.parseConfigurationFile()
    except:
        exit(-2)
    
    conf.openLogFile()
    ade=Ade(conf.verboseFlag)
    
    ade.connect(conf.serverAddr,conf.serverLogin,conf.serverPassword)
    print ("Connect to ADE: OK")
    conf.writeLogFile("Connect to ADE: OK")
    conf.writeVerbose("SessionId = %s" % ade.sessionId)
         
    projectList = ade.getProjectsList()
    
    conf.writeVerbose(projectList)
        
    if len(projectList)<=0:
        print ("Error: no project in ADE database")
        exit(-3)
        
    ade.selectProject(projectList[0][0]) # Access projectId of first project
    
    print ("Get ressources list: ", end = '', flush=True)
    ressourcesList = ade.getRessourcesList()
    for r in ressourcesList:
        conf.writeVerbose(r.name + " => " + r.id)
        
    if len(ressourcesList)<=0:
        print ("Error: no ressource in ADE database")
        conf.writeLogFile("Error: no ressource in ADE database")
        exit(-3)
        
    print ("OK")
    conf.writeLogFile("Get ressources list: OK")
    
    # Compare à la liste des ecrans
    for room in conf.rooms:
        for res in ressourcesList:
            if room.adePattern in res.name.upper():
                room.ressourcesId.append(res.id)
    
    #for r in conf.rooms:
    #    print (r)
    
    for room in conf.rooms:
        if len(room.ressourcesId) == 0:
            print ("Warning: room " + room.name + " has no ressource ID in ADE")
            conf.writeLogFile("Warning: room " + room.name + " has no ressource ID in ADE")
            
    print("Get rooms calendar: ", end="", flush=True)
    for room in conf.rooms:
        room.getCalendar(ade,1)
        
    print("OK")
    conf.writeLogFile("Get rooms calendar: OK")
    
# Programme principal

main()

