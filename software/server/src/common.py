# common.py
#
#

from datetime import datetime, date, timedelta

Room = tuple[str, int]
Rooms = list[Room]

# Class used for logging information in log file or on screen
class Log:
    logFile = ""
    verboseFlag = False
    isLogOpen= False
    
    @staticmethod
    def openLogFile(conf) -> None:
        try:
            Log.logFile = open(conf.logFileName, "a")
            Log.logFile.write("\n---------------------------------------------------------")
            Log.logFile.write("\nsdsd ver %d.%d" % (conf.MAJOR_VERSION, conf.MINOR_VERSION))
            Log.logFile.write("\nStart: " + str(datetime.now()))
            Log.logFile.write("\n\n")
            
            Log.isLogOpen=True
        except:
            print ("Warning: Unable to open log " + conf.logFileName)
            Log.isLogOpen=False
     
    @staticmethod
    def writeLogFile(s):
        if Log.isLogOpen:
            Log.logFile.write(s+"\n")
            Log.logFile.flush()
            
    @staticmethod
    def closeLogFile():
        if Log.isLogOpen:
            Log.logFile.close()
            
    @staticmethod
    def writeVerbose( s):
        if Log.verboseFlag:
            print (s)
 
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
    
# Class used for storing ressources information        
class Ressources():
    def __init__(self, id, name, screenId):
        self.id = id
        self.name = name
        self.screenId = screenId

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

# # Class used for storing room information  
# class Room():
#     name =""
#     adePattern = ""
#     ressourcesId = []
#     displayId = []
#     calendars = []
    
#     def __init__(self, name, adePattern, ressourcesId, displayId):
#         self.name = name
#         self.adePattern = adePattern.upper()
#         self.ressourcesId = ressourcesId
#         self.displayId = displayId
    
#     def __str__ (self):
#         s = self.name + " [adePattern: " + self.adePattern + "]\n\tAde ressources: ["
        
#         for r in self.ressourcesId:
#             s = s + r + ", "
        
#         s = s + "]\n\tDisplays: ["
        
#         for d in self.displayId:
#             s =s + d + ", "
            
#         s= s + "]"
#         return s
    
#     def getCalendar(self, ade):    
#         firstDay = Datetool.getFirstDayofWeek()
#         lastDay = Datetool.getLastDayofWeek()
        
#         calendar = []
        
#         if len(self.ressourcesId) !=0:
#             for res in self.ressourcesId:
#                 rawCalendar = ade.getRessourceCalendar(res)
    
#                 for cal in rawCalendar:
#                     calDay = Datetool.toDate(cal.day)
                
#                     if calDay>=firstDay and calDay<=lastDay:
#                         calendar.append(cal)
        
#             self.calendars=calendar
    
#     def updateCalendar(self, ade, week):
#         if len(self.ressourcesId) !=0:
#             pass

