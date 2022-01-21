#!/usr/bin/python3
#
# sdsd : Smart Display service daemon
# Serveur pour interroger la base de donnée ADE
# Version: 1.0
# Auteur: S. DI MERCURIO

import requests
import xml.etree.ElementTree as ET
import datetime
import sys

import yaml

# Constants
# Software version
majorvVersion = 1
minorVersion = 0

# Configuration files and location
configFileName = "./software/server/src/sdsd.yml"

# misc
firstWeekNbr = 31
# Fin des constants

# Variables globales
verboseFlag = False

# Fin des variables globales

class Ressources():
    def __init__(self, id, name, screenId):
        self.id = id
        self.name = name
        self.screenId = screenId

class Ade:
    sessionId = ""
    projectId = ""
    
    verboseOutput = False
    
    baseUrl = ""
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
                #print (project)
                #projectId = project.attrib['id']
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
                #if 'GEI' in child.attrib['name']:
                    #if child.attrib['name'] != 'GEI':
                        # print child.attrib['id']+':'+child.attrib['name']
                        ressources.append(Ressources(
                            child.attrib['id'], child.attrib['name'], 0))
        
        return ressources
    
    def close(self):
        self.sessionId=""
        self.projectId=""
        
def adeParser(config):
    # Obtient un sessionId
    baseUrl = 'https://edt.insa-toulouse.fr/jsp/webapi?'
    #baseUrl = config[]
    #cmd = baseUrl+'function=connect&login=adeweb&password=ade423'
    cmd = baseUrl+'function=connect&login=plymobil&password='
    r = requests.get(cmd)
    print(cmd)
    print(r)
    print(r.text)

    root = ET.fromstring(r.text)
    sessionId = root.attrib['id']

    # liste les projets
    cmd = baseUrl+'sessionId='+sessionId+'&function=getProjects&detail=4'
    print(cmd)
    r = requests.get(cmd)
    print(r)
    #print(r.text)

    # a priori, seul le projet 1 existe
    projectId = ''
    root = ET.fromstring(r.text)
    for project in root:
        if project.tag == 'project':
            projectId = project.attrib['id']

    print("Projet selectionné: " + projectId)
    
    # Selectionne le projet projectId
    cmd = baseUrl+'sessionId='+sessionId+'&function=setProject&projectId='+projectId
    print(cmd)
    r = requests.get(cmd)
    print(r)
    #print(r.text)

    # Recupere l'ensemble des ressources (salles)
    cmd = baseUrl+'sessionId='+sessionId+'&function=getResources&tree=false&detail=4'
    print(cmd)
    r = requests.get(cmd)
    print(r)
    #print(r.text)

    root = ET.fromstring(r.text)

    ressources = []

    # Ne garde que les salles du GEI
    for child in root:
        if 'classroom' in child.attrib['category']:
            if 'GEI' in child.attrib['name']:
                if child.attrib['name'] != 'GEI':
                    # print child.attrib['id']+':'+child.attrib['name']
                    ressources.append(Ressources(
                        child.attrib['id'], child.attrib['name']))

    print('Affichage des ressources')
    print(len(ressources))
    #for r in ressources:
    #    print(r.id + ':' + r.name)

    cmd = baseUrl+'sessionId='+sessionId + \
        '&function=getActivities&tree=false&resources=2335&detail=17'
    print(cmd)
    r = requests.get(cmd)
    print(r)

    # Liste tout ce qui est programme en gei 102 (Brut)
    print(r.text)

    root = ET.fromstring(r.text)

    # N'affiche que ce qui est programme semaine courante, avec le numero du jour derriere (de 0 a 6)
    currentWeekNbr = datetime.date(2022,1,19).isocalendar()[1]
    AdeWeekNbr = currentWeekNbr - firstWeekNbr # week 1 in ADE = Week 36 in real life
    if AdeWeekNbr <0:
        AdeWeekNbr = AdeWeekNbr + 52
    
    print ('')
    print ('')
    print ('')
    print ('AdeWeekNbr = ' + str(AdeWeekNbr)) 
    print ('')
    
    for child in root:
        if child.attrib['firstWeek'] == str(AdeWeekNbr):
            print(child.attrib['name'] + ':' + child.attrib['firstDay'] + 
                  ' ['+ child.attrib['firstSlot'] + '-'+ child.attrib['lastSlot']+']')

    print('fin')
    
def startupPrompt():
    print ("sdsd ver %d.%d" % (majorvVersion, minorVersion))
    print ()
   
def helpPrompt():
    print ("Command line options:")
    print ("\t-h/--help: this help message")
    print ("\t-v: verbose output")
     
def main():
    status = False
    verboseFlag = False
    
    for arg in sys.argv[1:]:
        print ("arg = %s" % (arg))
        
        if arg == '-v':
            verboseFlag = True
        elif arg == '-h' or arg == '--help':
            status =True
        else:
            status = True
            
    if status == True:
        helpPrompt()
        exit (-1)
        
    # parse config file
    try:
        with open(configFileName) as f:
    
            configuration = yaml.load(f, Loader=yaml.FullLoader)
            
            #print(configuration)
            #print("")
    except:
        print ("Unable to open configuration file %s" % (configFileName))
        exit (-2) 
        
    try:
        serverAddr = str(configuration["server"])
    except:
        print ("Error : missing 'server' entry in configuration file")
        exit(-2)
    
    try:
        serverLogin = str(configuration["login"])
    except:
        print ("Error : missing 'login' entry in configuration file")
        exit(-2)
        
    try:
        serverPassword = str(configuration["password"])
    except:
        print ("Warning : missing 'password' entry in configuration file, set to '' ")
        serverPassword = ""
     
    if verboseFlag:      
        print ("Server = ", serverAddr)
        print ("Login = ", serverLogin)
        print ("Password = ", serverPassword)
    
    ade=Ade(verboseFlag)
    
    ade.connect(serverAddr,serverLogin,serverPassword)
    if verboseFlag:
        print ("SessionId = %s" % ade.sessionId)
         
    projectList = ade.getProjectsList()
    
    if verboseFlag:
        print(projectList)
        
    if len(projectList)<=0:
        print ("Error: no project in ADE database")
        exit(-3)
        
    ade.selectProject(projectList[0][0]) # Access projectId of first project
    
    ressourcesList = ade.getRessourcesList()
    if verboseFlag:
        for r in ressourcesList:
            print(r.name + " => " + r.id)
        
    if len(ressourcesList)<=0:
        print ("Error: no ressource in ADE database")
        exit(-3)
        
    # Compare à la liste des ecrans
    ressourcesWithScreen = []
    
    for room in configuration["rooms"]:
        for res in ressourcesList:
            if room['name'].upper() in res.name.upper():
                
                resFound = False
                for r in ressourcesWithScreen:
                    if room['name'].upper() == r.name:
                        r.id.append(res.id)
                        resFound=True
                    
                if resFound != True:
                    ressourcesWithScreen.append(Ressources([res.id], room['name'].upper(), int(room['id'], 16)))
                
                
                
    for r in ressourcesWithScreen:
        print(r.name + " (ADE ID = " + str(r.id) + ", SCREEN ID = " + str(r.screenId)+")")
    
# Programme principal
startupPrompt()

main()

