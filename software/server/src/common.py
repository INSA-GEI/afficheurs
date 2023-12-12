# common.py
#
#

from datetime import datetime, date, timedelta, time
#from functools import total_ordering

#import json

# Classe de test
class __TESTS__:
    testsDate=0
    testCalendar=1

if __TESTS__.testsDate:
    currentDay = date(2023,11,29)

# Class used for date and time manipulation           
class Datetool:
    """Static class for date and time manipulation specific with this project """

    @staticmethod
    def getCurrentTimeFromMidnight()->int:
        return datetime.now().hour*60 + datetime.now().minute

    @staticmethod
    def getCurrentDay()->date:
        """Return a date object corresponding to current date and time. """
        if __TESTS__.testsDate:
            date_obj = currentDay
        else:
           date_obj = date(datetime.now().year, datetime.now().month, datetime.now().day)

        return date_obj
    
    @staticmethod
    def dateToStr(date_obj: date)->str:
        """Convert date object to string with format dd/mm/yy used by door screens."""
        return str(date_obj.day) + '/' + str(date_obj.month) + '/' + str(date_obj.year)
    
    @staticmethod
    def getCurrentWeek()->int:
        """Return current week number (from 1 to 52)."""
        date_obj = Datetool.getCurrentDay()
        currentWeek = date_obj.isocalendar()[1]
        return currentWeek
    
    @staticmethod
    def getFirstDayofWeek()->datetime:
        """Return first day (monday) of current week."""
        date_obj = Datetool.getCurrentDay()

        start_of_week = date_obj - timedelta(days=date_obj.weekday())  # Monday
        return start_of_week

    @staticmethod
    def getLastDayofWeek()->datetime:
        """Return last day (sunday) of current week."""
        end_of_week = Datetool.getFirstDayofWeek() + timedelta(days=6)  # Sunday
        
        return end_of_week
    
    @staticmethod
    def getDateStrForURL(day:datetime)->str:
        """Convert datetime object in string with format YY-MM-DD to be used in URL for calendar requests"""
        return day.strftime("%Y-%m-%d")

    @staticmethod
    def getDaysofWeek()->str:
        s=""
        s+=datetime.strptime(str(Datetool.getFirstDayofWeek()), '%Y-%m-%d').strftime('%d/%m/%Y')  # Monday
        s+=";"+datetime.strptime(str(Datetool.getFirstDayofWeek() + timedelta(days=1)), '%Y-%m-%d').strftime('%d/%m/%Y')  
        s+=";"+datetime.strptime(str(Datetool.getFirstDayofWeek() + timedelta(days=2)), '%Y-%m-%d').strftime('%d/%m/%Y') #Wednesday
        s+=";"+datetime.strptime(str(Datetool.getFirstDayofWeek() + timedelta(days=3)), '%Y-%m-%d').strftime('%d/%m/%Y') 
        s+=";"+datetime.strptime(str(Datetool.getFirstDayofWeek() + timedelta(days=4)), '%Y-%m-%d').strftime('%d/%m/%Y') #Friday
        s+=";"+datetime.strptime(str(Datetool.getFirstDayofWeek() + timedelta(days=5)), '%Y-%m-%d').strftime('%d/%m/%Y') #Saturday
        s+=";"+datetime.strptime(str(Datetool.getFirstDayofWeek() + timedelta(days=6)), '%Y-%m-%d').strftime('%d/%m/%Y') #Sunday
    
        return s

    @staticmethod
    def getDayNumber(date_obj: date)->int:
        return date_obj.isocalendar()[2] # 1 = Lundi, 7 = Dimanche
    
    @staticmethod
    def toDate(ade_date: str)->datetime:
        date_obj = datetime.strptime(ade_date, '%Y%m%d').date()
        return date_obj
    
    @staticmethod
    def toTime(ade_time: str)->datetime:
        time_obj = datetime.strptime(ade_time, '%H%M%S').time()
        return time_obj
    
    @staticmethod
    def minutsFromMidnight(ade_time: str)->int:
        date_obj = datetime.strptime(ade_time, '%H%M%S').time()
        minuts_since_midnight = date_obj.hour*60 + date_obj.minute

        return int(minuts_since_midnight)
    
    @staticmethod
    def minutsFromMidnighttoStr(minuts: int)->str:
        time_obj = time(int(minuts/60),\
                        int(minuts - int(minuts/60)*60))

        return time_obj.strftime("%H:%M")

