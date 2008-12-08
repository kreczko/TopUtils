import os
import sys
import getopt
from Timer import Timer
import threading
#set your config file here
import analyzeQCDBackground_cfg as cms
import mvaAnalysis_cfg as mva

SENDMAIL = "/usr/sbin/sendmail" # sendmail location

#class documentation
"executive script for a simple run, Version 1.0"
"twiki: https://twiki.cern.ch/twiki/bin/view/CMS/ConfigRunner"
class CfgRunner:
    
    #time in seconds to wait between commands
    __sleeptime = 10
    #for the -a option
    __additionalParameters = ''
    #executing path is in most cases the outputpath.
    #you can change it here
    #example: self.filepath = 'outputpath/rootfiles/'
    __filepath = 'cfgR/' 
    __fileprefix = 'MatrixMethod_' #will be used as filnameprefix
    __filesuffix = '.root' #filetype
    #sampletype
    __type = ''     
    __numberofevents = 0
    #standard output file
    __outputfile = 'output.txt'
    #standard output file for error stream
    __outputerr = 'outputErr.txt'
    #__runs_ = ""
    __jobstarted = False
    #not used at the moment
    __verbose = False
    __errorToken = ['RootError', 'Exception', 'Root_Error']
    __cfg = 'matrix'
    __skipevents = 0
    #email address used for notification
    __email = 'lkreczko@mail.desy.de'
    
    def __init__(self):
        self.__cmsRunTimer = {}
        self.__analysisTimer = {}
##--------------------------------------------------------------------------------------------        
    def main(self):
    #possible arguments:
        # parse command line parameter
        try:
            opts, args = getopt.getopt(sys.argv[1:], 'ht:e:s:f:c:a:v', ['help', 'type=', 'events=', 'out=', 'err=', 'add=', 'cfgt=', 'skip='])
        except getopt.error, msg:
            print msg
            print "for help use --help"
            sys.exit(2)
        # process parameter
        for o, a in opts:
            #shows class documentation
            if o in ("-h", "--help"):
                print __doc__
                sys.exit(0)
            #sampletype defined in SourceInput
            elif o in ("-t", "--type"):
                self.__type = a
            #number of events
            elif o in ("-e", "--events"):
                self.__numberofevents = int(a)
            #skip number of events
            elif o in ('-s', '--skip'):
                self.__skipevents = int(a)
            #output file
            elif o in ("-f", "--out"):
                if(not (a == '')):
                    self.__outputfilename = a
            #error output file
            elif o in ("--err"):
                if(not a == ''):
                    self.__outputerr = a
            #additional parameter for Config
            elif o in ("--add", '-a'):
                if(not a == ''):
                    self.__additionalParameters = a
            elif o in ('-v'):
                self.__verbose = True
            #config type, until now matrix or mva
            elif o in ('-c', '--cfgt'):
                self.__cfg = a
            else:
                print 'Argument(s) not recognized. See --help for usage'
                
        if (not self.__numberofevents == 0) and (not self.__type == ''):
            self.__doAll()
##-------------------------------------------------------------------------------    
    "executes the CMSSW runable"        
    def __executeCMSrun(self, configfile):
        print 'Executing cmsRun...'
        if os.path.exists(configfile):
            print '>> cmsRun ' + configfile
            if os.path.exists(self.__outputfile):
                os.remove(self.__outputfile)
            if os.path.exists(self.__outputerr):
                os.remove(self.__outputerr)   
            
            #setup runtime environment
            #os.system('eval `scramv1 runtime -sh`')# !!not working!!
            # nohup
#            eval('eval `scramv1 runtime -sh`')
            os.system('eval `scramv1 runtime -sh` | cmsRun ' + configfile + " >" + self.__outputfile + " 2> " + self.__outputerr + " < /dev/null&")
            self.__cmsRunTimer[self.__type].start()
            print ''
            self.__waitForFirst(self.__type)
            os.remove(configfile)
            self.__jobstarted = True
        else:
            print 'requested configfile does not exist'
        
##--------------------------------------------------------------        
    "does some things on the end of a job"
    def __endJob(self, type):
        print "starting something for ", type
        subject = 'ConfigRunner Job successfully ended'
        msg = 'ConfigRunner just finished the sample "' + type + '"\n\n'
        msg += 'following parameters were used:\n'
        msg += '- number of events: ' + self.__numberofevents.__str__() + '\n'
        msg += '- events skipped: ' + self.__skipevents.__str__() + '\n'
        msg += '- additional Config parameter: ' + self.__additionalParameters + '\n'
        msg += '- Config used: ' + self.__cfg + '\n\n'
        msg += 'time needed for starting cmsRun: ' + self.__cmsRunTimer[type].getMeasuredTime() + '\n'
        msg += 'time needed for analysis: ' + self.__analysisTimer[type].getMeasuredTime() + '\n'
        self.sendEmail(subject, msg)

        

    ##################################################
    # wait for 1st event to be processed
    ##################################################
    "waits for the first event to be processed"        
    def __waitForFirst(self, type):
        print '#########################################################################'
        print "###  Waiting for the 1st event of '" + str(type) + "' to be processed"
        print '#########################################################################' 
        printEvery = self.__sleeptime * 1.3
        msg = '...'
        #print 'waiting up to 10 times
        printtimer = {}
        for i in range(0, 10):
            printtimer[i] = threading.Timer(printEvery * i, self.__printMsg, [msg])
            printtimer[i].start()
        #waits until the files are created by cmsRun
        while(not os.path.exists(self.__outputerr)):
            Timer.sleep(1)
            #if after 6 min no file is created, script aborts
            if self.__cmsRunTimer[type].timePassed(os.times()) > 360:
                msg =  "cmsRun needs too long, aborting."
                print msg
                self.sendErrorNotification(msg)
                os.sys.exit(- 1)
            #waits until first event has been read
        while (self.__readFromFile(self.__outputerr) == ""):
            Timer.sleep(1)
            
        for i in printtimer:
            printtimer[i].cancel()
        print ''
        print "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"
        print self.__readFromFile(self.__outputerr)
        print "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
        if not 'Begin processing the 1st record.' in self.__readFromFile(self.__outputerr):
            print 'an error occured'
            self.sendErrorNotification('an error occured' + self.__readFromFile(self.__outputerr))
            os.sys.exit(- 1)
        self.__cmsRunTimer[type].stop()
        print 'Time to start cmsRun (s):', self.__cmsRunTimer[type].getMeasuredTime() 
        self.__analysisTimer[type].start()
        #start analysis timer
