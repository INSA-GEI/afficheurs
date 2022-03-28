# ade.py
#
#

from common import Ressource, Calendar
import logging
import requests
import xml.etree.ElementTree as ET
from datetime import datetime

class Ade:
    sessionId = ""
    projectId = ""
    baseUrl = ""
    debugFlag=False
      
    # misc
    firstWeekNbr = 31
    
    def __init__(self):
        pass
        
    def connect(self, serverAddr, login, password):
         # Obtient un sessionId
        self.baseUrl = serverAddr+'/jsp/webapi?'
        cmd = self.baseUrl+'function=connect&login='+login+'&password='+password
        r = requests.get(cmd)
        
        if self.debugFlag == True:
            print(cmd)
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
            
        if self.debugFlag == True:
            print(cmd)
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
        
        if self.debugFlag == True:
            print(cmd)
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
        
        if self.debugFlag == True:
            print(cmd)
            print(r)
            print(r.text)

        root = ET.fromstring(r.text)

        ressources = []

        # Ne garde que les salles de classe
        for child in root:
            if 'classroom' in child.attrib['category']:
                ressources.append(Ressource(child.attrib['id'], child.attrib['name'], 0))
        
        return ressources
    
    def getRessourceCalendar(self,ressource):
        cmd = self.baseUrl+'sessionId='+self.sessionId + \
            '&function=getActivities&tree=false&resources='+ressource+'&detail=17'
        r = requests.get(cmd,)
        r.encoding='utf-8'

        if self.debugFlag == True:
            print(cmd)
            print(r)
            print (r.text)
        
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
    
    #Warning: firstDate and lastDate in yyyy-mm-dd
    def getRessourceCalendarByDate(self,ressource,firstDate, lastDate):
        
        #convert yyyy-mm-dd based date to mm/dd/yyyy required by adeweb
        startDay = datetime.strptime(str(firstDate), '%Y-%m-%d').strftime('%m/%d/%Y')
        endDay = datetime.strptime(str(lastDate), '%Y-%m-%d').strftime('%m/%d/%Y')
        
        cmd = self.baseUrl+'sessionId='+self.sessionId + \
            '&function=getEvents&resources='+ressource+'&startDate='+startDay+'&endDate='+endDay+'&detail=8'
        r = requests.get(cmd,)
        r.encoding='utf-8'

        if self.debugFlag == True:
            print(cmd)
            print(r)
            print (r.text)
        
        root = ET.fromstring(r.text)

        calendar = []
        
        for event in root.findall('event'):
            cal = Calendar(day=event.attrib['date'],
                            title=event.attrib['name'],
                            firstHour=event.attrib['startHour'],
                            lastHour=event.attrib['endHour'])
            
            for eventParticipants in event.findall('resources'):
                for eventParticipant in eventParticipants.findall('resource'):
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
