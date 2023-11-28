# ade2.py
#
#

from common import Ressource, Calendar, Datetool
import logging
import requests
from datetime import datetime
import json


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
    roomsList = []
    debugFlag=False
      
    # misc
    firstWeekNbr = 31
    
    def __init__(self, debugFlag=False):
        self.debugFlag = debugFlag
        
    def getRoomsList(self):
        cmd = "https://edt.insa-toulouse.fr/jsp/custom/insa/geiSalles/getRooms.jsp?projectId=1&login=portes&password=ecransdeportes"
        r = requests.get(cmd)
            
        if self.debugFlag == True:
            print(cmd)
            print(r)
            print(r.text)

        if not "200" in str(r):
            raise ADEServerError(r)
        
        # Parse answer
        try:
            self.roomsList = json.loads(r.text)
        except Exception:
            raise ADEInvalidJSON
        
    def getRoomID(self, room:str)->str:
        for r in self.roomsList:
            if room.upper() in r['name'].upper():
                if self.debugFlag == True:
                    print('ID for room ' + room + ' is ' + r['id'])
            
                return r['id']
            
        raise ADERoomNotInList(room)
    
    #Warning: firstDate and lastDate in yyyy-mm-dd
    def getRessourceCalendarByDate(self,roomId:str, firstDate:str, lastDate:str)->list:
        
        #convert yyyy-mm-dd based date to mm/dd/yyyy required by adeweb
        #startDay = datetime.strptime(str(firstDate), '%Y-%m-%d').strftime('%m/%d/%Y')
        #endDay = datetime.strptime(str(lastDate), '%Y-%m-%d').strftime('%m/%d/%Y')
        
        cmd ="https://edt.insa-toulouse.fr/jsp/custom/insa/geiSalles/getEvents.jsp?id="+str(roomId) + \
                "&start=" + str(firstDate) + "&end=" + str(lastDate) + \
                "&projectId=1&login=portes&password=ecransdeportes"
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
            j=0;

            try:
                while j<(len(instructor)-1):
                    cal.addInstructor(instructor[j] + " " + instructor[j+1]) # Concatene le nom et le prenom de chaque prof
                    j=j+2
            except Exception: # Si il y a un probleme dans la concatenation des noms, passe sous silence
                pass
                        
            calendar.append(cal)
                    
        return calendar
    
    def close(self):
        pass