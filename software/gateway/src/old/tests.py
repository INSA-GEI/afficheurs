# un truc ici

import xbee
from networkmgr import NetworkMgr
import messages

device_addr = 0x0013a20041c02be8

raw_frame_JOIN = bytes([0x7E, 00, 0x0F ,0x80 ,0x00 ,0x13 ,0xA2 ,0x00 ,0x41 ,0xC0,\
                        0x2B ,0xE8 ,0x2A ,0x01 ,0x4A ,0x4F ,0x49 ,0x4E ,0x5B])

raw_frame_SETUP = bytes([0x7E, 00, 0x0F ,0x80 ,0x00 ,0x13 ,0xA2 ,0x00 ,0x41 ,0xC0,\
                        0x2B ,0xE8 ,0x2A ,0x01 ,0x4A ,0x4F ,0x49 ,0x4E ,0x5B])

def tests_srv(srv: NetworkMgr) -> bool:
    
    print("\nServer tests started\n\n")
    
    # print ("Request gateway configuration: Ok\n")
    # msg = messages.CMD_Config()
    # print ("\nMsg sent to server: " + str(msg.__class__))
    # print ("\n" + str(msg))
    # ans = srv.sendMessage(msg)
    # print ("\nReceived msg from server: " + str(ans.__class__))
    # print ("\n" + str(ans))
    
    # print ("\n\nRequest gateway configuration: Error\n")
    # msg = messages.CMD_Config()
    # print ("\nMsg sent to server: " + str(msg.__class__))
    # print ("\n" + str(msg))
    # ans = srv.sendMessage(msg)
    # print ("\nReceived msg from server: " + str(ans.__class__))
    # print ("\n" + str(ans))
    
    # print ("\n\nDisplay join request: accepted\n")
    # msg = messages.CMD_Join_Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_JOIN)
    # msg.decode_from_xbee(frame)
    
    # print ("\nMsg sent to server: " + str(msg.__class__))
    # print ("\n" + str(msg))
    # ans = srv.sendMessage(msg)
    # print ("\nReceived msg from server: " + str(ans.__class__))
    # print ("\n" + str(ans))
    
    # print ("\nXbee frame to be sent: " + str(ans.encode_to_xbee(1,0x02).__class__))
    # print ("\n" + str(ans.encode_to_xbee(1,0x02)))
    
    # print ("\n\nDisplay join request: rejected\n")
    # msg = messages.CMD_Join_Message()
    # frame = xbee._64Bit_Receive_Packet(raw_frame_JOIN)
    # msg.decode_from_xbee(frame)
    
    # print ("\nMsg sent to server: " + str(msg.__class__))
    # print ("\n" + str(msg))
    # ans = srv.sendMessage(msg)
    # print ("\nReceived msg from server: " + str(ans.__class__))
    # print ("\n" + str(ans))
    
    # print ("\nXbee frame to be sent: " + str(ans.encode_to_xbee(1,0x02).__class__))
    # print ("\n" + str(ans.encode_to_xbee(1,0x02)))
    
    print ("\n\nDisplay setup request\n")
    msg = messages.CMD_Join_Message()
    frame = xbee._64Bit_Receive_Packet(raw_frame_JOIN)
    msg.decode_from_xbee(frame)
    
    print ("\nMsg sent to server: " + str(msg.__class__))
    print ("\n" + str(msg))
    ans = srv.sendMessage(msg)
    print ("\nReceived msg from server: " + str(ans.__class__))
    print ("\n" + str(ans))
    
    print ("\nXbee frame to be sent: " + str(ans.encode_to_xbee(1,0x02).__class__))
    print ("\n" + str(ans.encode_to_xbee(1,0x02)))
    
    print ("\n\nTests finished")