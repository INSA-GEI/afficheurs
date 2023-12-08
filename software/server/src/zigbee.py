# zigbee.py
#
#

from digi.xbee.devices import XBeeDevice, ZigBeeDevice
from digi.xbee.util import utils

class Zigbee:
    device=None
    device_64bit_addr=None
    device_16bit_addr=None
    device_panid=None

    def open(self, port:str, baudrate:int)->None:
        self.device = ZigBeeDevice(port, baudrate)
        self.device.open()

    def getCurrentConfiguration(self)->None:
        self.device_64bit_addr = self.device.get_64bit_addr()
        self.device_16bit_addr = self.device.get_16bit_addr()
        self.device_panid = self.device.get_pan_id()
    
    def __str__(self) -> str:
        if self.device is not None:
            return str(self.device)
    
    def close(self)->None:
        self.device.close()

    def write(self, string:str):
        self.device.send_data_64_16()