# common.py
#
#

from datetime import datetime, date, timedelta

# Class used for date and time manipulation           
class Datetool:
    @staticmethod
    def getCurrentDay():
        date_obj = date(datetime.now().year, datetime.now().month, datetime.now().day)
        return date_obj
    
    @staticmethod
    def dateToStr(date_obj: date):
        return str(date_obj.day) + '/' + str(date_obj.month) + '/' + str(date_obj.year)
    
    @staticmethod
    def dateToInt(date_obj: date):
        return str(date_obj.day) + '/' + str(date_obj.month) + '/' + str(date_obj.year)
    
    @staticmethod
    def getCurrentWeek():
        date_obj = Datetool.getCurrentDay()
        currentWeek = date_obj.isocalendar()[1]
        return currentWeek
    
    @staticmethod
    def getFirstDayofWeek():
        date_obj = Datetool.getCurrentDay()

        start_of_week = date_obj - timedelta(days=date_obj.weekday())  # Monday
        return start_of_week

    @staticmethod
    def getLastDayofWeek():
        end_of_week = Datetool.getFirstDayofWeek() + timedelta(days=6)  # Sunday
        
        return end_of_week
    
    @staticmethod
    def getDayNumber(date_obj: date):
        return date_obj.isocalendar()[2] # 1 = Lundi, 7 = Dimanche
    
    @staticmethod
    def toDate(ade_date: str):
        date_obj = datetime.strptime(ade_date, '%d/%m/%Y').date()
        return date_obj
    
    @staticmethod
    def toTime(ade_time: str):
        time_obj = datetime.strptime(ade_time, '%H:%M').time()
        return time_obj
    
    @staticmethod
    def minutsFromMidnight(ade_time: str):
        date_obj = Datetool.toTime(ade_time)
        seconds_since_midnight = (date_obj - date_obj.replace(hour=0, minute=0, second=0, microsecond=0)).total_seconds()
        return int(seconds_since_midnight/60)

# Class used for storing room reservation
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
    
# Class used for storing ressources information        
class Ressource():
    def __init__(self, id, name, screenId):
        self.id = id
        self.name = name
        self.screenId = screenId
        
    def __str__(self):
        return str(self.id)+ ":" + str(self.name)
        
# Class used for storing room information  
class Room():
    name =""
    adePattern = ""
    ressourcesId = []
    displayId = []
    calendars = []
    
    def __init__(self, name, adePattern, ressourcesId, displayId):
        self.name = name
        self.adePattern = adePattern.upper() # est-ce necessaire ?
        self.ressourcesId = ressourcesId
        self.displayId = displayId
    
    def __str__ (self):
        s = self.name + " [adePattern: " + self.adePattern + "]\n\tAde ressources: ["
        
        for r in self.ressourcesId:
            s = s + r + ", "
        
        s = s + "]\n\tDisplays: ["
        
        for d in self.displayId:
            s =s + hex(d) + ", "
            
        s= s + "]"
        return s
    
    def getCalendar(self, ade):    
        firstDay = Datetool.getFirstDayofWeek()
        lastDay = Datetool.getLastDayofWeek()
        
        calendar = []
        
        if len(self.ressourcesId) !=0:
            for res in self.ressourcesId:
                rawCalendar = ade.getRessourceCalendar(res)
    
                for cal in rawCalendar:
                    calDay = Datetool.toDate(cal.day)
                
                    if calDay>=firstDay and calDay<=lastDay:
                        calendar.append(cal)
        
            return calendar
    
    def updateCalendar(self, ade, week):
        if len(self.ressourcesId) !=0:
            pass

# Class used for quick access to display information
class Display():
    id: int = 0
    room: Room = None
    
    
    