# Class used for storing room reservation
#@total_ordering
class Calendar():
    firstHour:int
    lastHour:int
    day:str
    title:str
    trainees = []
    instructors = []

    def __init__(self, day, title, firstHour, lastHour):
        self.day = day
        self.title = title
        self.firstHour = firstHour
        self.lastHour = lastHour
        self.trainees = []
        self.instructors = []
        
    def __repr__(self):
        return repr((self.day, self.firstHour, self.lastHour, self.title, self.trainees, self.instructors))
    
    def addTrainee(self, trainee):
        self.trainees.append(trainee)
        
    def addInstructor(self, instructor):
        self.instructors.append(instructor)
    
    def __str__(self):
        s = str(self.day) + " : " + self.title + \
            " (" + Datetool.minutsFromMidnighttoStr(self.firstHour) + \
            "-" + Datetool.minutsFromMidnighttoStr(self.lastHour) + ") \n\t\tTrainees: [" 
        
        for t in self.trainees:
            s = s + t + ", "
            
        s = s + "]\n\t\tInstructors: [" 
        
        for i in self.instructors:
            s =s + i + ", "
        
        s= s + "]"
        return s
        
    def __eq__(self, other):
        if not isinstance(other, Calendar):
            return False
        else:
            try :
                val=True
                
                if ((self.firstHour != other.firstHour) or 
                    (self.lastHour != other.lastHour) or 
                    (self.day != other.day) or 
                    (self.title != other.title)):
                    val =False
                else:
                    self.trainees.sort()
                    other.trainees.sort()
                    
                    if (self.trainees != other.trainees):
                        val =False
                    else:
                        self.instructors.sort()
                        other.instructors.sort()
                        
                        if (self.instructors != other.instructors):
                            val =False
                    
                return val
            except:
                return False
        
    def __ne__(self,other):
        return not self == other
    
    def __lt__(self, other):
        if not isinstance(other, Calendar):
            return False
        else:
            try :
                val=False
                
                if Datetool.toDate(self.day) < Datetool.toDate(other.day):
                    val=True
                elif (self.day == other.day) and (self.firstHour < other.firstHour):
                    val=True
                elif ((self.day == other.day) and (self.firstHour == other.firstHour) and (self.lastHour < other.lastHour)):
                    val=True
                    
                return val
            except:
                return False
        
    def isMergeable(self, other):
        if not isinstance(other, Calendar):
            return False
        else:
            try :
                val=True
                
                # Si l'heure de debut, de fin, le jour ou le titre ne sont pas identiques
                # le calendrier n'est pas fusionnable

                if ((self.firstHour != other.firstHour) or  
                    (self.lastHour != other.lastHour) or 
                    (self.day != other.day) or 
                    (self.title != other.title)):
                    val =False
                else:
                    val =True
                    
                return val
            except:
                return False
            
    def dif(a, b):
        return [i for i in range(len(a)) if a[i] != b[i]]
    
# Class used for quick access to display information
class Display():
    id:int
    lastSeen:int
    rfPower:int
    network16bitAddr:int

    def __init__(self, id:int):
        self.id=id
        self.lastSeen=-1
        self.rfPower=-1
        self.batterylevel=-1
        self.network16bitAddr=0xFFFF
        
    def __str__(self)->str:
        s = hex(self.id) +\
            "\n\tRF power: " + str(self.rfPower) +\
            "\n\tBattery: " + str(self.batterylevel) + "\n\t16Bit address: " + hex(self.network16bitAddr) + "\n\tlast seen: " + str(self.lastSeen) 
        
        return s

