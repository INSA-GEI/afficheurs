# un truc ici

import socket
import messages

class NetworkMgr():
    server_name: str=""
    server_port: int =0
    
    def __init__(self, server: str, port: int):
        # connection au server et recuperation de la configuration
        self.server_name = server
        self.server_port = port
        
    def sendMessage(self, msg:messages.Message) -> messages.Message:
        # Connect to server
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        try:
            #print ("Open socket to {}:{}".format(self.server_name, self.server_port))
            self.sock.connect((self.server_name, self.server_port))
        except ConnectionRefusedError:
            raise RuntimeError("Server not reachable")
        
        m = msg.encode_to_srv()
        
        # Send message to server
        try:
            self.sock.send(m.encode("utf-8"))
        except Exception as e:
            raise RuntimeError("Unable to send data to server: " + str(e))
        
        # wait for answer
        try:
            ans = self.__get_answer()
        except Exception as e:
            raise RuntimeError("Connection broken while receiving answer: " + str(e))
        
        try:
            self.sock.close()
        except Exception as e:
            raise RuntimeError("Unable to close socket: " + str(e))
        
        m = messages.Message()
        m.decode_from_srv(ans)
        return m
        
    def __get_answer(self) -> str:
        chunks = []
        bytes_recd = 0
        last_char=0
        
        while True:
            while last_char != 0x0A:
                chunk = self.sock.recv(2048)
                if chunk == b'':
                    raise RuntimeError("Socket connection broken")
                chunks.append(chunk)
                bytes_recd = bytes_recd + len(chunk)
                last_char=chunk[-1]
            
            return b''.join(chunks).decode("utf-8")

        
        