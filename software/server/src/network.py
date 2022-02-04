
import socket
import threading

import time
from common import Log

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
    waitForAnswer=False
    receiveEvent = None
    receivedAnswer = ""
    
    def __init__(self, id: int, name :str, address: str, receiveCallback=None, eventCallback=None) -> None:
        self.address = address
        self.name = name
        self.id = id
        self.receiveCallback = receiveCallback
        self.eventCallback = eventCallback
        self.reconnectionDelay =0
        self.waitForAnswer=False
        self.receiveEvent = threading.Event()
        
    def __str__(self) -> str:
        return "gateway " + self.name + " (" + self.address + ")"
    
    def ReceptionThread(self):
        self.isActive = False
        
        while True:
            time.sleep(self.reconnectDelay)
            
            try:
                self.__connect()
                self.reconnectDelay=0
                Log.writeLogFile("Connected to gateway " + self.name)
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
                    
                Log.writeLogFile("Gateway " + self.name + " unreachable, try again in " + str(self.reconnectDelay) + "s")
             
    def startReceptionThread(self) -> None:
        self.threadId = threading.Thread(target=self.ReceptionThread, args=())
        self.threadId.daemon=True
        self.threadId.start()
                 
    def __connect(self) -> None:
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # now connect to the gateway server on port 10234 
        
        try:
            self.sock.connect((self.address, 10234))
        except ConnectionRefusedError:
            self.isActive = False
            
            raise GatewayConnexionError("GatewayConnexionError", "unable to connect to " + self.name + " gateway")
        
        self.isActive = True
        self.waitForAnswer=False
        
    def __write(self, msg: str) -> None:
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

    def SendCalendars(self, calendars) -> None:
        for cal in calendars:
            roomName = cal[0]
            displaysId = cal[1]
            calendar = cal[2]
            
            # serialisation a faire
    
    def SendHello(self) -> None:
        self.__write("Hello\n")
        
        # Wait for an answer
        self.waitForAnswer=True
        self.receiveEvent.wait(5.0)
        if not self.receiveEvent.is_set():
            raise TimeoutError
        
        self.receiveEvent.clear()
        
        if self.receivedAnswer != "OK\n":
            raise RuntimeError
        
    def __read(self) -> None:
        chunks = []
        bytes_recd = 0
        last_char=0
        
        while True:
            while last_char != 0x0A:
                chunk = self.sock.recv(2048)
                if chunk == b'':
                    self.isActive=False
                    raise GatewayReceptionError("GatewayReceptionError","socket connection broken")
                chunks.append(chunk)
                bytes_recd = bytes_recd + len(chunk)
                last_char=chunk[-1]
            
            self.__receiveHandler(b''.join(chunks).decode("utf-8"))
            chunks = []
            bytes_recd =0
            last_char=0
    
    def __receiveHandler(self, data: str) -> None:
        # traitement a faire lors de la reception de donnée
        if self.waitForAnswer:
            self.receivedAnswer = data
            self.waitForAnswer=False
            self.receiveEvent.set()
        else:
            # si pas de donnée prevue en reception, alors envoie au callback
            if self.receiveCallback != None:
                self.receiveCallback(self.id, data)
            else:
                Log.writeLogFile("No callback defined for reception of gateway " + self.name + ": drop data")