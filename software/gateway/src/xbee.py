# un truc a mettre ici

import serial
from threading import Thread, Lock
import time

class API_Frame():
    RECEIVE_PACKET = 0x90
    EXPLICIT_RECEIVE_INDICATOR = 0x91
    TRANSMIT_STATUS = 0x89
    EXTENDED_TRANSMIT_STATUS = 0x8B
    LOCAL_AT_COMMAND_RESPONSE = 0x88
    LOCAL_AT_COMMAND_REQUEST = 0x08
    _64BIT_TRANSMIT_REQUEST = 0x00
    TRANSMIT_REQUEST = 0x10
    EXPLICIT_ADDRESSING_COMMAND_REQUEST= 0x11
    _64BIT_RECEIVE_PACKET = 0x80
    UNKNOWN_FRAME =0xFF
    
    type: int=UNKNOWN_FRAME
    raw_frame: bytes=[]
    
    def __init__(self):
        self.type = self.UNKNOWN_FRAME
        
    def __init__(self, raw_frame):
        self.raw_frame = raw_frame
        self.decode_frame(raw_frame)
    
    @staticmethod
    def get_raw_frame_type(raw_frame):
        return raw_frame[3]
    
    def decode_frame(self):
        self.decode_frame(self.raw_frame)
        
    def decode_frame(self, raw_frame):
        if self.verify_checksum() != True:
            raise ValueError("Frame is invalid")
        
        self.type = raw_frame[3]
    
    def encode_frame(self):
        pass
    
    def verify_checksum(self):
        checksum = 0

        # verify frame is valid
        for b in self.raw_frame[3:]:
            checksum += b

        checksum=checksum%256

        if checksum != 0xFF:
            return False
        else:
            return True
    
    def compute_checksum (self, raw_frame):
        checksum =0
        length = raw_frame[1]*256 + raw_frame[2]
        
        # compute checksum
        for b in raw_frame[3:3+length]:
            checksum += b
        
        checksum = 0xFF-(checksum%256)
        return checksum
    
    def __str__(self) -> str:
        switcher = {
            self.TRANSMIT_REQUEST: "Transmit Request",
            self.EXPLICIT_ADDRESSING_COMMAND_REQUEST: "Explicit Addressing Command Request",
            self.TRANSMIT_STATUS: "Transmit Status",
            self.EXTENDED_TRANSMIT_STATUS: "Extended Transmit Status",
            self.RECEIVE_PACKET: "Receive Packet",
            self.EXPLICIT_RECEIVE_INDICATOR: "Explicit Receive Indicator",
            self.LOCAL_AT_COMMAND_REQUEST: "Local AT Command Request",
            self.LOCAL_AT_COMMAND_RESPONSE: "Local AT Command Response",
            self._64BIT_TRANSMIT_REQUEST: "64 Bit Transmit Request",
            self._64BIT_RECEIVE_PACKET: "64 Bit Receive Packet",
            self.UNKNOWN_FRAME: "Unknown Frame",
            }
        
        return "Frame type: " + switcher.get(self.type)

class _64Bit_Receive_Packet(API_Frame):
    data: str = ""
    sender: int = 0
    status: int = 0
    rssi:int =0
    
    def __str__(self) -> str:
        s = API_Frame.__str__(self)
        s+="\nStatus: "+hex(self.status)
        s+="\nSender: "+hex(self.sender)
        s+="\nRSSI: "+hex(self.rssi)
        s+="\nData: "
        
        for d in self.data:
            s+=d
        
        return s
    
    def decode_frame(self, raw_frame):
        # Appel la methode du parent (ici pour verifier le checksum et decoder le type)
        API_Frame.decode_frame(self,raw_frame)

        if self.type != self._64BIT_RECEIVE_PACKET:
            raise ValueError("Wrong frame type")
        else:
            self.sender = int.from_bytes(raw_frame[4:12], "big")
            self.rssi = raw_frame[12]
            self.status = raw_frame[13]
            data_part = raw_frame[14:len(raw_frame)-1]
            self.data = data_part.decode("ascii")
            
