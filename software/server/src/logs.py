# logs.py
#
#

from common import Room

class DisplayReport:
    name:str
    roomslist:list

    def __init__(self, name, roomslist):
        self.name = name
        self.roomslist = roomslist

    def write(self):
        f = open(self.name, "w")

        f.write('='*50)
        f.write('\n')

        for room in self.roomslist:
            for d in room.displays:
                f.write("| {} | {} | {}/{}/{} |".format(str("0x{:016X}".format(d.id)), str(d.lastSeen), str(d.displayMinRSSI), str(d.displayMaxRSSI), str(d.displayMoyRSSI)))
                f.write('\n')
                f.write('-'*50)
                f.write('\n')
                
        f.close()
