# messages.py
#
#

from common import Datetool, Room, Display
import logging

from config import Config

class Transaction:
    cmd:str
    answer:str
    retryCnt:int
    senderAddr:None
    senderId:None
    txSuccess:bool

    def __init__(self) -> None:
        self.answer=None
        self.cmd=None
        self.retryCnt=4
        self.senderAddr=None
        self.senderId=None
        self.txSuccess=False

class Message():
    ANS_OK = "OK"
    ANS_ERR = "ERR"
    ANS_REJECT = "REJECT"
    ANS_ACCEPT = "ACCEPT"
    
    CMD_UNKNOWN = "UNKOWN"
    CMD_JOIN = "JOIN"
    CMD_SETUP="SETUP"
    CMD_GET_CALENDAR="CAL"
    CMD_GET_UPDATE="UPDATE"
    CMD_REPORT="REPORT"
    
    ERR_NO_ACTION = 1
    ERR_INVALID_FRAME = 2
    ERR_ERROR_DECODING_FRAME = 3
    ERR_GW_REFUSED =10
    ERR_DISPLAY_REFUSED =11
  
    configuration:Config= None
    log:logging=None

    def __init__(self,configuration:Config):
        self.configuration=configuration

        self.log = logging.getLogger(__name__)
        self.log.setLevel(self.configuration.loglevel)

    def manage(self, cmd:str, room:Room, display: Display, displayId:int) -> str:
        s=""

        if room is None:
            # Display is not in room list => to be added in configuration file
            self.log.warning("Room not found for display ID [{}]".format(hex(displayId)[2:].upper()))
            s= Message.ANS_REJECT
        else:
            # This case should never happen: type for room is not Room
            if not isinstance(room, Room):
                self.log.error("Invalid type for room {}".format (type(room)))
                raise Exception("Invalid type for room {}".format (type(room)))
            # otherwise, everything is good, let's answer to this command
            else:
                self.log.debug("Message.manage: receive cmd [{}] from room {}".format (cmd, room.name))
                if Message.CMD_JOIN in cmd: 
                    # Reception of JOIN cmd, useless now
                    # let's answer OK !
                    s = self.__cmdJOINManager(room)
                elif Message.CMD_SETUP in cmd:
                    # Reception of SETUP cmd
                    s = self.__cmdSETUPManager(room)
                elif Message.CMD_GET_CALENDAR in cmd:
                    # Reception of CAL cmd
                    s = self.__cmdCALManager(room)
                elif Message.CMD_GET_UPDATE in cmd:
                    # Reception of UPDATE cmd
                    s = self.__cmdUPDATEManager(room, display, cmd)
                elif Message.CMD_REPORT:
                    #useless now
                    s=""
                else:
                    # Unknown command
                    s = Message.ANS_ERR

        return s

    def __cmdJOINManager(self, room:Room) -> str:
        return '{}|{}'.format(Message.ANS_ACCEPT, hex(self.configuration.rf_panId)[2:])

    def __cmdSETUPManager(self, room:Room) -> str:
        return '{}|{}|{}|{}|{}|{}|{}|{}'.format(
            Message.ANS_OK,
            Datetool.getCurrentTimeFromMidnight(),
            Datetool.getDayNumber(Datetool.getCurrentDay()),
            room.name,
            room.type,
            self.configuration.refreshStartTime,
            self.configuration.refreshEndTime,
            self.configuration.refreshTime
        )

    def __cmdCALManager(self, room:Room) -> str:
        s=Message.ANS_OK+'|'+Datetool.getDaysofWeek()+'|'

        for c in room.mergedCalendars:
            s= s+ '{};{};{};{};'.format(
                c.day,
                c.firstHour,
                c.lastHour,
                (c.title[:32] + '...') if len(c.title) > 35 else c.title
            )

            for t in c.trainees:
                s=s+ '{},'.format(
                    (t[:19] + '.') if len(t) > 20 else t
                )

            s=s[:-1] # remove last ','
            s=s+'|'

            for i in c.instructors:
                s=s+ '{},'.format(
                    (i[:19] + '.') if len(i) > 20 else i
                )

            s=s[:-1] # remove last ','
            s=s+'#'

        s=s[:-1] # remove last '#'
        s=s+"|EOF"

        # Vu que le calendrier va etre envoyé, on remet le flag calendarUpdate à False 
        room.calendarUpdate=False
        
        return s

    def __cmdUPDATEManager(self, room:Room, display: Display, cmd:str) -> str:
        command = cmd.split('|')
        try:
            display.batterylevel = int(command[1])
        except:
            if len(command)>1:
                self.log.warning("Invalid battery level value in UPDATE command : {}".format(str(command[1])))
            else:
                self.log.warning("No battery field in UPDATE command")

            display.batterylevel = -2

        try:
            display.rfPower = int(command[2])
        except:
            if len(command)>2:
                self.log.warning("Invalid RF power value in UPDATE command : {}".format(str(command[2])))
            else:
                self.log.warning("No RF power field in UPDATE command")
            
            display.rfPower = -2

        s='{}|{}'.format(
            Message.ANS_OK,
            1 if room.calendarUpdate else 0
        )

        return s