import os
import sys
import getopt
from Timer import Timer
import threading
#set your config file here
import analyzeQCDBackground_cfg as cms

"executive script for a simple run"
class CfgRunner:
    #TODO: dynamic inport of needed Confug
    __standardOut = 'output.txt'
    __standardErr = 'outputErr.txt'
    #time in seconds to wait between commands
    __sleeptime   = 10
    #for the -a option
    __additionalParameters = ''
    #executing path is in most cases the outputpath.
    #you can change it here
    #example: self.filepath = 'outputpath/rootfiles/'
    __filepath    = '' 
    __fileprefix  = 'MatrixMethod_new_binning_' #will be used as filnameprefix
    __filesuffix  = '.root' #filetype
    #sampletype
    __type        = ''     
    __numberofevents = 0
    __outputfile  = 'output.txt'
    __outputerr   = 'outputErr.txt'
    __runs_       = ""
    __jobstarted  = False
    __verbose     = False
    __errorToken  = ['RootError', 'Exception', 'Root_Error']
    
    def __init__(self):
        self.__cmsRunTimer   = {}
        self.__analysisTimer = {}
        
    def main(self):
    #possible arguments:
        # parse command line parameter
        try:
            opts, args = getopt.getopt(sys.argv[1:], 'ht:e:f:a:v', ['help', 'type=', 'events=', 'out=', 'err=', 'add='])
        except getopt.error, msg:
            print msg
            print "for help use --help"
            sys.exit(2)
        # process parameter
        for o, a in opts:
            if o in ("-h", "--help"):
                print __doc__
                sys.exit(0)
            elif o in ("-t", "--type"):
                self.__type = a
            elif o in ("-e", "--events"):
                self.__numberofevents = int(a)
            elif o in ("-f", "--out"):
                if(not (a == '')):
                    self.__outputfilename = a
            elif o in ("--err"):
                if(not a == ''):
                    self.__outputerr = a
            elif o in ("--add", '-a'):
                if(not a == ''):
                    self.__additionalParameters = a
            elif o in ('-v'):
                self.__verbose = True
            else:
                print 'Argument(s) not recognized. See --help for usage'
                
        if (not self.__numberofevents == 0) and (not self.__type == ''):
            self.__doAll()
    
    "executes the CMSSW runable"        
    def __executeCMSrun(self, configfile):
        print 'Executing cmsRun...'
        print "##################################################"
        if os.path.exists(configfile):
            print 'cmsRun ' + configfile
            if os.path.exists(self.__outputfile):
                os.remove(self.__outputfile)
            if os.path.exists(self.__outputerr):
                os.remove(self.__outputerr)   
            
            #setup runtime environment
            #os.system('eval `scramv1 runtime -sh`')# !!not working!!
            # nohup
            os.system('cmsRun ' + configfile + " >" + self.__outputfile + " 2> " + self.__outputerr + " < /dev/null&")
            Timer.sleep(self.__sleeptime)
            self.__waitForFirst(self.__type)
            os.remove(configfile)
            self.__jobstarted = True
        else:
            print 'requested configfile does not exist'
        
        
    "does some things on the end of a job"
    def __endJob(self, type):
        print "starting something for ", type
        

    ##################################################
    # wait for 1st event to be processed
    ##################################################
    "waits for the first event to be processed"        
    def __waitForFirst(self, type):
        #TODO: abort on error
        while (self.__readFromFile(self.__outputerr) == ""):
            print "Waiting for the 1st event of", type, "to be processed..."
            Timer.sleep(self.__sleeptime)
        print "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"
        print self.__readFromFile(self.__outputerr)
        print "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
        if not 'Begin processing the 1st record.' in self.__readFromFile(self.__outputerr):
            print 'an error occured'
            os.sys.exit(-1)
        self.__cmsRunTimer[type].stop()
        print 'Time to start cmsRun (s):', self.__cmsRunTimer[type].getMeasuredTime() 
        self.__analysisTimer[type].start()
        #start analysis timer
        
    def __readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str
    
    def __doAll(self):
        #check if only some samples are to be done
        #top  or     qcd;top
        #more than one kind of sample
        self.__runs = self.__createRuns(self.__type)
        #check if source-type is defined
        for i in self.__runs:
            self.__type = i

            if (self.__type in cms.Config.allowedTypes):              
                self.__doJob(self.__type)
                if self.__jobstarted:
                    print "job started"
                    thread = threading.Thread(target=self.__waitingForEnd, args = (self.__type,))
                    thread.start()
                    self.__jobstarted = False
            elif self.__type == 'quit':
                os._exit(0)
            else:
                print "not allowed type used"
                print "allowed types: ", cms.Config.allowedTypes
                os._exit(0)
        
                
    def __doJob(self, type):
        #create Config
        process = cms.Config(type, self.__additionalParameters)
        process.modifyOption('events', self.__numberofevents)
        output = self.__filepath + self.__fileprefix + type + "_" + Timer.getDate() + self.__filesuffix
        process.modifyOption('output',output)
        #setup outputfiles
        self.__outputfile = 'output_' + output.__str__().replace(self.__filesuffix, '.txt')
        self.__outputerr = 'outputErr_' + output.__str__().replace(self.__filesuffix, '.txt')
        #initilize timers
        self.__cmsRunTimer[type] = Timer()
        self.__analysisTimer[type] = Timer()
        #start counting
        self.__cmsRunTimer[type].start()
        self.__executeCMSrun(process.returnTempCfg())
                
    def __createRuns(self, command):
        print 'Parsing command...'
        sampletypes = []
        allruns = []
        #remove ' and " from command
        #print command
        if ';' in command:
            sampletypes = command.split(';')
        else:
            sampletypes = [command]
    
        
        for a in sampletypes:
            allruns.append(a)
            
        return allruns
    
    def __waitingForEnd(self, type):
        err = False
        output = self.__filepath + self.__fileprefix + type + "_" + Timer.getDate() + self.__filesuffix
        erO = 'outputErr_' + output.__str__().replace(self.__filesuffix, '.txt')
        while not 'Summary' in self.__readFromFile(erO) and not err:
            if (self.__numberofevents == -1):
                #every 30min
                Timer.sleep(self.__sleeptime*180)
            else:
            	#130s for each 1k events
                t = self.__sleeptime*self.__numberofevents/100
                Timer.sleep(t)
            if "Root_Error" in self.__readFromFile(erO):
                print "an error occured in", type, 'sample'
                err = True
            print 'waiting for', type, 'to end...'
        print type, 'ended'
        self.__analysisTimer[type].stop()
        print 'Time needed for analysis (s):', self.__analysisTimer[type].getMeasuredTime()
        self.__endJob(type)
        



if __name__ == '__main__':
    runner = CfgRunner()
    runner.main()
    
    
