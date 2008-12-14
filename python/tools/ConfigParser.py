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
        #return self.getNodeList(self.getDocument(), nodename)
    
    def getChildNodes(parentnode, name):
        ret = []
        for i in parentnode.childNodes:
            if i.localName == name:
                ret.append(i)
        return ret
    getChildNodes = staticmethod(getChildNodes)  
      
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
    
    def getInputFiles(self):
        ret = {}
        sources = self.getNodeList('sources')
        #exact one sources per config
        if len(sources) >1 or not sources:
            raise ConfigError, 'not exact one <sources> inside config file'
        files = self.getChildNodes(sources[0], 'file')
        #files = self.getNodeList('file')
        if not files:
            raise ConfigError, 'no input files defined'
        for i in files:
            key = self.getAttributeValue(i, 'id')
            value = self.getAttributeValue(i, 'name')
            ret[key] = value
        return ret
    
    def getFilenameByID(self, id):
        return self.getInputFiles()[id]
    
    def getInputs(self):
        ret = {}
        sources = self.getNodeList('sources')
        #exact one sources per config
        if len(sources) >1 or not sources:
            raise ConfigError, 'not exact one <sources> inside config file'
        #parent node of inputs has to be sources
        inputs = self.getChildNodes(sources[0],'input')
        for i in inputs:
            ret[self.getAttributeValue(i, 'name')] = Input.getFromNode(i)
        return ret
    def getPlots(self):
        plots = self.getNodeList('plots')
        if len(plots) >1 or not plots:
            raise ConfigError, 'not exact one <plots> inside config file'
        create = ConfigParser.getAttributeValue(plots[0], 'create')
        makeSum = ConfigParser.getAttributeValue(plots[0], 'makeSummaryFile')
        sumfile = ConfigParser.getAttributeValue(plots[0], 'summaryFile')
        #plots can contain either single histograms or hist lists
        return Plots(None, create, makeSum, sumfile)
    

class ConfigError(Exception):
    def __init__(self, value):
        self.value = value
        
    def __str__(self):
        return repr(self.value)

"""
container for all Plots
"""
class Plots:
    #define the output as ROOT can write it
    output = ['eps','ps','pdf','jpg','png','gif']
    def __init__(self,histlist, create = 'eps,png', makeSummary = 'false', sumfile ='inspect.ps'):
        self.fileOutputs = []
        for i in create.split(','):
            if i in self.output:
                self.fileOutputs.append(i)
            else:
                raise ConfigError, 'wrong output file format for histogram'
        if makeSummary.lower() == 'true':
            self.makeSummary = True
        else:
            self.makeSummary = False
        if sumfile.endswith('.ps') or sumfile.endswith('.pdf'):
            self.sumfile = sumfile
        else:
            self.sumfile = sumfile + '.ps'
        self.histlist = histlist
    
    def getFromNode(node):
        print 'f'
    getFromNode = staticmethod(getFromNode)
            
        
class Histogramm:
    def __init__(self, varlist, name):
        self.logX = 0
        self.logY = 0
        if name != '':
            self.name = name
        else:
            raise ConfigError, 'no histogram name defined'
        if varlist:   
            self.varlist = varlist
        else:
            raise ConfigError, 'no variable defined for histogram' + self.name
        
        
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
    
    def isValid(self):
        #define needed values
        return False
    
    readHistSetupFromFile = staticmethod(readHistSetupFromFile)
    readHistSetupFromNode = staticmethod(readHistSetupFromNode)
        
class Variable:
    colors ={'black' : 1,
             'red' : 2}
    operations = ['add', 'divide', 'substract']
    def __init__(self):
        #histogran
        self.hist = None
        #how the variable is produces
        self.operation = 'none'
        self.color = 'black'
        print 'Im a var!'
        
        
    def getColorCode(self):
        code = 1 #black   
        if self.color in self.colors.keys():
            code = self.colors[self.color]
        return code
    
"""
Just a simple container
"""
class Filter:
    types = ['contains', '!contains', 'exact', 'beginsWith']
    
    def __init__(self, type, value):
        if not type in self.types:
            raise ConfigError, 'unknown filter type'
        else:
            self.type = type
            self.value = value
            
    def getFromNode(node):
        #is valid Filter Node:
        if not node.localName == 'filter':
            raise ConfigError, 'invalid filter node'
        #self.getAttributeValue(y, 'type'), self.getAttributeValue(y, 'value'))
        type = ConfigParser.getAttributeValue(node, 'type')
        if not type in Filter.types:
            raise ConfigError, 'invalid filter type'
        value = ConfigParser.getAttributeValue(node, 'value')
        if value == '':
            raise ConfigError, 'invalid filter value'
        return Filter(type, value)
    getFromNode = staticmethod(getFromNode)
            
class Input:
    def __init__(self, folderlist):
        if not folderlist:
            raise ConfigError, 'empty input'
        else:
            self.folderlist = folderlist
            
    def getFromNode(node):
        input = None
        fold = []
        folders = ConfigParser.getChildNodes(node, 'folder')
        for x in folders:
            fold.append(Folder.getFromNode(x))
        input = Input(fold)
        return input
    getFromNode = staticmethod(getFromNode)
            
class Folder:
    def __init__(self, name, filterlist):
        self.filterlist = filterlist
        self.name = name
        
    #if filterlist has some elements
    #this value is true and false otherwise 
    def hasFilters(self):
        return self.filterlist
    def getFromNode(node):
        folder = None
        #is valid Folder Node:
        if not node.localName == 'folder':
            raise ConfigError, 'invalid folder node'
        filt = []
        filters = ConfigParser.getChildNodes(node, 'filter')
        if filters:
            for y in filters:
                filt.append(Filter.getFromNode(y))
            folder = Folder(ConfigParser.getAttributeValue(node, 'name'),filt)
        else:
            folder = Folder(ConfigParser.getAttributeValue(node, 'name'),None) 
        return folder
    getFromNode = staticmethod(getFromNode)