class Ressource:
    id:int
    calendars:list

    def __init__(self, id:int):
        self.id=id
        self.calendars=[]

    def __str__(self)->str:
        s = "\tRessource ID: " + str(self.id) +\
            "\n\tCalendars:"
        
        for c in self.calendars:
            s = s + "\n\t" + str(c) 
        
        return s

    def isCalendarsEqual(self, cal:list)->bool:
        # on ne peut pas comparer directement les deux listes, 
        # les objets les composants sont à des adresses differentes.
        # Ce sont donc les contenus qui doivent etre comparés

        if len(self.calendars) != len(cal):
            return False
        else:
            for index,c in enumerate(self.calendars):
                if c != cal[index]:
                    return False

        return True

# Class used for storing room information  
class Room():
    name:str
    adePattern:str
    type:str
    ressources = []
    displays = []
    mergedCalendars = []
    calendarUpdate:bool

    def __init__(self, name, type, adePattern, ressources, displays):
        self.name = name
        self.type = type
        self.adePattern = adePattern.upper() # est-ce necessaire ?
        self.ressources = ressources
        self.displays = displays
        self.calendarUpdate=False
        self.mergedCalendars=[]
    
    def __str__ (self):
        s = self.name + "[" + self.type +"] [adePattern: " + self.adePattern + "]\n\tAde ressources\n"
        
        for r in self.ressources:
            s = s + "\t\t"+ str(r.id)+"\n" # show only ressource id
        
        s = s + "\tDisplays: ["
        
        for d in self.displays:
            s =s + hex(d.id) + ", "
            
        s= s + "]\n\tMerged calendar:\n"
        
        for c in self.mergedCalendars:
            s = s+ "\t"+str(c.day) + " : " + c.title + \
                " (" + Datetool.minutsFromMidnighttoStr(c.firstHour) + \
                "-" + Datetool.minutsFromMidnighttoStr(c.lastHour) + ") \n\t\tTrainees: [" 
            
            for t in c.trainees:
                s = s + t + ", "
                
            s = s + "]\n\t\tInstructors: [" 
            
            for i in c.instructors:
                s =s + i + ", "
            
            s= s + "]\n"  
        
        s=s +"\n\tCalendar update: " + str(self.calendarUpdate)
        return s
    
    def __repr__(self):
        return repr((self.name, self.adePattern, self.type, self.ressources, self.displays, self.mergedCalendars, self.calendarUpdate))
    
    def hasDisplay(self, displayId:int)->Display:
        for d in self.displays:
            if d.id==displayId:
                return d
            
        return None
    
    def mergeRessourcesCalendars(self)->None:
        from operator import attrgetter
        oldCal= self.mergedCalendars.copy()  # save a copy of previous calendar, in order to compare it later

        self.mergedCalendars.clear() #flush calendar

        for res in self.ressources:
            self.mergedCalendars.extend(res.calendars) #append list with ressource list

        self.mergedCalendars.sort(key=attrgetter('day','firstHour', 'lastHour')) # sort list by day, then firsthour and lastly, lasthour

        # TODO: trouver comment retirer les doublons
        # self.mergedCalendars = list(dict.fromkeys(self.mergedCalendars)) # Remove duplicate elements
        j=0
        while j<len(self.mergedCalendars)-1:
            
            if self.mergedCalendars[j] == self.mergedCalendars[j+1]: 
                # les deux entrées sont exactement identiques => suppression de la deuxieme entrée

                self.mergedCalendars.pop(j+1)

            elif self.mergedCalendars[j].isMergeable(self.mergedCalendars[j+1]):
                # Les deux entrées concernent le même enseignement 
                # mais ont des etudiants ou enseignants differents => fusion des deux entrées

                self.mergedCalendars[j].trainees.extend(self.mergedCalendars[j+1].trainees)
                self.mergedCalendars[j].instructors.extend(self.mergedCalendars[j+1].instructors)

                self.mergedCalendars[j].trainees.sort()
                self.mergedCalendars[j].instructors.sort()

                self.mergedCalendars[j].trainees= list(dict.fromkeys(self.mergedCalendars[j].trainees)) # Remove duplicate elements
                self.mergedCalendars[j].instructors = list(dict.fromkeys(self.mergedCalendars[j].instructors)) # Remove duplicate elements

                self.mergedCalendars.pop(j+1)
                
            else:    
                j=j+1

        self.calendarUpdate=True
        if self.mergedCalendars == oldCal:
            self.calendarUpdate =False  # if mergedcalendar is identical to previous one, no update
        


