# config.py
#
#

from datetime import date, datetime, time

class Configuration():
    MAJOR_VERSION = 1
    MINOR_VERSION = 0

    # Configuration files and location
    configFileName = "./software/server/src/sdsd.yml"
    logFileName = "./software/server/src/sdsd.log"
    logFile = ""
    verboseFlag = False
    
    rooms = []
    gateways = []
    
    @staticmethod
    def parseCommandLine(cmdline: str) -> None:
        status = False
    
        Configuration.startupPrompt()
        
        for arg in cmdline:
            print ("arg = %s" % (arg))
        
            if arg == '-v':
                Configuration.verboseFlag = True
            elif arg == '-h' or arg == '--help':
                status =True    
            else:
                status = True
            
        if status == True:
            Configuration.helpPrompt()
        
        if status == True :
            raise Exception("Command Line Error","Help requested or invalid parameter")
        
    @staticmethod    
    def startupPrompt():
        print ("sdsd ver %d.%d" % (Configuration.MAJOR_VERSION, Configuration.MINOR_VERSION))
        print ()
   
    @staticmethod
    def helpPrompt():
        print ("Command line options:")
        print ("\t-h/--help: this help message")
        print ("\t-v: verbose output")
        print ("\t-c <file>: use <file> as configuration file")
        print ("\t-l <file>: use <file> as log file")
     
    @staticmethod   
    def openLogFile():
        try:
            Configuration.logFile = open(Configuration.logFileName, "a")
            Configuration.logFile.write("\n---------------------------------------------------------")
            Configuration.logFile.write("\nsdsd ver %d.%d" % (Configuration.MAJOR_VERSION, Configuration.MINOR_VERSION))
            Configuration.logFile.write("\nStart: " + str(datetime.now()))
            Configuration.logFile.write("\n\n")
            
            Configuration.isLogOpen=True
        except:
            print ("Warning: Unable to open log " + Configuration.logFile)
            Configuration.isLogOpen=False
    
    @staticmethod   
    def writeLogFile(s):
        if Configuration.isLogOpen:
            Configuration.logFile.write(s+"\n")
            Configuration.logFile.flush()
    
    @staticmethod   
    def closeLogFile():
        if Configuration.isLogOpen:
            Configuration.logFile.close()
    
    @staticmethod   
    def writeVerbose(s):
        if Configuration.verboseFlag:
            print (s)
        
        