import xml.dom.minidom as parser
import os, sys
from ROOT import TFile
from sets import Set


"""
The ConfigParser reads a XML-configuration file and makes the content easy to access by a script
@author: lkreczko
@contact: lkreczko@mail.desy.de
@version: 0.3
"""
class ConfigParser:
    """
    Constructor for ConfigParser
    @param file: the configuration file 
    @param rootname: the name of the XML-Tree-Root (defines the config type) 
    @requires: file has to exist
    @raise IOError: if the configuration file does not exist 
    """
    def __init__(self, file, rootname= 'HistPlotter'):
        self.__rootname =  rootname
        self.__includeFiles = []#list of setups ['name'] = filepath
        self.__histDefinitions = []
        if ConfigParser.fileExists(file, 'config'):
            self.__file = file
        
    def fileExists(file, type):
        ret = os.path.exists(file)
        if(not ret):
            msg = type + ' file "' + file +' " does not exist'
            raise IOError, msg
        return ret
    fileExists = staticmethod(fileExists)
    
    """
    @return: minidom representation of the XML-configfile
    """        
    def getDocument(self):
        return parser.parse(self.__file)
    
    """
    @param nodename: the name of the nodes
    @return: a list of nodes with name = nodename 
    """
    def getNodeList(self, nodename):
        return self.getDocument().getElementsByTagName(nodename)
        #return self.getNodeList(self.getDocument(), nodename)
    
    """
    @param parentnode: minidom node
    @param name: name of the childNodes
    @return: a list of childNodes of parentNode with the name = name   
    """
    def getChildNodes(parentnode, name):
        ret = []
        for i in parentnode.childNodes:
            if i.localName == name:
                ret.append(i)
        return ret
    getChildNodes = staticmethod(getChildNodes)  
    
    """
    A wrapper for the minidom childNodes function.
    The empty childNodes are sorted out by this method
    @return: all not empty childNodes
    """
    def getAllChildNodes(parentnode):
        ret = []
        for i in parentnode.childNodes:
            if i and i.localName:
                ret.append(i)
        return ret
    getAllChildNodes = staticmethod(getAllChildNodes)
    
    def getNodeByAttribute(nodelist, attName, attValue):
        node = None
        for i in nodelist:
            if ConfigParser.hasAttribute(i, attName):
               if ConfigParser.getAttributeValue(i, attName) == attValue:
                   node = i
        return node
    getNodeByAttribute = staticmethod(getNodeByAttribute)
    
    def hasAttribute(node, attr):
        ret = False
        for i in range(node.attributes.length):
            name = node.attributes.item(i).name
            if name ==attr:
                ret = True
        return ret
    
    hasAttribute = staticmethod(hasAttribute)
    
    """
    @return: rootnode of the XML-tree
    @requires: rootnod-name == self.__rootname
    """  
    def getRoot(self):
        root = self.getDocument().documentElement
        if root.localName != self.__rootname:
            msg = 'not a "' + self.__rootname + '" configuration file'
            raise ConfigError, msg
        return root

    """
    Reads the include files section of the config.
    @requires: each include is unique
    @todo: only valid XML-Files should be included
    @return: a list of files to include
    """
    def readIncludes(self):
        includes = self.getNodeList('include')
        for i in includes:
            file = self.getAttributeValue(i, 'file')
            ConfigParser.fileExists(file, 'include')
#            if not os.path.exists(file):
#                raise ConfigError, 'include file does not exist'
            #file = self.getAttributeValue(i, 'value')
            if not file in self.__includeFiles:
                self.__includeFiles.append(file)
            else:
                raise ConfigError, 'multiple includes with same name ' + name.__str__()
        return self.__includeFiles
        
    """
    @param node: minidom node referenz
    @param name: name of the attribute
    @raise ConfigError: if the attribute with name = 'name' does not exist
    @return: value of the attribute named 'name' of the given node
    """
    def getAttributeValue(node, name):
        value = None
        for i in range(node.attributes.length):
            if name == node.attributes.item(i).name:
                value = node.attributes.item(i).value
        if value == None:
            raise ConfigError, 'node has no attribute named ' + name.__str__()
        return value
    #make the function statuc
    getAttributeValue = staticmethod(getAttributeValue)
    
    """
    Reads the <sources><file... section of the config
    @requires: all specified files have to exist
    @return: a dictionary in form of dic[fileID] = filename
    """
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
            #the file has to exist
            if ConfigParser.fileExists(value, 'root'):
                ret[key] = value
        return ret
    
    """
    @param id: the id of the file specified in the config
    @return: the filename associated to the id
    @raise InputKexError: if the ID hasn't been specified yet
    """
    def getFilenameByID(self, id):
        return self.getInputFiles()[id]
    
    def fileContainsObject(file, objectname):
        ret = False
        ConfigParser.fileExists(file, 'root')
        f = TFile(file)
        k = f.Get(objectname)
        if k:
            ret = True            
        return ret
    fileContainsObject = staticmethod(fileContainsObject)
    """
    Reads the <sources><input... section of the config
    @requires: exact one occurance of <sources>
    @raise ConfigError: if the requirement is not fullfilled
    @return: an instance of the Input class
    @see: Input.getFromNode(node)
    """
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
    
    """
    reads the <plots> section of the config
    @requires: exact one occurance of <plots>
    @raise ConfigError: if the requirement is not fullfilled
    @return: an instance of the Plots class
    @see: Plots.__init__(histlist, create, makeSum, sumfile)
    @todo: finish this
    """
    def getPlots(self):
        inputfiles = self.getInputFiles()
        inputs = self.getInputs()
        includes = self.readIncludes()
        plots = self.getNodeList('plots')
        return Plots.getFromNode(plots,includes, inputs, inputfiles)
    

