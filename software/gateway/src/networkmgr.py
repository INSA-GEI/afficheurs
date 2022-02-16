# un truc ici

import socket
import messages

class NetworkMgr():
    server_name: str=""
    server_port: int =0
    
    def __init__(self, server: str, port: int):
        # connection au server et recuperation de la configuration
        
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        try:
            self.sock.connect((self.server_name, self.server_port))
        except ConnectionRefusedError:
            raise RuntimeError("Server not reachable")
        
    def sendMessage(self, msg:messages.Message) -> messages.Message:
        # Connect to server
        
        try:
            self.sock.connect((self.server_name, self.server_port))
        except ConnectionRefusedError:
            raise RuntimeError("Server not reachable")
        
        m = msg.encode()
        
        # Send message to server
        try:
            self.sock.send(m.encode("ascii"))
        except Exception as e:
            raise RuntimeError("Unable to send data to server: " + str(e))
        
        # wait for answer
        try:
            ans = self.__get_answer()
        except Exception as e:
            raise RuntimeError("Connection broken while receiving answer: " + str(e))
        
        return messages.decodeMessage(ans)
        
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
            
            return b''.join(chunks).decode("ascii")

        
        