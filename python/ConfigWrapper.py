import time
import datetime
import SourceInput as input

class ConfigWrapper:
    def __init__(self, cfg):
        self.config = self.readFromFile(cfg)
        rrr = time.mktime(datetime.datetime.utcnow().timetuple()).__str__()
        self.outputConfig = "tempconfig_" + rrr + ".cfg";
        self.pathcounter_ = 0;
        self.paths = "";
        self.source_ = ""
        #default values
        self.numberOfEvents_ = 10
        self.outputRootfile = "test.root"
        
        
    def source(self, source):
        print source, "  ", input.source.keys()
        if source in input.source.keys():
            self.source_ = input.source[source]
        else:
            print 'invalid source' 
        
    def addPath(self, path):
        self.paths += "path p" + self.pathcounter_.__str__() + " = {" + path + "}\n"
        self.pathcounter_ += 1
        
    
    def out(self, outputfile):
        self.outputRootfile = outputfile
        
    def events(self, numberOfEvents):
        self.numberOfEvents_ = numberOfEvents
        
    def replaceInFile(self, search, replace):
        #print search, replace
        self.config = self.config.replace(search, replace)
        
    def readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str
    
    #writes a string to a file (overwrites the file)
    def writeToFile(self, filename, str):
        file = open(filename, 'w')
        file.write(str)
        file.close()
        
    def replaceAll(self):
        self.replaceInFile("{$source}", self.source_)
        self.replaceInFile("{$out}", self.outputRootfile)
        self.replaceInFile("{$events}", self.numberOfEvents_.__str__())
        self.replaceInFile("{$paths}", self.paths)
        
        
    def returnTempCfg(self):
        self.replaceAll()
        self.writeToFile(self.outputConfig, self.config)
        return self.outputConfig