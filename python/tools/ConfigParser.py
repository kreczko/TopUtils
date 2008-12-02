import xml.dom.minidom as parser
import os

class ConfigParser:
    def __init__(self, file):
        self.__file = 'TopAnalysis/TopUtils/test/DummyConfig.xml'
        self.__rootname = 'HistPlotter'
        self.__includeFiles = []#list of setups ['name'] = filepath
        self.__histDefinitions = []
        if os.path.exists(file):
            self.__file = file
        else:
            raise IOError, 'configuration file does not exist'
            
    def getDocument(self):
        return parser.parse(self.__file)
    
        
    def getNodeList(self, nodename):
        return self.getDocument().getElementsByTagName(nodename)

        
    def getRoot(self):
        root = self.getDocument().documentElement
        if root.localName != 'HistPlotter':
            raise ConfigError, 'not a HistPlotter configuration file'
        return root

    def readIncludes(self):
        includes = self.getNodeList('include')
        for i in includes:
            file = self.getAttributeValue(i, 'file')
            #file = self.getAttributeValue(i, 'value')
            if not file in self.__includeFiles:
                self.__includeFiles.append(file)
            else:
                raise ConfigError, 'multiple includes with same name ' + name.__str__()
        return self.__includeFiles
        
    def getAttributeValue(node, name):
        value = ''
        for i in range(node.attributes.length):
            if name == node.attributes.item(i).name:
                value = node.attributes.item(i).value
        if value == '':
            raise ConfigError, 'node has no attribute named ' + name.__str__()
        return value
    
    getAttributeValue = staticmethod(getAttributeValue)
    
    def readHistSetups(setups):
        histlist = []
        return ''

class ConfigError(Exception):
    def __init__(self, value):
        self.value = value
        
    def __str__(self):
        return repr(self.value)

class Histogramm:
    def __init__(self):
        self.logX = 0
        self.logY = 0   
        
    """
    Reads all histogram definitions from a file and returns a dictionary with histograms
    dic['histname'] = Histogram
    """
    def readHistSetupFromFile(file):
        if not os.path.exists(file):
            raise IOError, 'histogram configuration file not found'
        return ''
    def readHistSetupFromNode(node):
        return ''
    
    readHistSetupFromFile = staticmethod(readHistSetupFromFile)
    readHistSetupFromNode = staticmethod(readHistSetupFromNode)
        
class Variable:
    def __init__(self):
        print 'Im a var!'