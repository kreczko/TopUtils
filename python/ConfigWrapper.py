from Timer import Timer

"A Wrapper for a cfg to python transition"
class ConfigWrapper:
    __config = ''
    __outputConfig = ''
    __pathcounter = 0

    
    def __init__(self, cfg, type):
        self.__config = self.readFromFile(cfg)
        self.__outputConfig = "tempconfig_" +type + "_" + Timer.getTime() + ".cfg"  
        
        #make protected
        self._options = {}
        self._options['source'] = ""
        self._options['output'] = ""
        self._options['events'] = ""
        self._options['paths'] = ""
        
    def modifyOption(self, optionName, value):
        if optionName in self._options.keys():
            self._options[optionName] = value
        else:
            print 'Config option' , optionName, 'not found'    
        
#    def source(self, source):
#        if source in input.source.keys():
#            self.__source = input.source[source]
#        else:
#            print 'invalid source'
#            print 'accepted sources: ', input.source.keys()  
#        
    def addPath(self, path):
        self._options['paths'] += "path p" + self.__pathcounter.__str__() + " = {" + path + "}\n"
        self.__pathcounter += 1
        
    def _replaceInFile(self, search, replace):
        self.__config = self.__config.replace(search.__str__(), replace.__str__())
        
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
        
    #make protected
    def _replaceAll(self):
        for a in self._options.keys():
            self._replaceInFile('{$' + a.__str__() + '}', self._options[a])
        
        
    def returnTempCfg(self):
        self._replaceAll()
        self.writeToFile(self.__outputConfig, self.__config)
        return self.__outputConfig