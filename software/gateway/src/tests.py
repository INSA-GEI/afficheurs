# un truc ici

import xbee
from networkmgr import NetworkMgr
from  messages import Message

device_addr = 0x0013a20041c02be8

raw_frame_JOIN = bytes([0x7E, 00, 0x0F ,0x80 ,0x00 ,0x13 ,0xA2 ,0x00 ,0x41 ,0xC0,\
                        0x2B ,0xE8 ,0x2A ,0x01 ,0x4A ,0x4F ,0x49 ,0x4E ,0x5B])

raw_frame_SETUP = bytes([0x7E, 0x00, 0x10, 0x80, 0x00, 0x13, 0xA2, 0x00, 0x41, 0xC0,\
                         0x2B, 0xE8, 0x2A, 0x01, 0x53, 0x45, 0x54, 0x55, 0x50, 0xFA])

raw_frame_CAL = bytes([0x7E ,0x00 ,0x0E ,0x80 ,0x00 ,0x13 ,0xA2 ,0x00 ,0x41 ,0xC0\
                      ,0x2B ,0xE8 ,0x2A ,0x01 ,0x43 ,0x41 ,0x4C ,0xBB])

raw_frame_UPDATE = bytes([0x7E ,0x00 ,0x11 ,0x80 ,0x00 ,0x13 ,0xA2 ,0x00 ,0x41 ,0xC0\
                         ,0x2B ,0xE8 ,0x2A ,0x01 ,0x55 ,0x50 ,0x44 ,0x41 ,0x54 ,0x45 ,0xC8])

raw_frame_REPORT = bytes([0x7E ,0x00 ,0x1C ,0x80 ,0x00 ,0x13 ,0xA2 ,0x00 ,0x41 ,0xC0\
                         ,0x2B ,0xE8 ,0x2A ,0x01 ,0x52 ,0x45 ,0x50 ,0x4F ,0x52 ,0x54 ,0x7C\
                         ,0x33 ,0x32 ,0x7C ,0x31 ,0x46 ,0x7C ,0x32 ,0x32 ,0x7C ,0x30 ,0x4F])