##-------------------------------------------------
    def __readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str
 ##-------------------------------------------------   
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
                    print ""
                    thread = threading.Thread(target=self.__waitingForEnd, args=(self.__type,))
                    thread.start()
                    self.__jobstarted = False
            elif self.__type == 'quit':
                os._exit(0)
            else:
                print "Oh no! Not allowed type used!"
                print "allowed types: ", cms.Config.allowedTypes
                os._exit(1)
        
 ##-------------------------------------------------               
    def __doJob(self, type):
        #create Config
        process = None
        if self.__cfg == 'matrix':
            process = cms.Config(type, self.__additionalParameters)
        elif self.__cfg == 'mva':
            process = mva.Config(type, self.__additionalParameters)
            
        process.modifyOption('skipevents', self.__skipevents)            
        process.modifyOption('events', self.__numberofevents)
        if not os.path.exists(self.__filepath):
            os.mkdir(self.__filepath)
        #TODO: move to ConfigWrapper
        output = self.__fileprefix + type + "_" + Timer.getDate() + self.__filesuffix
        process.modifyOption('output', output)
        #setup outputfiles
        self.__outputfile = self.__filepath + 'output_' + output.__str__().replace(self.__filesuffix, '.txt')
        self.__outputerr = self.__filepath + 'outputErr_' + output.__str__().replace(self.__filesuffix, '.txt')
        #initilize timers
        self.__cmsRunTimer[type] = Timer()
        self.__analysisTimer[type] = Timer()
        #start counting
        self.__executeCMSrun(process.returnTempCfg())
 ##-------------------------------------------------               
    def __createRuns(self, command):
        #print 'Parsing command...'
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
##-------------------------------------------------    
    def __waitingForEnd(self, type):
        err = False
        output = self.__fileprefix + type + "_" + Timer.getDate() + self.__filesuffix
        erO = self.__filepath + 'outputErr_' + output.__str__().replace(self.__filesuffix, '.txt')
        printEvery = self.__sleeptime
        if (self.__numberofevents == - 1):
            #every 30min
            printEvery = self.__sleeptime * 180
        else:
            #every 100s for each 1k events
            t = self.__numberofevents / 100 - self.__numberofevents % 100
            printEvery = self.__sleeptime * t
        
        #print 'waiting up to 100 times
        printtimer = {}
        msg = "### waiting for '" + str(type) + "' to end..."
        #TODO: add last line off the outputfileErr
        if printEvery > 0:
            for i in range(0, 100):
                printtimer[i] = threading.Timer(printEvery * i, self.__printMsg, [msg])
                printtimer[i].start()
                    
        while not 'Summary' in self.__readFromFile(erO) and not err:            
            if "Root_Error" in self.__readFromFile(erO):
                msg = "an error occured in " + type + ' sample'
                print msg
                msg += '\n Output file: \n\n' + self.__readFromFile(erO)
                err = True
                self.sendErrorNotification(msg)
            Timer.sleep(1)
            if((self.__analysisTimer[type].timePassed(os.times()) % printEvery) == 0):
                print 'waiting for', type, 'to end...'
        print type, 'ended'
        if printEvery > 0:
            for i in printtimer:
                printtimer[i].cancel()
        #TODO: printing long time, but timer stop should be independent
        self.__analysisTimer[type].stop()
        print 'Time needed for analysis (s):', self.__analysisTimer[type].getMeasuredTime()
        self.__endJob(type)

##-------------------------------------------------
    def sendEmail(self, subject, msg):
        p = os.popen("%s -t" % SENDMAIL, "w")
        p.write("To: " + self.__email + "\n")
        p.write("Subject: " + subject + "\n")
        p.write("\n") # blank line separating headers from body
        p.write(msg)
        sts = p.close()
        if sts != 0 and sts != None:
            print "Sendmail exit status", sts   
            
##-------------------------------------------------            
    def sendErrorNotification(self, msg):
        subject = 'ConfigRunner error occured'
        self.sendEmail(subject, msg)

##-------------------------------------------------        
    #used for timed output
    #this won't be needed with python > 3.0
    def __printMsg(self, msg):
        print msg
        

##-------------------------------------------------
## start function
##-------------------------------------------------
if __name__ == '__main__':
    runner = CfgRunner()
    runner.main()
    
