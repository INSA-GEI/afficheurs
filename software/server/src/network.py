
import socket

from config import Configuration
import time

class GatewayConnexionError(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = message
   
class GatewayReceptionError(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = message     

class GatewayTransmissionError(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = message

class GatewayEvents:
    EVENT_CONNECTED=1
    EVENT_CONNECTION_LOST=0
    UNKNOWN_EVENT=-1
    
class Gateway:
    id=0
    name = ""
    address = ""
    isActive=False
    reconnectDelay=0
    receiveCallback=None
    eventCallback=None
    reconnectionDelay =0
    
    def __init__(self, id: int, name :str, address: str, receiveCallback=None, eventCallback=None) -> None:
        self.address = address
        self.name = name
        self.id = id
        self.receiveCallback = receiveCallback
        self.eventCallback = eventCallback
        self.reconnectionDelay =0
        
    def __str__(self) -> str:
        return "gateway " + self.name + " (" + self.address + ")"
    
    def ReceptionThread(self):
        self.isActive = False
        
        while True:
            time.sleep(self.reconnectDelay)
            
            try:
                self.connect()
                self.reconnectDelay=0
                Configuration.writeLogFile("Connected to gateway " + self.name)
                self.isActive = True
                
                if self.eventCallback != None:
                    self.eventCallback(self.id, GatewayEvents.EVENT_CONNECTED)
                    
                self.__read()
            except (GatewayConnexionError, GatewayReceptionError, socket.gaierror) as e:
                
                if e.__class__ == GatewayReceptionError:
                    if self.eventCallback != None:
                        self.eventCallback(self.id, GatewayEvents.EVENT_CONNECTION_LOST)
                    
                self.isActive = False
                if self.reconnectDelay == 0:
                    self.reconnectDelay = 5
                else:
                    self.reconnectDelay = self.reconnectDelay*2
                    if self.reconnectDelay > 5*60: # 5 minutes
                        self.reconnectDelay = 5*60
                    
                Configuration.writeLogFile("Gateway " + self.name + " unreachable, try again in " + str(self.reconnectDelay) + "s")
                            
    def connect(self) -> None:
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # now connect to the gateway server on port 10234 
        
        try:
            self.sock.connect((self.address, 10234))
        except ConnectionRefusedError:
            self.isActive = False
            
            raise GatewayConnexionError("GatewayConnexionError", "unable to connect to " + self.name + " gateway")
        
        self.isActive = True
        
    def write(self, msg: str) -> None:
        totalsent = 0
        if msg[-1] != '\n':
            msg = msg + '\n'
            
        MSGLEN = len(msg)
        msgbytes = bytes(msg,'UTF-8')
        
        while totalsent < MSGLEN:
            sent = self.sock.send(msgbytes[totalsent:])
            if sent == 0:
                self.isActive=False
                raise GatewayTransmissionError("GatewayTransmissionError","socket connection broken")
            totalsent = totalsent + sent

    def __read(self) -> None:
        chunks = []
        bytes_recd = 0
        
        while True:
            while chunks==[] or chunks[-1][-1] != 0x0A:
                chunk = self.sock.recv(2048)
                if chunk == b'':
                    self.isActive=False
                    raise GatewayReceptionError("GatewayReceptionError","socket connection broken")
                chunks.append(chunk)
                bytes_recd = bytes_recd + len(chunk)
            
            if self.receiveCallback != None:
                self.receiveCallback(self.id, b''.join(chunks).decode("utf-8"))
            else:
                Configuration.writeLogFile("No callback defined for reception of gateway " + self.name + ": drop data")
                
            chunks = []
            bytes_recd =0