#!/usr/bin/env python3

try:
    import RPi.GPIO as GPIO
except:
    pass # not on raspberry, don't use GPIO

import time
from threading import Thread
from queue import Queue

import xbee
from networkmgr import NetworkMgr
from messages import Message
import logging, os
import statistics
import tests

import argparse
import configparser

serialDevice = "/dev/serial0"
serverName = ""
serverPort = -1
panID = -1
chanID = -1
gatewayName= ""
netman:NetworkMgr=None

rssi_device = {}
DEFAULT_CONFIG_FILE = "/home/dimercur/Travail/git/afficheurs/software/gateway/src/default.conf"

log = logging.getLogger("gateway")
logging.basicConfig(level=os.environ.get("LOGLEVEL", "INFO"))

try:
    GPIO_LED_ERROR = (29, GPIO.OUT)
    GPIO_LED_ACTIVITY = (31, GPIO.OUT)
    GPIO_LED_PWR = (33, GPIO.OUT)

    GPIO_XBEE_SLEEP_RQ = (11,GPIO.OUT)
    GPIO_XBEE_SLEEP = (13, GPIO.IN)
    GPIO_XBEE_RESET = (15, GPIO.OUT)
    GPIO_XBEE_ASSOCIATE = (16, GPIO.IN)
    GPIO_XBEE_RSSI_PWM = (12, GPIO.IN)
except:
    pass

def GPIO_set(gpio):
    try:
        GPIO.output(gpio[0], GPIO.HIGH) 
    except:
        pass # pass silently as we are not running RPI
    
def GPIO_clear(gpio):
    try:
        GPIO.output(gpio[0], GPIO.LOW) 
    except:
        pass # pass silently as we are not running RPI

def GPIO_get(gpio):
    try:
        if GPIO.input(gpio[0]) == GPIO.HIGH:
            return True 
        else:
            return False
    except:
        pass # pass silently as we are not running RPI

def threadServer():
    while True:
        time.sleep(10.0)

def RxCallback(xb:xbee.XBEE, frame:xbee.API_Frame)->None:
    global rssi_device
    
    try:
        GPIO_set(GPIO_LED_ACTIVITY)
        time.sleep(0.5)
        GPIO_clear(GPIO_LED_ACTIVITY)
    except:
        pass # not executing code on RPI
    
    if frame.__class__ == xbee._64Bit_Receive_Packet or \
       frame.__class__ == xbee.Receive_Packet_Frame or \
       frame.__class__ == xbee.Explicit_Receive_Indicator:
        msg= Message()
        msg.decode_from_xbee(frame)
        end = False
        
        if frame.sender in rssi_device:
            rssi = rssi_device[frame.sender]
            try:
                rssi.append(frame.rssi)
            except:
                rssi.append(0)
            rssi_device[frame.sender]=rssi
        else:
            rssi_device[frame.sender]=[frame.rssi]
            
        if msg.type == "REPORT":
            ack=False
            #Rajoute les infos de la gateway concernant cet ecran
            msg.data.append(str(hex(max(rssi_device[frame.sender]))[2:].upper()))
            msg.data.append(str(hex(min(rssi_device[frame.sender]))[2:].upper()))
            msg.data.append(str(hex(statistics.mean(rssi_device[frame.sender]))[2:].upper()))
        else:
            ack=True
            
        ans = netman.sendMessage(msg,ack)
        
        if ack:
            if msg.type == "JOIN":
                ans_frame = ans.encode_to_xbee(1, 0x02)
            else:
                ans_frame = ans.encode_to_xbee(1, 0x00)
          
            if msg.type == "CAL" or \
                msg.type == "SETUP":
                end = True
          
            xb.sendFrame(ans_frame, end)
            
    elif frame.__class__ == xbee.Transmit_Status or\
         frame.__class__ == xbee.Extended_Transmit_Status:
        #stuff to do here
        id = frame.id
        status = frame.status
    elif frame.__class__ == xbee.Modem_Status:
        log.info ("Modem Status received: " + hex(frame.status))
    else:
        #unsupported frame
        log.warning("Unsupported frame received: " + str(frame))

def parseCommandLine() -> str:
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', metavar='configfile', help='Configuration file')
    
    args = parser.parse_args()
    if args.c == None:
        args.c = DEFAULT_CONFIG_FILE

    log.info('Use configuration file "' + args.c+'"')
    return args.c
    
def getConfiguration(confFile: str)->bool:
    global serverName
    global serverPort
    
    config = configparser.ConfigParser()
    if config.read(confFile) == []:
        log.error('Unable to open configuration file "' + confFile + '"')
        return False
    
    # print (config.sections())
    if not 'gateway' in config:
        log.error('No "gateway" section in configuration file')
        return False
    
    try:
        serverName = config['gateway']['server']
        serverPort = int(config['gateway']['port'])
    except Exception as e:
        log.error('Misformatted configuration file (' + str(e) + ')')
        return False
    
    return True

