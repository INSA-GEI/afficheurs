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
import messages

import tests

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

def RxCallback(decoded_frame:xbee.API_Frame)->None:
    print ("\nReceived frame: "+str(decoded_frame))
    
    try:
        GPIO_set(GPIO_LED_ACTIVITY)
        time.sleep(0.5)
        GPIO_clear(GPIO_LED_ACTIVITY)
    except:
        pass # not executing code on RPI

def main():

    # Initialisation des differents GPIO
    print ("Initialize I/O and Xbee")

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

        print ("System ready")
        GPIO_set(GPIO_LED_PWR)
    except:
        pass # Initialization no needed as we are not executing on RPI
    
    # comXbeeThreadId = Thread(target=threadComXbee, args=())
    # comXbeeThreadId.daemon=True
    # comXbeeThreadId.start()

    comServerThreadId = Thread(target=threadServer, args=())
    comServerThreadId.daemon=True
    comServerThreadId.start()

    # Start Xbee RX thread
    try:
        monXbee = xbee.XBEE(receiveCallback=RxCallback, com="/dev/serial0", baudrate=9600)
    except:
        monXbee = xbee.XBEE(receiveCallback=RxCallback, com="/dev/ttyUSB0", baudrate=9600)

    #tx_frame=xbee.Transmit_Request(1,0x0013a20041c02be8,0,"Salut les gars")
    #print ("\nTX Frame: " + str(tx_frame))
    #monXbee.sendFrame(tx_frame)
    
    netman = NetworkMgr('localhost', 5166)
    print ("Server tests: " + str(tests.tests_srv(netman)))
    
    try:
        comServerThreadId.join()
    except KeyboardInterrupt:
        print ("Server stopped by user")

    # Clear I/O config
    try:
        GPIO.cleanup()
    except:
        pass
    
    print ("bye bye")

if __name__ == '__main__':
    main()

