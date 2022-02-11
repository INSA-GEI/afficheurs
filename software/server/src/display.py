#
#

from common import Datetool, Room, Rooms
from network import Gateway
from protocol import Protocol

# Class used for display management and information 
class Display():
    name: str = ""
    rooms = []
    displayId: int = 0
    calendars = []
    gateway = None
    calendarChange = False
    
    def __init__(self, name: str, adePattern: str, rooms: Rooms, displayId: int) -> None:
        self.name = name
        self.rooms = rooms
        self.displayId = displayId
        self.calendarChange = False
        
    def __str__ (self):
        return self.name
    
    def setGateway(self, gw) -> None:
        self.gateway = gw
        
    def getGateway(self):
        return self.gateway
        
    def getCalendar(self, ade):    
        firstDay = Datetool.getFirstDayofWeek()
        lastDay = Datetool.getLastDayofWeek()
        
        calendar = []
        
        if len(self.ressourcesId) !=0:
            for res in self.ressourcesId:
                rawCalendar = ade.getRessourceCalendar(res, firstDay, lastDay)
    
                for cal in rawCalendar:
                    calDay = Datetool.toDate(cal.day)
                
                    if calDay>=firstDay and calDay<=lastDay:
                        calendar.append(cal)
        
            self.calendars=calendar
    
    def updateCalendar(self, ade, week):
        if len(self.ressourcesId) !=0:
            pass
