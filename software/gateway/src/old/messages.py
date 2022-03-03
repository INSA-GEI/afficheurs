# un truc ici

import xbee

class Message():
    ANS_OK = "OK"
    ANS_ERR = "ERR"
    ANS_REJECT = "REJECT"
    ANS_ACCEPT = "ACCEPT"
    
    CMD_UNKNOWN = "UNKOWN"
    CMD_JOIN = "JOIN"
    CMD_CONFIG="CONFIG"
    CMD_SETUP="SETUP"
    CMD_GET_CALENDAR="CAL"
    CMD_GET_UPDATE="UPDATE"
    
    type: str=CMD_UNKNOWN
    raw_msg: str=""
    split_msg = []
    device_id :int =0
    rssi: int =0xFF
    
    def __init__ (self, raw_message:str):
        #self.raw_msg = raw_message.rstrip('\n')
        self.decode(raw_message)
        
    def __str__ (self):
        s="Msg cmd: " + self.type
        s+="\nDevice Id: "+hex(self.device_id)
        s+="\nRssi: "+hex(self.rssi)
            
        return s
  
    @staticmethod
    def get_message_type(raw_msg:str) -> str:
        return raw_msg.rstrip('\n').split('|')[0]
    
    def decode(self, raw_message:str):
        self.raw_msg =  raw_message.rstrip('\n')
        self.split_msg = self.raw_msg.split('|')
        self.type = self.split_msg[0]
            
        try:
            self.device_id = int(self.split_msg[1], base=16)
        except Exception as e:
            self.device_id = 0
        
    def encode(self) -> str:
        return self.type+"|"+hex(self.device_id)+"|"+hex(self.rssi)+"\n"
    
    def decode_from_xbee(self, frame: xbee.API_Frame):
        if (frame.__class__ == xbee._64Bit_Receive_Packet) or \
            (frame.__class__ == xbee.Receive_Packet_Frame) or \
            (frame.__class__ == xbee.Explicit_Receive_Indicator):
            
            self.device_id = frame.sender
            
            try:
                self.rssi = frame.rssi
            except:
                # Pas d'info sur le rssi
                self.rssi = 0xFF
            
            self.split_msg = frame.data.split('|')
            self.type = self.split_msg[0]
        else:
            # invalid frame
            raise RuntimeError ("Invalid frame type")
        
    def encode_to_xbee(self, frame_id: int, options: int=0) -> xbee.Transmit_Request:
        s = self.type+":"
        return xbee.Transmit_Request(frame_id, self.device_id, options, s)
        
class ANS_OK_Message(Message):
    
    data =[]
    
    def __str__ (self):
        s=Message.__str__(self) + "\nData: [ " 
        
        for d in self.data:
            s+=d+" , "
        
        return s+" ]"
    
    def decode(self, raw_message:str):
        Message.decode(self, raw_message)
        
        try:
            for d in self.split_msg[1:]:
                self.data.append(d)
        except:
            pass
    
    def encode_to_xbee(self, frame_id: int, options: int=0) -> xbee.Transmit_Request:
        f = Message.encode_to_xbee(self, frame_id, options)
        f.data = self.type+"|"
        
        for d in self.data:
            f.data+=d+"|"
            
        return f

class ANS_ERR_Message(Message):
    err_val: int=0
    
    def __str__ (self):
        return Message.__str__(self) + "\nError val: " + str(self.err_val)
    
    def decode(self, raw_message:str):
        Message.decode(self, raw_message)
        
        try:
            self.err_val = int(self.split_msg[2])
        except:
            self.err_val = -1
    
    def encode_to_xbee(self, frame_id: int, options: int=0) -> xbee.Transmit_Request:
        f = Message.encode_to_xbee(self, frame_id, options)
        f.data = self.type+"|"+str(self.err_val)
        return f
           
class ANS_Reject_Message(Message):
    def encode_to_xbee(self, frame_id: int, options: int=0) -> xbee.Transmit_Request:
        f = Message.encode_to_xbee(self, frame_id, options)
        f.data = self.type
        return f

class ANS_Accept_Message(Message):
    pan_id: int=0
    
    def __str__ (self):
        return Message.__str__(self) + "\nPan Id: " + hex(self.pan_id)
    
    def decode(self, raw_message:str):
        Message.decode(self, raw_message)
        
        try:
            self.pan_id = int(self.split_msg[2], base=16)
        except:
            self.pan_id = 0
    
    def encode_to_xbee(self, frame_id: int, options: int=0) -> xbee.Transmit_Request:
        f = Message.encode_to_xbee(self, frame_id, options)
        f.data = self.type+"|"+hex(self.pan_id)
        return f
    
class CMD_Join_Message(Message):
    def __init__ (self):
        self.type = Message.CMD_JOIN

class CMD_Setup(Message):
    def __init__ (self):
        self.type = Message.CMD_SETUP

class CMD_Config(Message):
    def __init__ (self):
        self.raw_msg = "CONFIG"
        self.decode(self.raw_msg)

class CMD_Get_Calendar(Message):
    def __init__ (self):
        self.type = Message.CMD_GET_CALENDAR

class CMD_Get_Update(Message):
    def __init__ (self):
        self.type = Message.CMD_GET_UPDATE
        
def decodeMessage(raw_msg) -> Message:
    msg_type=Message.get_message_type(raw_msg)
    
    switcher ={
        Message.ANS_OK: ANS_OK_Message,
        Message.ANS_ERR: ANS_ERR_Message,
        Message.ANS_REJECT: ANS_Reject_Message,
        Message.ANS_ACCEPT: ANS_Accept_Message,
        Message.CMD_JOIN: CMD_Join_Message,
        Message.CMD_CONFIG: CMD_Config,
        Message.CMD_SETUP: CMD_Setup,
        Message.CMD_GET_CALENDAR: CMD_Get_Calendar,
        Message.CMD_GET_UPDATE: CMD_Get_Update,
    }
    
    return switcher.get(msg_type, Message)(raw_msg)


   
        