"""
A simple Exception implementations for customized errors 
"""
class ConfigError(Exception):
    """
    constructor
    @param value: an error value
    """
    def __init__(self, value):
        self.value = value
        
    """
    @return: a string representation of the exception
    """
    def __str__(self):
        return repr(self.value)

###########################################################################################
######## Config Helpers  #####################################################################
##########################################################################################

"""
container for all Plots
"""
class Plots:
    #define the output as ROOT can write it
    output = ['eps','ps','pdf','jpg','png','gif']
    """
    constructor
    @param histlist: the list of Histogram instances
    @param create: what kind of file should the histograms be printed
    @param makeSummary: should a summary file (a file with all histograms) be made?
    @param sumfile: the filename of the summary file
    @requires: create-values in the allowed list    
    """
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
    
    """
    Creates a Plots instance from a minidom node
    @param node:the <plots> node
    @requires: the node to be the <plots> node 
    """
    def getFromNode(plots, includes, inputs, inputfiles):
        if len(plots) >1 or not plots:
            raise ConfigError, 'not exact one <plots> inside config file'
        create = ConfigParser.getAttributeValue(plots[0], 'create')
        makeSum = ConfigParser.getAttributeValue(plots[0], 'makeSummaryFile')
        sumfile = ConfigParser.getAttributeValue(plots[0], 'summaryFile')
        #read histsetups
        histsetups = {}
        histsetuplist = []
        for i in includes:
            setup = Histogramm.readHistSetupFromFile(i)
            histsetuplist.append(setup)
        if len(histsetuplist) > 0:
            histsetups = histsetuplist[0]
            for i in range(2,len(histsetuplist)):
                for k in histsetups.keys():
                    if i.has_key(k): 
                        print 'Warning: multible definitions of  histsetup "', k,'"'
                    else:
                         histsetups[k] = i[k]

        #read first the single histograms
        single = ConfigParser.getChildNodes(plots[0], 'hist')
        singleList =[]
        for h in single:
            hist = Histogramm.readHistSetupFromNode(h)
            #check if Variables have valid input, input exists and setup is existing
            if not hist.opt['setup'] == '':
                if hist.opt['setup'] in histsetups.keys():
                    hist.applySetup(histsetups[hist.opt['setup']].configOptions)
                else:
                    print 'Warning: HistSetup ', hist.opt['setup'], 'is not defined'
            singleList.append(hist)
        #plots can contain either single histograms or hist lists
        multilist = []
        #join the two lists        
        singleList.extend(multilist)
        joinlist = singleList
        return Plots(joinlist, create, makeSum, sumfile)        
    #make the function static
    getFromNode = staticmethod(getFromNode)
    
            

"""
The class Histogram provides access and checking of the histogram properties
"""        
class Histogramm:
    defaultXML = 'test/DefaultHistConfig.xml'
    
    def __init__(self, varlist, optionlist):
        #read defaults
        defaultlist = self.readDefaults()
        self.opt = defaultlist
        self.configOptions = optionlist
        #replaces defaults by config values, if they had been defined
        for i in optionlist.keys():
            if i in defaultlist.keys():
                self.opt[i] = optionlist[i]
            else:
                msg = 'option "' + i + '" was not defined for Histogram'
                raise ConfigError, msg
            
        if varlist:   
            self.varlist = varlist
        else:
            raise ConfigError, 'no variable defined for histogram ' + self.opt['name']
        
#        if not self.isValid():
#            msg = 'HistConfig of "'  + self.opt['name'] + '" is not valid'
#            raise ConfigError, msg
        
    def __cmp__(self, other):
        return cmp(self.name, other.name)
        
