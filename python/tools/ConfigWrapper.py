## system imports
import os
import sys

## timer
from Timer import Timer


##-----------------------------------------------------------------------------------
## wrapper class for a cfg to python transition
## twiki: https://twiki.cern.ch/twiki/bin/view/CMS/ConfigRunner#ConfigWrapper
class ConfigWrapper:
    ##
    ## configurables
    ##
    __config       = ''   ## input  config
    __outputConfig = ''   ## output config
    __pathcounter  = 1000 ## counts the number of paths to be changed/added in
                          ## the predefined config file; 1000 leaves room for 
                          ## user defined process paths in the predefined cfg
                          ## file

##-----------------------------------------------------------------------------------
##  Constructor; options are source, output, event, paths
##  these can be changed within a predefined config file
    def __init__(self, cfg, output, type):
        ## read input config from source file 
        self.__config = self.readFromFile(cfg)
        ## create output config filename            
        self.__outputConfig = output
        ## define options
        self._options = {}
        self._options['source'] = ""  ## change source
        self._options['output'] = ""  ## change output 
        self._options['events'] = ""  ## change number of events
        self._options['skips' ] = ""  ## change number of skipped events
        self._options['paths' ] = ""  ## add paths if specified

##-----------------------------------------------------------------------------------        
##  * modifies an option
    def modifyOption(self, optionName, value):
        if optionName in self._options.keys():
            self._options[optionName] = value
        else:
            print 'config option' , optionName, 'not found'    

##-----------------------------------------------------------------------------------        
##  * reads a file into a string
    def readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str

##-----------------------------------------------------------------------------------            
##  * writes a string to a file (overwriting existing files)
    def writeToFile(self, filename, str):
        file = open(filename, 'w')
        file.write(str)
        file.close()

##-----------------------------------------------------------------------------------
##  * adds a path to the cfg file; must be given as: module1 * module2 + ...aso
    def addPath(self, path):
        self._options['paths'] += "process.p" + self.__pathcounter.__str__()
        self._options['paths'] += " = cms.Path(" + path + ")\n"
        self.__pathcounter += 1

##-----------------------------------------------------------------------------------
##  * replace 'search' by 'replace' in the input config file
    def _replaceInFile(self, search, replace):
        self.__config = self.__config.replace(search.__str__(), replace.__str__())
        
##-----------------------------------------------------------------------------------
##  * replace all entries which were mapped to the option keys in inpit config file
    def _replaceAll(self):
        self.__config += '\n'
        self.__config += '####################################################### \n'
        self.__config += '## \n'
        self.__config += '## automatic parameter replacement \n'
        self.__config += '## \n'
        self.__config += '####################################################### \n'
        for a in self._options.keys():
            if a in ('source'):
                ## modify input files                
                self.__config += 'process.source.fileNames  = ['
                self.__config += self._options[a] + ']'
                self.__config += '\n'                
            if a in ('events'):
                ## modify event numbers
                self.__config += 'process.maxEvents.input   = '
                self.__config += self._options[a].__str__()
                self.__config += '\n'
            if a in ('skips' ):
                ## modify skip numbers
                self.__config += 'process.source.skipEvents = cms.untracked.uint32('
                self.__config += self._options[a].__str__() + ')'
                self.__config += '\n'                
            if a in ('output'):
                ## modify output file
                if(not self._options[a].lower() == 'none'):
                    self.__config += 'process.TFileService.fileName = \''
                    self.__config += self._options[a] + '\''
                    self.__config += '\n'                                
            if a in ('paths' ):
                ## modify event numbers
                self.__config += self._options[a]
                self.__config += '\n'                    
                
##-----------------------------------------------------------------------------------
##  * returns the name of the temporary config file
    def returnTempCfg(self, path='.'):
        ## replace all specified parameters in
        ## the input config
        self._replaceAll()
        ## create job directory if it doesn't
        ## exist yet
        if (not os.path.exists(path)):
            os.system('mkdir ' + path)
        ## define output path
        outpath = path + '/' + self.__outputConfig
        self.writeToFile(outpath, self.__config)
        return outpath

##-----------------------------------------------------------------------------------    
    def endJob(self, type):
        print 'nothing defined for: ', type
        
    endJob = staticmethod(endJob)
