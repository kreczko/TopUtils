from Timer import Timer

"interface between old config language and python"
class InspectWrapper:
    def __init__(self, cfg, type):
        self.__config = self.__readFromFile(cfg)
        self.__outputConfig = "tempconfig_" + type + "_" + Timer.getTime() + ".cfg";
        
        #list of options
        self.__singleOptions = "histInput, filterOption, rootOutput, outputDir, writePlotsTo, writePlotsAs, legXLeft, legXRight, legYLower, legYUpper"
        self.__options = {}
        self.__options['histInput'] = ""
        self.__options['rootInput'] = ""
        self.__options['inputDirs'] = ""
        self.__options['histFilter'] = ""
        self.__options['plotFilter'] = ""
        self.__options['filterOption'] = ""
        self.__options['rootOutput'] = ""
        self.__options['outputDir'] = ""
        self.__options['outputLabels'] = ""
        self.__options['writePlotsTo'] = "."
        self.__options['writePlotsAs'] = "ps"
        self.__options['xLog'] = ""
        self.__options['yLog'] = ""
        self.__options['xGrid'] = ""
        self.__options['yGrid'] = ""
        self.__options['histScale'] = ""
        self.__options['histMinimum'] = ""
        self.__options['histMaximum'] = ""
        self.__options['histErrors'] = ""
        self.__options['histType'] = ""
        self.__options['histStyle'] = ""
        self.__options['histColor'] = ""
        self.__options['lineWidth'] = ""
        self.__options['markerStyle'] = ""
        self.__options['markerSize'] = ""
        self.__options['xAxes'] = ""
        self.__options['yAxes'] = ""
        self.__options['legEntries'] = ""
        self.__options['legXLeft'] = ""
        self.__options['legXRight'] = ""
        self.__options['legYLower'] = ""        
        self.__options['legYUpper'] = ""
        self.__options['outputFileName'] = ""
        #inspectSum options
        self.__sumOptions = 'histWeights'
        self.__options['histWeights'] = ""
        
    "changes an option of the config. Options which are global will be replaced, other will be expanded"
    def modifyOption(self, optionName, value):
        if optionName in self.__options.keys():
            if optionName in self.__singleOptions.split(","):       
                self.replaceOption(optionName, value)             
            else:
                self.__options[optionName] += value + '\n'
        else:
            print 'Inspect option not found'
            
    "Replaces an option in the config"
    def replaceOption(self, optionName, value):
        if optionName in self.__options.keys():
            self.__options[optionName] = value
        else:
            print 'Inspect option not found'
       
    "returns the name of the temporary created config-file" 
    def returnTempCfg(self):
        self.__replaceAll()
        self.__writeToFile(self.__outputConfig, self.__config)
        return self.__outputConfig
    
    def __replaceAll(self):
        for a in self.__options.keys():
            self.__replaceInFile('{$' + a.__str__() + '}', self.__options[a])
        
    def __replaceInFile(self, search, replace):
       # print search, replace
        self.__config = self.__config.replace(search, replace)
        
    def __readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str
    
    "writes a string to a file (overwrites the file)"
    def __writeToFile(self, filename, str):
        file = open(filename, 'w')
        file.write(str)
        file.close()
        