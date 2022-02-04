

import queue
import threading
from typing import Any
from network import Gateway
from common import Log

class StateMachineMessage:
    UNKNOWN_EVENT=0
    GATEWAY_CONNECTED=1
    GATEWAY_DISCONNECTED=2
    DEVICE_JOIN=3
    
    @staticmethod
    def toString(state) -> str:
        switcher = {
            StateMachineMessage.UNKNOWN_EVENT: "UNKNOWN_EVENT",
            StateMachineMessage.GATEWAY_CONNECTED: "GATEWAY_CONNECTED",
            StateMachineMessage.GATEWAY_DISCONNECTED: "GATEWAY_DISCONNECTED",
            StateMachineMessage.DEVICE_JOIN: "DEVICE_JOIN",
        }
    
        return switcher.get(state,"UNKNOWN_EVENT")
    
class StateMachine():
    q= None
    threadId=None
    
    def __init__(self) -> None:
        self.q=queue.Queue()
        self.threadId = threading.Thread(target=self.__threadStateMachine, 
                                         args=(), 
                                         name="Machine State")
        self.threadId.daemon = True
        self.threadId.start()
        
    def sendMessage(self, messageId :StateMachineMessage, parameters: Any) -> None:
        self.q.put((messageId,parameters))
    
    def __threadStateMachine(self) -> None:
        switcher = {
            StateMachineMessage.UNKNOWN_EVENT: self.__UnknownAction,
            StateMachineMessage.GATEWAY_CONNECTED: self.__GatewayConnectAction,
            StateMachineMessage.GATEWAY_DISCONNECTED: self.__GatewayDisconnectAction,
            StateMachineMessage.DEVICE_JOIN: self.__DisplayJoinAction,
            }
 
        while True:
            msg = self.q.get()
            
            # get() method of dictionary data type returns
            # value of passed argument if it is present
            # in dictionary otherwise second argument will
            # be assigned as default value of passed argument
            try:
                switcher.get(msg[0], self.__UnknownAction)(msg[1])
            except TimeoutError:
                Log.writeLogFile("Timeout error with event " + StateMachineMessage.toString(msg[0]))
            except RuntimeError:
                Log.writeLogFile("Invalid answer in event " + StateMachineMessage.toString(msg[0]))
            
    def __GatewayConnectAction(self, data) -> None :
        gw= data[0]
        rooms= data[1]
        print ("EVENT_CONNECTED received from gateway " + gw.name)
        
        # calData= []
        # for room in rooms:
        #     calData.append((room.name, room.displayId, room.calendars))
            
        # gw.SendCalendars(calData)    
        gw.SendHello()
    
    def __GatewayDisconnectAction(self, data) -> None:
        gw= data[0]
        print ("EVENT_DISCONNECTED received from gateway " + gw.name)
    
    def __DisplayJoinAction(self, data) -> None:
        deviceId = data[0]
        print ("DEVICE_JOIN received from device " + str(deviceId))
    
    def __UnknownAction(self,data) -> None:
        print ("UNKNOWN_EVENT received")