class Receive_Packet_Frame(API_Frame):
    data: str = ""
    sender: int = 0
    status: int = 0
    
    def __str__(self) -> str:
        s = API_Frame.__str__(self)
        s+="\nStatus: "+hex(self.status)
        s+="\nSender: "+hex(self.sender)
        s+="\nData: "
        
        for d in self.data:
            s+=d
        
        return s
    
    def decode_frame(self, raw_frame):
        # Appel la methode du parent (ici pour verifier le checksum et decoder le type)
        API_Frame.decode_frame(self,raw_frame)

        if self.type != self.RECEIVE_PACKET:
            raise ValueError("Wrong frame type")
        else:
            self.sender = int.from_bytes(raw_frame[4:12], "big")
            self.status = raw_frame[14]
            data_part = raw_frame[15:len(raw_frame)-1]
            self.data = data_part.decode("utf-8") 
            
class Explicit_Receive_Indicator(API_Frame):
    data: str = ""
    sender: int = 0
    status: int = 0
      
    def __str__(self) -> str:
        s = API_Frame.__str__(self)
        s+="\nStatus: "+hex(self.status)
        s+="\nSender: "+hex(self.sender)
        s+="\nData: "
        
        for d in self.data:
            s+=d
        
        return s
    
    def decode_frame(self, raw_frame):
        # Appel la methode du parent (ici pour verifier le checksum et decoder le type)
        API_Frame.decode_frame(self,raw_frame)

        if self.type != self.EXPLICIT_RECEIVE_INDICATOR:
            raise ValueError("Wrong frame type")
        else:
            self.sender = int.from_bytes(raw_frame[4:12], "big")
            self.status = raw_frame[20]
            data_part = raw_frame[21:len(raw_frame)-1]
            self.data = data_part.decode("utf-8")   

class Transmit_Status(API_Frame):
    id: int=0
    status: int =0
    
    def __str__(self) -> str:
        s = API_Frame.__str__(self)
        s+="\nFrame Id: "+hex(self.id)
        s+="\nStatus: "+hex(self.status)
        
        return s
    
    def decode_frame(self, raw_frame):
        # Appel la methode du parent (ici pour verifier le checksum et decoder le type)
        API_Frame.decode_frame(self,raw_frame)

        if self.type != self.TRANSMIT_STATUS:
            raise ValueError("Wrong frame type")
        else:
            self.id = raw_frame[4]
            self.status = raw_frame[5]
            
class Extended_Transmit_Status(API_Frame):
    id: int=0
    status: int =0
    tx_retry:int=0
    discovery_status:int=0
    
    def __str__(self) -> str:
        s = API_Frame.__str__(self)
        s+="\nFrame Id: "+hex(self.id)
        s+="\nStatus: "+hex(self.status)
        s+="\nTx retries: "+hex(self.tx_retry)
        s+="\nDiscovery: "+hex(self.discovery_status)
        
        return s
    
    def decode_frame(self, raw_frame):
        # Appel la methode du parent (ici pour verifier le checksum et decoder le type)
        API_Frame.decode_frame(self,raw_frame)

        if self.type != self.EXTENDED_TRANSMIT_STATUS:
            raise ValueError("Wrong frame type")
        else:
            self.id = raw_frame[4]
            self.tx_retry = raw_frame[7]
            self.status = raw_frame[8]
            self.discovery_status = raw_frame[9]
            
class Transmit_Request(API_Frame):
    id: int=0
    options: int =0
    data: str=""
    dest: int =0
    
    def __init__(self, id, dest, options, data):
        self.type = API_Frame.TRANSMIT_REQUEST
        
        self.id=id
        self.options=options
        self.dest = dest
        self.data = data
            
    def __str__(self) -> str:
        s = API_Frame.__str__(self)
        s+="\nFrame Id: "+hex(self.id)
        s+="\nDest address: "+hex(self.dest)
        s+="\nOptions: "+hex(self.options)
        s+="\nData: "
        
        for d in self.data:
            s+=d
        
        return s
    
    def encode_frame(self):
        #raw_frame = bytes(len(self.data)+18)
        raw_frame = bytearray()
        
        if self.type != self.TRANSMIT_REQUEST:
            raise ValueError("Wrong frame type")
        
        raw_frame+=bytearray(b'~')
        raw_frame+=bytearray(int.to_bytes(len(self.data)+18-4,length=2, byteorder='big'))
        raw_frame+=bytearray(int.to_bytes(self.type,length=1, byteorder='big'))
        raw_frame+=bytearray(int.to_bytes(self.id,length=1, byteorder='big'))
        raw_frame+=bytearray(int.to_bytes(self.dest,length=8,byteorder='big'))
        raw_frame+=bytearray(int.to_bytes(0xFFFE,length=2,byteorder='big'))
        raw_frame+=bytearray(int.to_bytes(0,length=1, byteorder='big'))
        raw_frame+=bytearray(int.to_bytes(self.options,length=1, byteorder='big'))
        raw_frame+=bytearray(str.encode(self.data, encoding="latin_1"))
        raw_frame+=bytearray(int.to_bytes(API_Frame.compute_checksum(self, bytes(raw_frame)),
                                          length=1, 
                                          byteorder='big'))
        
        self.raw_frame=bytes(raw_frame)
        return self.raw_frame
        