def tests_srv(srv: NetworkMgr) -> bool:
    
    print("\nServer tests started\n\n")
    
    print ("")
    print ("---------------------------------------")
    print ("Request gateway configuration: Ok")
    print ("---------------------------------------")
    print ("")
    msg = Message()
    msg.type = Message.CMD_CONFIG
    
    print ("\nMsg sent to server: ")
    print ("\n" + str(msg))
    
    try:
        ans = srv.sendMessage(msg)
        print ("\nReceived msg from server: ")
        print ("\n" + str(ans))
    except RuntimeError as e:
        print ("Exception RuntimeError raised: " + str(e)) 
    
    print ("")
    print ("---------------------------------------")
    print ("Request gateway configuration: Err")
    print ("---------------------------------------")
    print ("")
    msg = Message()
    msg.type = Message.CMD_CONFIG
    print ("\nMsg sent to server: ")
    print ("\n" + str(msg))
    
    try:
        ans = srv.sendMessage(msg)
        print ("\nReceived msg from server: ")
        print ("\n" + str(ans))
    except RuntimeError as e:
        print ("Exception RuntimeError raised: " + str(e))  
    
    # print ("")
    # print ("---------------------------------------")
    # print ("Screen want to JOIN: ACCEPT")
    # print ("---------------------------------------")
    # print ("")

    # msg = Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_JOIN)
    # msg.decode_from_xbee(frame)
    
    # print ("Msg sent to server: ")
    # print ("\n" + str(msg))
    
    # try:
    #     ans = srv.sendMessage(msg)
    #     print ("\nReceived msg from server: ")
    #     print ("\n" + str(ans))
    #     frame = ans.encode_to_xbee(1,0x02)
    #     print ("Encoded Xbee frame: " +str (frame))
    # except RuntimeError as e:
    #     print ("Exception RuntimeError raised: " + str(e)) 
    
    # print ("")
    # print ("---------------------------------------")
    # print ("Screen want to JOIN: REJECT")
    # print ("---------------------------------------")
    # print ("")
    
    # msg = Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_JOIN)
    # msg.decode_from_xbee(frame)
    
    # print ("Msg sent to server: ")
    # print ("\n" + str(msg))
    
    # try:
    #     ans = srv.sendMessage(msg)
    #     print ("\nReceived msg from server: ")
    #     print ("\n" + str(ans))
    #     frame = ans.encode_to_xbee(1,0x02)
    #     print ("Encoded Xbee frame: " +str (frame))
    # except RuntimeError as e:
    #     print ("Exception RuntimeError raised: " + str(e)) 
  
    # print ("")
    # print ("---------------------------------------")
    # print ("Screen requests SETUP: OK")
    # print ("---------------------------------------")
    # print ("")
    
    # msg = Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_SETUP)
    # msg.decode_from_xbee(frame)
    
    # print ("Msg sent to server: ")
    # print ("\n" + str(msg))
    
    # try:
    #     ans = srv.sendMessage(msg)
    #     print ("\nReceived msg from server: ")
    #     print ("\n" + str(ans))
    #     frame = ans.encode_to_xbee(1,0x00)
    #     print ("Encoded Xbee frame: " +str (frame))
    # except RuntimeError as e:
    #     print ("Exception RuntimeError raised: " + str(e)) 
        
    # print ("")
    # print ("---------------------------------------")
    # print ("Screen requests SETUP: ERR")
    # print ("---------------------------------------")
    # print ("")
    
    # msg = Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_SETUP)
    # msg.decode_from_xbee(frame)
    
    # print ("Msg sent to server: ")
    # print ("\n" + str(msg))
    
    # try:
    #     ans = srv.sendMessage(msg)
    #     print ("\nReceived msg from server: ")
    #     print ("\n" + str(ans))
    #     frame = ans.encode_to_xbee(1,0x00)
    #     print ("Encoded Xbee frame: " +str (frame))
    # except RuntimeError as e:
    #     print ("Exception RuntimeError raised: " + str(e)) 
        
    # print ("")
    # print ("---------------------------------------")
    # print ("Screen requests CAL: OK")
    # print ("---------------------------------------")
    # print ("")
    
    # msg = Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_CAL)
    # msg.decode_from_xbee(frame)
    
    # print ("Msg sent to server: ")
    # print ("\n" + str(msg))
    
    # try:
    #     ans = srv.sendMessage(msg)
    #     print ("\nReceived msg from server: ")
    #     print ("\n" + str(ans))
    #     frame = ans.encode_to_xbee(1,0x00)
    #     print ("Encoded Xbee frame: " +str (frame))
    # except RuntimeError as e:
    #     print ("Exception RuntimeError raised: " + str(e))
        
    # print ("")
    # print ("---------------------------------------")
    # print ("Screen requests CAL: ERR")
    # print ("---------------------------------------")
    # print ("")
    
    # msg = Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_CAL)
    # msg.decode_from_xbee(frame)
    
    # print ("Msg sent to server: ")
    # print ("\n" + str(msg))
    
    # try:
    #     ans = srv.sendMessage(msg)
    #     print ("\nReceived msg from server: ")
    #     print ("\n" + str(ans))
    #     frame = ans.encode_to_xbee(1,0x00)
    #     print ("Encoded Xbee frame: " +str (frame))
    # except RuntimeError as e:
    #     print ("Exception RuntimeError raised: " + str(e)) 
        
    print ("")
    print ("---------------------------------------")
    print ("Screen requests REPORT")
    print ("---------------------------------------")
    print ("")
    
    msg = Message()
    frame = xbee._64Bit_Receive_Packet(raw_frame_REPORT)
    msg.decode_from_xbee(frame)
    
    print ("Msg sent to server: ")
    print ("\n" + str(msg))
    
    try:
        ans = srv.sendMessage(msg)
        print ("\nReceived msg from server: ")
        print ("\n" + str(ans))
        frame = ans.encode_to_xbee(1,0x00)
        print ("Encoded Xbee frame: " +str (frame))
    except RuntimeError as e:
        print ("Exception RuntimeError raised: " + str(e))
        
    print ("\n\nTests finished")
    return True