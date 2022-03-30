# messages.py
#
#

import socket
import socketserver
from threading import Thread
from queue import Queue

from _thread import *

import logging,os

logmessagemgr = logging.getLogger("messages_mgr")
logmessages = logging.getLogger("messages")
logging.basicConfig(level=os.environ.get("LOGLEVEL", "INFO"))

class Message():
    ANS_OK = "OK"
    ANS_ERR = "ERR"
    ANS_END = "END"
    ANS_REJECT = "REJECT"
    ANS_ACCEPT = "ACCEPT"
    ANS_DROP = "DROP"
    
    CMD_UNKNOWN = "UNKOWN"
    CMD_JOIN = "JOIN"
    CMD_CONFIG="CONFIG"
    CMD_SETUP="SETUP"
    CMD_GET_CALENDAR="CAL"
    CMD_GET_UPDATE="UPDATE"
    CMD_REPORT="REPORT"
    
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
        return raw_msg.rstrip('\n').split('|')[0].upper()
    
    def decode_from_client(self, raw_message:str):
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
                    print ("No device Id in message \"{}\"".format(self.raw_msg))
                else:
                    print ("Invalid device ID \"{}\" in message \"{}\"".format(self.data[-1],self.raw_msg ))
        
        if self.type == self.CMD_JOIN and len(self.data) ==3: # JOIN cmd is 3 elements long
            #keep only rssi value
            self.data = self.data[1:]
            self.data = self.data[:-1]
        elif self.type == self.CMD_CONFIG and len(self.data) ==2: # CONFIG cmd is 2 elements long
            self.data = []
        elif self.type == self.CMD_SETUP and len(self.data) ==2: # SETUP cmd is exactly 2 elements long
            self.data = []
        elif self.type == self.CMD_GET_CALENDAR and len(self.data) ==2: # GET_CALENDAR cmd is exactly 2 elements long
            self.data = []
        elif self.type == self.CMD_GET_UPDATE and len(self.data) ==2: # GET_UPDATE cmd is exactly 2 elements long
            self.data = []
        elif self.type == self.CMD_REPORT and len(self.data) ==11: # CMD_REPORT cmd is exactly 11 elements long
            self.data = self.data[1:]
            self.data = self.data[:-1]
        else:
            self.data = []
            if self.type == self.CMD_JOIN \
               or self.type == self.CMD_CONFIG \
               or self.type == self.CMD_SETUP \
               or self.type == self.CMD_GET_CALENDAR \
               or self.type == self.CMD_REPORT:
                raise RuntimeError("Invalid frame length in message \"{}\"".format(self.raw_msg))
            else:
                raise RuntimeError("Invalid frame type ("+ self.type +") in message \"{}\"".format(self.raw_msg))
            
    def encode_to_client(self) -> str:
        s = self.type + '|'
        
        for d in self.data:
            s+=d+'|'
            
        return s+hex(self.device_id)[2:].upper()+"\n"

class MessageMgr():
    ServerSocket = None
    actionHandler=None
    
    @staticmethod
    def gethostname():
        return socket.gethostname()
    
    def __init__(self, host, port, maxclients):
        self.ServerSocket = socket.socket()
        
        try:
            self.ServerSocket.bind((host, port))
        except socket.error as e:
            #logmessagemgr.error(str(e))
            raise RuntimeError("Socket port %s already in use"%(str(port)))
            
        logmessagemgr.info("Server started on %s:%s"%(host, str(port)))
        logmessagemgr.info('Waiting for connections ...')
        self.ServerSocket.listen(maxclients)
        
    def __threaded_client(self, connection:socket.socket, gateway):
        print ("Client thread started")
        connection.sendall(str.encode('Welcome to server\n'))
        raw_message=""
        
        while True:
            data = connection.recv(2048)
            if not data:
                #Connection closed by client
                break
            
            raw_message += data.decode('utf-8')
            if raw_message[-1]== '\n':    
                #print ("Received data: " +data.decode('utf-8'))
                #reply = 'Ok\n'
                #print ("raw_message = %s"%raw_message)
                msg = Message()
                msg_ans=None
                try:
                    msg.decode_from_client(raw_message)
                    #print ("Received message: " + str(msg))
                except RuntimeError as e:
                    logmessagemgr.warning("RuntimeError in __threaded_client while decoding message (" +(str(e))+')')
                    msg_ans=Message()
                    msg_ans.type = Message.ANS_ERR
                    msg_ans.data = [str(3)] # error 3 (invalid frame)
                    msg_ans.device_id=msg.device_id
                except Exception as e:
                    logmessagemgr.warning("Unknown error in __threaded_client while decoding message (" +(str(e))+')')
                    msg_ans=Message()
                    msg_ans.type = Message.ANS_ERR
                    msg_ans.data = [str(4)] # error 4 (unknwo error in decoding frame)
                    msg_ans.device_id=msg.device_id
                
                if msg_ans == None:
                    if self.actionHandler != None:
                        msg_ans = self.actionHandler(msg,gateway)
                    else:
                        msg_ans=Message()
                        msg_ans.type = Message.ANS_ERR
                        msg_ans.data = [str(2)] # no action from server
                        msg_ans.device_id=msg.device_id
                
                connection.sendall(str.encode(msg_ans.encode_to_client()))   
                break
        
        #close tcp connection with client
        connection.close()
        print ("Client thread finished")
        
    def start(self):
        while True:
            Client, (name, port) = self.ServerSocket.accept()
            print ('Connection from ' + name + ':' + str(port) + " accepted")
            start_new_thread(self.__threaded_client, (Client, name ))
        
    def stop(self): 
        self.ServerSocket.close()