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
    data = []
    device_id :int =0
        
    def __str__ (self):
        s="Msg cmd: " + self.type
        s+="\nDevice Id: "+hex(self.device_id)
        s+="\nData: [ " 
        try:
            s+= self.data[0]
        except: 
            pass
        
        for d in self.data[1:]:
            s+= " | " + d
         
        s+= " ]"   
        return s
  
    @staticmethod
    def get_message_type(raw_msg:str) -> str:
        return raw_msg.rstrip('\n').split('|')[0]
    
    def decode_from_srv(self, raw_message:str):
        self.raw_msg =  raw_message.rstrip('\n')
        self.data = self.raw_msg.split('|')
        self.type = self.data[0].upper()
       
        try:
            self.device_id = int(self.data[-1], base=16)
        except Exception as e:
            try: 
                self.device_id = int(self.data[-1], base=10)
            except Exception as e:
                self.device_id = 0
                
                if len(self.data)>=1:
                    print ("ERROR: No device Id in message \"{}\"".format(self.raw_msg))
                else:
                    print ("ERROR: Invalid device ID \"{}\" in message \"{}\"".format(self.data[-1],self.raw_msg ))
        
        if self.type == self.ANS_OK and len(self.data) >=2: # Ok answer is at least 2 elements long
            self.data = self.data[1:]
            self.data = self.data[:-1]
        elif self.type == self.ANS_ACCEPT and len(self.data) ==3: # ACCEPT answer is exactly 3 elements long
            self.data = self.data[1:]
            self.data = self.data[:-1]
        elif self.type == self.ANS_REJECT and len(self.data) ==2: # REJECT answer is exactly 3 elements long
            self.data = []
        elif self.type == self.ANS_ERR and len(self.data) ==3: # ERR answer is exactly 3 elements long
            self.data = self.data[1:]
            self.data = self.data[:-1]
        else:
            self.data = []
            raise RuntimeError("ERROR: Invalid frame length in message \"{}\"".format(self.raw_msg))
            
    def encode_to_srv(self) -> str:
        s = self.type + '|'
        
        if self.type == "JOIN":
            s+=hex(self.rssi) + '|'
            
        for d in self.data:
            s+=d+'|'
            
        return s+hex(self.device_id)+"\n"
    
    def decode_from_xbee(self, frame: xbee.API_Frame):
        if (frame.__class__ == xbee._64Bit_Receive_Packet) or \
            (frame.__class__ == xbee.Receive_Packet_Frame) or \
            (frame.__class__ == xbee.Explicit_Receive_Indicator):
            
            self.device_id = frame.sender
            
            self.data = frame.data.split('|')
            self.type = self.data[0]
            
            if self.type == "JOIN":
                self.rssi = frame.rssi
                
            if len(self.data) >=1:
                self.data = self.data[1:]
            else:   
                self.data = []
        else:
            # invalid frame
            raise RuntimeError ("Invalid frame type")
        
    def encode_to_xbee(self, frame_id: int, options: int=0) -> xbee.Transmit_Request:
        s = self.type+"|"
        
        for d in self.data:
            s+=d+'|'
            
        s=s[:-1]
        
        return xbee.Transmit_Request(frame_id, self.device_id, options, s)
        
        