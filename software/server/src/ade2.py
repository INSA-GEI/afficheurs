# ade2.py
#
#

from common import Calendar, Datetool, Room, Ressource, Display
import logging
import requests
from datetime import datetime
import json
from config import Config

class ADEServerError(Exception):
    """Exception raised for errors when connecting to ADE server.
    """

    def __init__(self, code:str):
        self.message = "Error code when connecting to ADE server : " + str(code)
        super().__init__(self.message)

class ADEInvalidJSON(Exception):
    """Exception raised for errors when parsing answer from server.
    """

    def __init__(self):
        self.message = "Error when parsing answer from server : invalid JSON"
        super().__init__(self.message)

class ADERoomNotInList(Exception):
    """Exception raised for errors when searching ID of a given room.
    """

    def __init__(self, room:str):
        self.message = "Room " + room + " not found in list"
        super().__init__(self.message)

class Ade2:
    roomsList:Room = []
    debugFlag=False
    configuration:Config=None
    jsonRoomsList=None

    # misc
    firstWeekNbr = 31
    
    def __init__(self, roomslist:list, configuration:Config, debugFlag=False):
        self.debugFlag = debugFlag
        self.roomsList = roomslist
        self.configuration = configuration
        self.jsonRoomsList = []

    def getRoomsList(self)->None:
        cmd=self.configuration.adeServer+"/getRooms.jsp?projectId="+str(self.configuration.adeProjectId)+\
            "&login="+str(self.configuration.adeLogin)+\
            "&password="+str(self.configuration.adePassword)

        r = requests.get(cmd)
            
        if self.debugFlag == True:
            print(cmd)
            print(r)
            print(r.text)

        if not "200" in str(r):
            raise ADEServerError(r)
        
        # Parse answer
        try:
            self.jsonRoomsList = json.loads(r.text)
        except Exception:
            raise ADEInvalidJSON

    # Get room ID in a room list  
    def getRoomID(self, room:str)->list:
        id=[]

        for r in self.jsonRoomsList:
            if room.upper() in r['name'].upper():
                if self.debugFlag == True:
                    print('ID for room ' + room + ' is ' + str(r['id']))
            
                id.append(r['id'])
        
        if len(id)==0: # list is empty, no room found
            raise ADERoomNotInList(room)
        else:
            return id
    
    # Warning: firstDate and lastDate in yyyy-mm-dd
    def getRessourceCalendarByDate(self,roomId:str, firstDate:str, lastDate:str)->list:
        
        #convert yyyy-mm-dd based date to mm/dd/yyyy required by adeweb
        #startDay = datetime.strptime(str(firstDate), '%Y-%m-%d').strftime('%m/%d/%Y')
        #endDay = datetime.strptime(str(lastDate), '%Y-%m-%d').strftime('%m/%d/%Y')
        
        cmd = self.configuration.adeServer+"/getEvents.jsp?id="+str(roomId) + \
              "&start=" + str(firstDate) + "&end=" + str(lastDate) + \
              "&projectId="+str(self.configuration.adeProjectId)+\
              "&login="+str(self.configuration.adeLogin)+\
              "&password="+str(self.configuration.adePassword)
        
        r = requests.get(cmd)
        r.encoding='utf-8'

        if self.debugFlag == True:
            print(cmd)
            print(r)
            print (r.text)
        
        if not "200" in str(r):
            raise ADEServerError(r)
        
        # Parse answer
        try:
            jsonCalendar=json.loads(r.text)
        except Exception:
            raise ADEInvalidJSON
        
        # Parse received JSON calendar
        calendar = []
        
        for jcal in jsonCalendar:
            jsonStartDate=jcal['start'].split('T')
            jsonEndDate=jcal['end'].split('T')
            date=Datetool.toDate(jsonStartDate[0])

            cal = Calendar(day=Datetool.getDayNumber(date),
                            title=jcal['title'],
                            firstHour=Datetool.minutsFromMidnight(jsonStartDate[1]),
                            lastHour=Datetool.minutsFromMidnight(jsonEndDate[1]))
            
            trainee = jcal['trainee'].strip().split(' ')

            for t in trainee:
                cal.addTrainee(t)

            instructor = jcal['instructor'].strip().split(' ')
            j=0

            try:
                while j<(len(instructor)-1):
                    fullName=""
                    while instructor[j].isupper():
                        fullName=fullName+instructor[j]+" " # Concatene les noms du prof
                        j=j+1
                    
                    fullName=fullName+instructor[j] # Concatene le nom et le prenom du prof
                    
                    cal.addInstructor(fullName) # rajoute à la liste de prof
                    j=j+1
            except Exception: # Si il y a un probleme dans la concatenation des noms, passe sous silence
                pass
                        
            calendar.append(cal)
                    
        return calendar
    
    # load all calendar for rooms defined in config file
    def getCalendars(self):
        counter = 4
        listRetrieved=False

        while counter!=0 and not listRetrieved:
            try:
                self.getRoomsList()
                listRetrieved=True
            except Exception as e:
                counter=counter-1
                if counter==0:
                    raise e
                
        for room in self.roomsList:
            try:
                pattern = str(room.adePattern)
                ids=self.getRoomID(pattern)

                for id in ids:
                    room.ressources.append(Ressource(id))
            except ADERoomNotInList as e:
                raise(e)
                    
        # Get calendar for current week   
        for room in self.roomsList:
            for res in room.ressources:
                res.calendars.clear()

                startDay=Datetool.getDateStrForURL(Datetool.getFirstDayofWeek())
                endDay=Datetool.getDateStrForURL(Datetool.getLastDayofWeek())

                cal = self.getRessourceCalendarByDate(res.id, startDay, endDay)

                for c in cal:
                    res.calendars.append(c)

    def close(self):
        pass