#    def setOptions(self, options):
#        for i in optionlist.keys():
#            if i in defaultlist.keys():
#                self.opt[i] = optionlist[i]
#            else:
#                msg = 'option "' + i + '" was not defined for Histogram'
#                raise ConfigError, msg
            
    def applySetup(self, options):
        print options
        for i in options.keys():
            if i in self.opt.keys():
                if not i in self.configOptions.keys():
                    self.opt[i] = options[i]
            else:
                msg = 'option "' + i + '" was not defined for Histogram'
                raise ConfigError, msg
                
    """
    Reads all histogram definitions from a file and returns a dictionary with histograms
    dic['histname'] = Histogram
    """
    def readHistSetupFromFile(file):
        varlist = [Variable({})]
        setups = {}
        cfg = ConfigParser(file, 'HistSetup')
        hists = ConfigParser.getChildNodes(cfg.getRoot(), 'hist')
        if not hists:
            print 'Warning: empty setup file'
        for i in hists:
            hist = Histogramm.readHistSetupFromNode(i, True)
            setups[hist.opt['name']] = hist
        return setups
    
    def readHistSetupFromNode(node, setup =False):
        varlist = []
        if setup:
            varlist.append(Variable({}))
        optlist = {}
        #go through all attributes
        for i in range(node.attributes.length):
            name = node.attributes.item(i).name
            value = node.attributes.item(i).value
            optlist[name] = value
#        optlist = {'name':name}
        #go through all children nodes
        for i in ConfigParser.getAllChildNodes(node):
            #don't do complex input like var and legend
            if not i.localName in ['var','legend']:
                optlist[i.localName] = ConfigParser.getAttributeValue(i, 'v')
                
        for x in ConfigParser.getChildNodes(node, 'var'):
            var = Variable.getFromNode(x)
            if var:
                varlist.append(var)
        return Histogramm(varlist, optlist)
    
    """
    @return: a dictionary of options + default values
    """
    def readDefaults(self):
        defaults = {}
        cfg = ConfigParser(self.defaultXML, 'HistSetup')
        root =ConfigParser.getNodeByAttribute(ConfigParser.getChildNodes(cfg.getRoot(), 'hist'), 'name', 'DEFAULT')
        if not root:
            raise ConfigError, 'no hist default found'
        #for all attributes
        for i in range(root.attributes.length):
            defaults[root.attributes.item(i).name] = root.attributes.item(i).value
        options = ConfigParser.getAllChildNodes(root)
        #for all options
        for i in options:
            defaults[i.localName] = ConfigParser.getAttributeValue(i, 'v')
        #for all children attributes named 'v'
        return defaults
        #in the defaults file all attributes are defined
    
    def isValid(self):
        #define needed values
        valid = self.checkInput()
        return valid
    
    def checkInput(self):
        return False
    
    readHistSetupFromFile = staticmethod(readHistSetupFromFile)
    readHistSetupFromNode = staticmethod(readHistSetupFromNode)
        
class Variable:
    defaultXML = 'test/DefaultVarConfig.xml'
    #use maybe RGB?
    colors ={'black' : 1,
             'red' : 2}
    operations = ['add', 'divide', 'substract', 'none']
    histTypes = {'line':0}
    """
    Contructor
    missing fillstyles
    @param histType: the type of the histogram style: marker, line or otherwise
    @param style: the style of the type: line-style, marker-style etc.
    @param color: the color of the input
    @param operation: defines the construction of the variables  
    """
    def __init__(self, optionlist):
         #read defaults
        defaultlist = self.readDefaults()
        self.opt = defaultlist
        #replaces defaults by config values, if they had been defined
        for i in optionlist.keys():
            if i in defaultlist.keys():
                self.opt[i] = optionlist[i]
            else:
                msg = 'option "' + i + '" was not defined for Variable'
                raise ConfigError, msg
#        if color:
#            self.color = color
#        else:
#            self.color = 'black'
#        if operation in self.operations:
#            self.operation = operation
#        else:
#            msg = 'unknown operation "' + operation + '"'
#            raise ConfigError, msg
#        if style:
#            self.style = style
#        else: 
#            self.style = '0'
#        if type:
#            self.histType = histType
#        else:
#            self.histType = 'line'
        
        #print 'Im a var!'
        
        
    def getColorCode(self):
        code = 1 #black   
        if self.color in self.colors.keys():
            code = self.colors[self.color]
        return code
    
    def getFromNode(node):
        optlist = {}
        for i in range(node.attributes.length):
            name = node.attributes.item(i).name
            value = node.attributes.item(i).value
            optlist[name] = value
        return Variable(optlist)
    getFromNode = staticmethod(getFromNode)
    
    def readDefaults(self):
        defaults = {}
        cfg = ConfigParser(self.defaultXML, 'VarSetup')
        root =ConfigParser.getNodeByAttribute(ConfigParser.getChildNodes(cfg.getRoot(), 'var'), 'name', 'DEFAULT')
        if not root:
            raise ConfigError, 'no var default found'
        #for all attributes
        for i in range(root.attributes.length):
            defaults[root.attributes.item(i).name] = root.attributes.item(i).value
        options = ConfigParser.getAllChildNodes(root)
        #for all options
        for i in options:
            defaults[i.localName] = ConfigParser.getAttributeValue(i, 'v')
        #for all children attributes named 'v'
        return defaults
        #in the defaults file all attributes are defined
    
"""
Just a simple container
"""
class Filter:
    #the allowed types of a filter
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