class XBEE():
    waitforAnswer=False
    lock:Lock=None
    rxThreadId:Thread=None

    uart:serial.Serial=None
    receiveCallback=None

    def __init__(self, receiveCallback, com:str, baudrate:int=9600) -> None:
        if receiveCallback!=None:
            self.receiveCallback=receiveCallback
        else:
            raise RuntimeError("Receive callback not defined")

        if com == "":
            raise RuntimeError("Com port cannot be empty")

        #self.uart = serial.Serial (com, baudrate, timeout=0.5)    # Open port with baud rate
        self.uart = serial.Serial (com, baudrate)    # Open port with baud rate
                
        self.waitforAnswer=False
        self.lock=Lock()

        self.rxThreadId = Thread(target=self.__receiveThread, args=(), name="Xbee RX thread")
        self.rxThreadId.daemon=True
        self.rxThreadId.start()

    def __receiveThread(self):
        while True:
            time.sleep(0.03)

            try:
                received_frame = self.uart.read(size=1) # Receive first byte (should be ~)
                if len(received_frame) == 1:
                    if received_frame == b'~':
                        
                        received_frame += self.uart.read(size=2) # get next two bytes: size of following frame

                        if len(received_frame) == 3:
                            remaining_frame_length = received_frame[1]*256 + received_frame[2]

                            received_frame += self.uart.read(size=remaining_frame_length+1)

                            if len(received_frame) == remaining_frame_length +3+1: #size of frame + 3 bytes of header + 1 for checksum
                                try:
                                    decode_frame=self.__decodeFrame(received_frame)
                                except ValueError: # frame is invalid
                                    print ("Invalid RX frame: " + str(received_frame))
                                    decode_frame=None

                                if decode_frame != None:
                                    #print ("decoded frame:\n"+str(decode_frame))
                                    self.receiveCallback(self, decode_frame) # send frame to caller

                                # else, frame is invalid, drop it
                        # else timeout received, drop frame and start again
                    # else, invalid frame start delimiter, skip it
                # timeout while reading, try again
            except Exception as e:
                print ("Uncaught exception in RX thread:\n" + str(e))
    
    def sendFrame(self, frame: API_Frame) -> None:
        raw_frame = frame.encode_frame()
        
        np = 60 # A reprendre en faisant une lecture du parametre NP du XBEE, qui limite la taille d'une frame Xbee
        remainingBytes = len(raw_frame)
        
        while remainingBytes:
            if len(raw_frame)>=np:
                local_buffer= raw_frame[0:np-1]
            else:   
                local_buffer= raw_frame
                
            self.uart.write(local_buffer)
            
            remainingBytes = remainingBytes-np
            if remainingBytes < 0:
                remainingBytes =0
            else:
                raw_frame = raw_frame[np:]
                
        self.uart.write(Transmit_Request(1, frame.dest, frame.options, "END").encode_frame())
        
    def __decodeFrame(self, frame) -> API_Frame:
        frame_type=API_Frame.get_raw_frame_type(frame)
        
        switcher ={
            API_Frame.TRANSMIT_REQUEST: Transmit_Request,
            API_Frame.EXPLICIT_ADDRESSING_COMMAND_REQUEST: Explicit_Receive_Indicator,
            API_Frame.TRANSMIT_STATUS: Transmit_Status,
            API_Frame.EXTENDED_TRANSMIT_STATUS: Extended_Transmit_Status,
            API_Frame.RECEIVE_PACKET: Receive_Packet_Frame,
            API_Frame.EXPLICIT_RECEIVE_INDICATOR: API_Frame,
            API_Frame.LOCAL_AT_COMMAND_REQUEST: API_Frame,
            API_Frame.LOCAL_AT_COMMAND_RESPONSE: API_Frame,
            API_Frame._64BIT_TRANSMIT_REQUEST: API_Frame,
            API_Frame._64BIT_RECEIVE_PACKET: _64Bit_Receive_Packet,
        }
        
        return switcher.get(frame_type, API_Frame)(frame)