def main():
    global netman
    global serialDevice
    global serverName
    global serverPort
    global panID
    global chanID
    global gatewayName
    global rssi_device
    
    print ("\nSmartRoom gateway v. 1.0\n")
    if not getConfiguration(parseCommandLine()):
        exit(-1)
    
    # Initialisation des differents GPIO
    log.info ("Initialize I/O")

    try:
        # Clear I/O config
        if GPIO.getmode() != None:
            GPIO.cleanup()

        # Start configuration
        GPIO.setmode(GPIO.BOARD)
        
        GPIO.setup(GPIO_LED_ERROR[0], GPIO_LED_ERROR[1])
        GPIO.setup(GPIO_LED_ACTIVITY[0], GPIO_LED_ACTIVITY[1])
        GPIO.setup(GPIO_LED_PWR[0], GPIO_LED_PWR[1])
        GPIO.setup(GPIO_XBEE_SLEEP_RQ[0], GPIO_XBEE_SLEEP_RQ[1])
        GPIO.setup(GPIO_XBEE_SLEEP[0], GPIO_XBEE_SLEEP[1])
        GPIO.setup(GPIO_XBEE_RESET[0], GPIO_XBEE_RESET[1])
        GPIO.setup(GPIO_XBEE_ASSOCIATE[0], GPIO_XBEE_ASSOCIATE[1])
        GPIO.setup(GPIO_XBEE_RSSI_PWM[0], GPIO_XBEE_RSSI_PWM[1])

        GPIO_clear(GPIO_XBEE_SLEEP_RQ)
        GPIO_set(GPIO_XBEE_RESET)
        GPIO_clear(GPIO_LED_ERROR)
        GPIO_clear(GPIO_LED_ACTIVITY)
        GPIO_clear(GPIO_LED_PWR)

        # Reset XBEE
        GPIO_clear(GPIO_XBEE_RESET)
        time.sleep(0.1) # Maintains Reset during 100ms
        GPIO_set(GPIO_XBEE_RESET)

        time.sleep(2.0) # Wait 2 seconds to be sure XBEE is ready

        GPIO_set(GPIO_LED_PWR)
    except:
        pass # Initialization no needed as we are not executing on RPI
    
    log.info("Retrieve configuration from server " + serverName +":"+str(serverPort))

    serverConnect=False
    while not serverConnect:
        try:
            netman = NetworkMgr(serverName, serverPort)
            serverConnect = True
        except Exception as e:
            serverConnect = False
            log.warning("Unable to connect to server (" + str(e)+ ")")
            log.warning("Next try in 30 s")
            time.sleep(30.0)
            
    #log.info("Connection successful")
    
    msg = Message()
    msg.type = Message.CMD_CONFIG
    try:
        ans = netman.sendMessage(msg)
    except RuntimeError as e:
        log.error("Unable to communicate with server (" + str(e)+ "): Abort")
        exit(1)
    
    if "ERR" in ans.type:
        log.error("Unable to get configuration from server (err= " + hex(int(ans.data[0], base=16))+ "): Abort")
        exit(1)
        
    panID = 0
    chanID =0
    gatewayName= ""
    
    try:
        panID = int(ans.data[0],base=16)
        chanID =  int(ans.data[1],base=16)  
        gatewayName = ans.data[2]
    except Exception as e:
        log.error("Invalid answer from server (" + str(e)+ "): Abort")
        log.error("Offensing answer is: " + str(ans.raw_msg))
        exit(1)
    
    comServerThreadId = Thread(target=threadServer, args=())
    comServerThreadId.daemon=True
    comServerThreadId.start()
    
    log.info("PANID: " + hex(panID) + ", CHANID: " + hex(chanID) + ", Gateway name: " + gatewayName)

    # Start Xbee RX thread
    try:
        serialDevice = "/dev/serial0"
        monXbee = xbee.XBEE(receiveCallback=RxCallback, com=serialDevice, baudrate=9600)
    except:
        try:
            serialDevice = "/dev/ttyUSB0"
            monXbee = xbee.XBEE(receiveCallback=RxCallback, com=serialDevice, baudrate=9600)
        except Exception as e:
            log.error("Unable to open serial port "+ serialDevice + " for Xbee ("+ str(e) + ")")
            
    log.info("Opened serial device {} at {} bauds for Xbee".format(serialDevice, str(9600)))
    log.info("Reconf of xbee with new panID/ChanID")

    # change ChanID
    ans=monXbee.sendATRequest("CH",chanID.to_bytes(length=2, byteorder='big'))
    if ans.status != 0:
        log.error("Unable to configure Xbee channel to " + hex(chanID))
        log.error("Answer frame is: " + str(ans))
        exit(5)
    
    # change PanID
    ans=monXbee.sendATRequest("ID",panID.to_bytes(length=2, byteorder='big'))
    if ans.status != 0:
        log.error("Unable to configure Xbee panID to " + hex(panID))
        log.error("Answer frame is: " + str(ans))
        exit(5)
    
    try:
        comServerThreadId.join()
    except KeyboardInterrupt:
        log.info("Gateway stopped by user")

    # Clear I/O config
    try:
        GPIO.cleanup()
    except:
        pass
    
    print ("bye bye")

if __name__ == '__main__':
    main()
