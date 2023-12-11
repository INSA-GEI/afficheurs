# messages.py
#
#

from common import Datetool, Room, Calendar
import logging,os

from config import Config

logmessagemgr = logging.getLogger("messages_mgr")
logmessages = logging.getLogger("messages")
logging.basicConfig(level=os.environ.get("LOGLEVEL", "INFO"))

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
    
    type: str=CMD_UNKNOWN
    raw_msg: str=""
    data = []
    device_id :int =0
  
    configuration:Config= None
    log:logging=None

    @staticmethod
    def manage(cmd:str, room:Room) -> str:
        s=""

        if room is None:
            # Display is not in room list => to be added in configuration file
            print("Room not found")
            s= Message.ANS_REJECT
        else:
            # This case should never happen: type for room is not Room
            if not isinstance(room, Room):
                print("Invalid type for room {}".format (type(room)))
                raise Exception("Invalid type for room {}".format (type(room)))
            # otherwise, everything is good, let's answer to this command
            else:
                print("Message.manage: receive cmd [{}] from room {}".format (cmd, room.name))
                if Message.CMD_JOIN in cmd: 
                    # Reception of JOIN cmd, useless now
                    # let's answer OK !
                    s = Message.cmdJOINManager(room)
                elif Message.CMD_SETUP in cmd:
                    # Reception of SETUP cmd
                    s = Message.cmdSETUPManager(room)
                elif Message.CMD_GET_CALENDAR in cmd:
                    # Reception of CAL cmd
                    s = Message.cmdCALManager(room)
                elif Message.CMD_GET_UPDATE in cmd:
                    # Reception of UPDATE cmd
                    s = Message.cmdUPDATEManager(room)
                elif Message.CMD_REPORT in cmd:
                    # Reception of REPORT cmd
                    s = Message.cmdREPORTManager(room, cmd)
                else:
                    # Unknown command
                    s = Message.ANS_ERR

        return s

    @staticmethod
    def cmdJOINManager(room:Room) -> str:
        return '{}|{}'.format(Message.ANS_ACCEPT, hex(Message.configuration.rf_panId)[2:])

    @staticmethod
    def cmdSETUPManager(room:Room) -> str:
        return '{}|{}|{}|{}|{}|{}|{}|{}'.format(
            Message.ANS_OK,
            Datetool.getCurrentTimeFromMidnight(),
            Datetool.getDayNumber(Datetool.getCurrentDay()),
            room.name,
            room.type,
            Message.configuration.refreshStartTime,
            Message.configuration.refreshEndTime,
            Message.configuration.refreshTime
        )

    @staticmethod
    def cmdCALManager(room:Room) -> str:
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

    @staticmethod
    def cmdUPDATEManager(room:Room) -> str:
        s='{}|{}'.format(
            Message.ANS_OK,
            1 if room.calendarUpdate else 0
        )

        return s

    @staticmethod
    def cmdREPORTManager(room:Room, cmd:str) -> str:
        s=cmd.split('|')
        
        return  
