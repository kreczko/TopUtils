import xml.dom.minidom as parser
import os, sys
from ROOT import TFile
from sets import Set
import copy
#from types import type

# xmlTag => Python class
classmap = {}
classmap["hist"] = "Histogram"
classmap["input"] = "Input"
#classmap["legend"] = "Legend"
classmap["filter"] = "Filter"
classmap["folder"] = "Folder"
classmap["plots"] = "Plots"
classmap["histlist"] = "HistogramList"
classmap["var"] = "Variable"
##default configurations for various ConfigObjects
defaults = {"plots" : "test/DefaultPlotConfig.xml",
                    "hist" : "test/DefaultHistConfig.xml", 
                    "legend" : "test/DefaultLegConfig.xml",
                    "var" : "test/DefaultVarConfig.xml"
                }

class Configuration():
    
    """ \brief The definitions of the configuration"""
    def __init__(self, configRoot="Plotter", verbose=False):
        self.verbose = verbose
        self.Print("Begin configuration")
        self.Print("Creating configuration with '%s' as document root" % configRoot)
        if not configRoot == "":
            self.root = configRoot
        else:
            raise ConfigError, "Configuration cannot have an empty rootnode"
        self.files = {}
        self.inputs = {}
        self.plots = None
        
#===============================================================================
#    loads all the data from the XML configuration file
#===============================================================================
    def loadConfiguration(self, xmlfile):
        """ loads all the data from the XML configuration file
        \param xmlfile the XML configuration file
        """
        if not os.path.exists(xmlfile):
            raise IOError, "Configuration file '%s' does not exist" %xmlfile
        self.Print("Loading configuration file %s" % xmlfile)
        rootnode = Parser.getDocumentRoot(xmlfile)
        if self.root == rootnode.localName:#right configuration file
            #get includes, sources, plots
            sources = Parser.getChildNodes(rootnode, "sources")[0] #only the first <sources> is read
            files = Parser.getChildNodes(sources, "file")
            inputs = Parser.getChildNodes(sources, "input")
            plots = Parser.getChildNodes(rootnode, "plots")[0]#only the first <plots> is read
            includes = Parser.getChildNodes(rootnode, "includes")
            self.readFiles(files)
            self.readInputs(inputs)
            self.readPlots(plots)
            self.resolveInputs()
        else:
            raise ConfigError, "Error: the configuration file is not valid for the given config type"
        
    def resolveInputs(self):
        for hist in self.getPlots().subobjects["hist"]:
            hist.resolveInputs(self.getInputFiles(), self.getInputs())
        for list in self.getPlots().subobjects["histlist"]:
            list.resolveInputs(self.getInputFiles(), self.getInputs())
            
        
    def readPlots(self, node):
        plots = Plots()
#        plots.readDefaults(self.defaults["plots"])
        plots.parseFromNode(node)
        self.plots = plots
    
    def getPlots(self):
        return self.plots
    
    def readFiles(self, nodelist):
        self.Print("Reading files...")
        for i in nodelist:
            file = ConfigObject(rootNodeName = "file")
            file.parseFromNode(i)
            id = file.getOption("id")
            name = file.getOption("name")
            if self.files.has_key(id):
                raise ConfigError, "multiple entries for file id %s" %id
            else:
                self.Print("Adding file '%s' with id='%s'" %(name, id))
                self.files[id] = name
                
    def readInputs(self, nodelist):
        for i in nodelist:
            input = Input()
            input.parseFromNode(i)
            name = input.getOption("name")
            if self.inputs.has_key(name):
                raise ConfigError, "multiple entries for file id %s" %name
            else:
                self.Print("Adding input '%s'" %name)
                self.inputs[name] = input
                
    def getInputByName(self, name):
        return self.getInputs()[name]
    
    def getInputs(self):
        return self.inputs
    
#===============================================================================
#    prints a message if the verbose option was used
#===============================================================================
    def Print(self, msg):
        """
        prints a message if the verbose option was used
        @param msg: the message to be printed 
        """
        if self.verbose:
            print "======== " + msg + " ========="
            
#===============================================================================
#    @param id: the id of the file specified in the config
#    @return: the filename associated to the id
#    @raise InputKeyError: if the ID hasn't been specified yet
#===============================================================================
    def getFilenameByID(self, id):
        """
         @param id: the id of the file specified in the config
         @raise \n raises InputKeyError if the ID hasn't been specified yet
        @return: the filename associated to the id
        """
        return self.getInputFiles()[id]
    
    def getInputFiles(self):
        return self.files

#===============================================================================
# stores the options of an ConfigObject and their defaults        
#===============================================================================
class OptionSet():
    """
    \brief stores the options of an ConfigObject and their defaults        
    """
        
    def __init__(self):
            self.options = {}
            self.defaults = {}
            pass
        
#===============================================================================
#        returns the option with given name, if it exists. Otherwise an exception is raised.
#===============================================================================
    def getOption(self, option):
        """
         @return the option with given name, if it exists. Otherwise raises ConfigError.
         """
        if self.hasOption(option):
            return self.options[option]
        else:
            raise ConfigError, "Option is not known."

#===============================================================================
#            adds an option to the OptionSet
#===============================================================================
    def addOption(self, option, value):
            """
            adds an option to the OptionSet
            """
            if not self.hasOption(option):
                self.defaults[option] = value
                self.options = self.defaults
            else:
                self.setOption(option, value)
#                raise ConfigError, "multiple definition of " + option
                
            
#===============================================================================
#        Sets the default values
#===============================================================================
    def setDefaults(self, options):
            """
            Sets the default values
            @deprecated: will be deleted
            """
            for i in options.keys():
                self.addOption(i, options[i])
            
#===============================================================================
#        Sets the options
#===============================================================================
    def setOptions(self, options):
            """
            Sets the options
            """
            for i in options.keys():
                self.addOption(i, options[i])
                
                
#===============================================================================
#        sets an option to a value
#        The option has to be defined in the default configuration
#        If the option is reset a second time in one ConfigObject a warning is given.
#        If the optionvalue is the same as the default value a warning is given
#        @raise ConfigError if option is unknown
#===============================================================================        
    def setOption(self, option, value):
            """
            sets an option to a value
            The option has to be defined in the default configuration
            If the option is reset a second time in one ConfigObject a warning is given.
            If the optionvalue is the same as the default value a warning is given
            @param option: the option to change
            @raise \n raises ConfigError if option is unknown
            @param value: the value of the option  
            """
            if self.hasOption(option):
                if self.defaults[option] == value:
                    print "Warning: given value for '%s' is default value." % option
                self.options[option] = value
                if not self.options[option] == self.defaults[option]:
                    print "Warning: multiple definition of '%s'. Overwriting last value" % option
            else:
                raise ConfigError, "Error: option %s is not known" % option

#===============================================================================
#        check if the option value is allowed.
#        In simplest case check if string, integer or float, see typedef            
#===============================================================================
    def checkOptionValue(self, option, type):
        """
        check if the option value is allowed.
        In simplest case check if string, integer or float, see typedef   
        """
        pass
        
    def hasOption(self, option):
        """
        @return: if option is in the set
        """
        return self.options.has_key(option)
            
            
class ConfigObject():
    """An simple configurable object"""
    
    def __init__(self, rootNodeName="", doNotParse=[], mandatoryObjects=[]):
        """
        The standard constructor
        @param rootNodeName the name of the initial XML node for parsing. DEPRACATED
        @param doNotParse list of complex objects which should not be parsed as options
        @param mandatoryObjects complex objects which have te exists. NOT IN USE
        """
        if not rootNodeName == "":
            self.rootName = rootNodeName
        else:
            raise ConfigError, "ConfigObject cannot have an empty rootnode!"
        self.doNotParse = doNotParse
        self.options = OptionSet()
        self.mandatoryObjects = mandatoryObjects
        self.subobjects = {}
        for i in self.doNotParse:
            self.subobjects[i] = []
    
    def setOption(self, option, value):
        self.options.setOption(option, value)
    def getOption(self, name):
        """
        @param name the name of the option
        @raise \n raises ConfigError if the option name is empty
        @raise \n raises KeyError if the option does not exist   
        @return value of the option with the name = name
        """
        if not name:
            raise ConfigError, "empty option name"
        else:
            return self.options.getOption(name)
        
    def readDefaults(self, configFile):
        defaults = Parser.getDocumentRoot(configFile)
        self.parseFromNode(defaults)

    def parseFromNode(self, node):
        """
        gets all attributes of the node and textnodes which are not in "doNotParse" and and adds them as options
        """
        if not node:
            raise ConfigError, "Cannot parse an empty node"
        root = node.localName 
        if defaults.has_key(root):
            self.readDefaults(defaults[root])
        atts = Parser.getListOfAttributes(node)
#        children = copy.deepcopy(Parser.getAllChildNodes(copy.deepcopy(node)))
        children = Parser.getAllChildNodes(node)
        for i in atts:
            self.options.addOption(i, atts[i])
        for i in children:
            if i.localName not in self.doNotParse:
                if Parser.hasAttribute(i, "v"):#get value from <tag v="value" />
                    self.options.addOption(i.localName, Parser.getAttributeValue(i, "v"))
                else:#get value from <tag>value</tag>
                    self.options.addOption(i.localName, Parser.getText(i))
            else:
                obj = None
                if classmap.has_key(i.localName):
                    str = classmap[i.localName]
                    src = str + "()"
                    obj = eval(src)
#                    obj.parseFromNode(copy.deepcopy(i))
                else:
                    obj = ConfigObject(rootNodeName = i.localName)
                obj.parseFromNode(i)
                self.subobjects[i.localName].append(obj)
                
    
    

class ConfigError(Exception):
    """
A simple Exception implementations for customized errors 
"""
  
    def __init__(self, value):
        """ constructor   
          @param value: an error value"""
        self.value = value
        
#===============================================================================
#    @return: a string representation of the exception
#===============================================================================
    def __str__(self):
        return repr(self.value)
    
#only static functions
class Parser():
    
#===============================================================================
#    A wrapper for the minidom childNodes function.
#    The empty childNodes are sorted out by this method
#    @return: all not empty childNodes
#===============================================================================
    def getAllChildNodes(parentnode):
        ret = []
        if not parentnode:
            raise ConfigError, "parent node is empty"
        for i in parentnode.childNodes:
            if i and i.localName:
#                ret.append(copy.deepcopy(i))
                ret.append(i)
        return ret
    getAllChildNodes = staticmethod(getAllChildNodes)
#===============================================================================
#    Removes the empty nodes and returns only nodes matching to the name
#    @param parentnode: minidom node
#    @param name: name of the childNodes
#    @return: a list of childNodes of parentNode with the name = name   
#===============================================================================
    def getChildNodes(parentnode, name):
        ret = []
        for i in parentnode.childNodes:
            if i.localName == name:
                ret.append(i)
        return ret
    getChildNodes = staticmethod(getChildNodes)  
#===============================================================================
#    @return a list of nodes from a XML file
#===============================================================================
    def getNodeList(xmlFile, nodename):
#        return copy.deepcopy(Parser.getDocument(xmlFile).getElementsByTagName(nodename))
        return Parser.getDocument(xmlFile).getElementsByTagName(nodename)
    getNodeList = staticmethod(getNodeList)
    
    def getDocument(xmlfile):
        if FileService.exists(xmlfile):
            return parser.parse(xmlfile)
    getDocument = staticmethod(getDocument)
    
    def getText(node):
        ret = ""
        for i in node.childNodes:
            if i.nodeType == i.TEXT_NODE:
                ret += i.data
        return ret
    getText = staticmethod(getText)
    
#===============================================================================
#    @return: rootnode of the XML-tree
#    @requires: rootnod-name == self.__rootname
#===============================================================================
    def getDocumentRoot(xmlfile):
        ret = Parser.getDocument(xmlfile).documentElement
        if not ret == "":
            return ret
        else:
            msg = "Error: file %s does not have a root node." % xmlfile
            raise ConfigError, msg
    getDocumentRoot = staticmethod(getDocumentRoot)
    
    def getListOfAttributes(node):
        if not node:
            raise ConfigError, "empty node"
        list = {}
        for i in range(node.attributes.length):
            name = node.attributes.item(i).name
            value = node.attributes.item(i).value
#===============================================================================
#            multiple attributes are already covered by the minidom parser, raising an excpetion
#===============================================================================
            if name in list.keys():
                print "Warning: multiple definitions of %s" %name
            list[name] = value
        return list
    getListOfAttributes = staticmethod(getListOfAttributes)
    
#===============================================================================
#    @param node: the minidom-node
#    @param attr: attribute name  
#    @return: if the node has an attribute with the specified name
#===============================================================================
    def hasAttribute(node, attr):
        ret = False
        for i in range(node.attributes.length):
            name = node.attributes.item(i).name
            if name == attr:
                ret = True
        return ret
    hasAttribute = staticmethod(hasAttribute)
    
#===============================================================================
#    @param node: minidom node referenz
#    @param name: name of the attribute
#    @raise ConfigError: if the attribute with name = 'name' does not exist
#    @return: value of the attribute named 'name' of the given node
#===============================================================================
    def getAttributeValue(node, name):
        value = None
        for i in range(node.attributes.length):
            if name == node.attributes.item(i).name:
                value = node.attributes.item(i).value
        if value == None:
            raise ConfigError, 'node has no attribute named ' + name.__str__()
        return value
    getAttributeValue = staticmethod(getAttributeValue)
#===============================================================================
# A wrapper for any file or TFile related issues
#===============================================================================
class FileService():
#===============================================================================
#    @return if a file exists. If it doesn't an IOError is raised
#===============================================================================
    def exists(filename):
        ret = os.path.exists(filename)
        if not ret:
            msg = "Error: file %s does not exist." % filename
            raise IOError, msg
        return ret
    exists = staticmethod(exists)
    
#===============================================================================
#    Checks if a given root file contains an key/object
#    @param file: filename
#    @param objectname: objectname
#    @return: if files contains object 
#===============================================================================
    def fileContainsObject(file, objectname):
        """
        Checks if a given root file contains an key/object
        \param file: filename
        \param objectname: objectname
        \return: if files contains object 
        """
        ret = False
        FileService.exists(file)
        f = TFile(file)
        k = f.Get(objectname)
        if k:
            ret = True            
        f.Close()
        return ret
    fileContainsObject = staticmethod(fileContainsObject)
    
    def getFileContent(file):
        FileService.exists(file)
        ret = []
        file = TFile(file)
        tlist = file.GetListOfKeys().__iter__()
        for i in tlist:
            for x in FileService.followDirs(file, i.GetName()):
                ret.append(x)
        return ret
    getFileContent = staticmethod(getFileContent)
    
    def followDirs(file, dir):
        obj = file.Get(dir)
        list = None
        ret = []
        if "TDirectory" in obj.__str__():
            list = file.Get(dir).GetListOfKeys()
        if list:
            for i in list.__iter__():
                ret.append(FileService.followDirs(file, dir+"/"+ i.GetName()))
            return ret
        else:
            return dir
            
    followDirs = staticmethod(followDirs)
#        while(tlist.pop()):
#            print tlist.pop()
#        for i in  TFile(file).GetListOfKeys():
#            print i
    
    
class Plots(ConfigObject):
    rootNodeName = "plots"
    doNotParse = ["hist", "histlist"]
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
  
class Histogram(ConfigObject):
    rootNodeName = "hist"
    doNotParse = ["var", "legend", "statbox"]      
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
        self.variables = {}
        self.legend = None
    
    def getVariable(self, name):
        ret = None
        for i in self.subobjects["var"]:
            if i.getOption("name") == name:
                ret = i
#        if name in self.variables.keys():
#            return self.variables[name]
        return ret
        
    def resolveInputs(self, files, inputs):
        histinput = self.getOption("input")
        for var in self.subobjects["var"]:
            varinput = var.getOption("input")
            varsource = self.getVarSourceFile(var)
            if isinstance(varsource, unicode):
                var.setOption("source", [files[varsource], self.getVarInput(var)])
            else:
                vars = []
                for i in varsource:
                    vars.append(self.getVariable(i))
                var.setOption("source", vars)
    
                    
    def getVarInput(self, var):
        histinput = self.getOption("input")
        varinput = var.getOption("input")
        ret = None
        if not Input.separator in varinput and not Input.separator in histinput:
            if varinput:
                ret = varinput
            else:
                ret = histinput
        elif "f"+Input.separator in varinput and histinput:
            varinput = varinput.replace("f"+Input.separator, "")
            if varinput.endswith("/"):
                ret = varinput + histinput
            else:
                ret = varinput + "/" + histinput
        elif "f"+Input.separator in histinput and varinput:
            histinput = histinput.replace("f"+Input.separator, "")
            if histinput.endswith("/"):
                ret = histinput + varinput
            else:
                ret = histinput+ "/" + varinput
        return ret
                
            
    def getVarSourceFile(self,var):
        ret = None
        source = var.getOption('source')
        op = var.getOption('operation')
        if Variable.ksourceFile in source and op.lower() == "none":
            source = source.split(Variable.typeDelimiter)
            ret =  source[1]
        elif Variable.ksourceVar in source and not op.lower() == "none":
            vars = source.split(Variable.entryDelimiter)
            ret = []
            for i in vars:
                v = i.replace(Variable.ksourceVar+Variable.typeDelimiter,"")
                ret.append(v.strip())
        return ret
    
#    def getLegend(self):
#        return self.legend
    
        
class HistogramList(ConfigObject):
    rootNodeName = "hist"
    doNotParse = ["var", "legend", "statbox"]      
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
        #TODO: only vars with file input!
        #additional parameter "for"

    def resolveInputs(self, files, inputs):
        for var in self.subobjects["var"]:
            varsource = self.getVarSourceFile(var)
            if isinstance(varsource, unicode):
                file = files[varsource]
                input = self.getVarInput(var, inputs)
                filecontent = FileService.getFileContent(file)
                input.setContent(filecontent)
                print input.getFilteredContent()
#                var.setOption("source", [files[varsource], self.getVarInput(var)])
            else:
                vars = []
                for i in varsource:
                    print i
                    vars.append(self.getVariable(i))
                print vars
                var.setOption("source", vars)
                
    def getVarSourceFile(self,var):
        ret = None
        source = var.getOption('source')
        op = var.getOption('operation')
        if Variable.ksourceFile in source and op.lower() == "none":
            source = source.split(Variable.typeDelimiter)
            ret =  source[1]
        elif Variable.ksourceVar in source and not op.lower() == "none":
            vars = source.split(Variable.entryDelimiter)
            ret = []
            for i in vars:
                v = i.replace(Variable.ksourceVar+Variable.typeDelimiter,"")
                ret.append(v.strip())
        return ret
    
    def getVarInput(self, var, inputs):
        histinput = self.getOption("input")
        varinput = var.getOption("input")
        ret = None
        if "i" + Input.separator in histinput:
            ret = histinput.replace("i"+Input.separator, "")
        elif "i" + Input.separator in varinput:
            ret = varinput.replace("i"+Input.separator, "")
        if inputs.has_key(ret):
            ret = inputs[ret]
        else:
            raise ConfigError, "Used input '%s' is not defined." % ret
            
        return ret
        
class Variable(ConfigObject):
    ksourceFile = 'file'
    ksourceVar = 'var'
    typeDelimiter = ':'
    entryDelimiter = ','
    
    def __init__(self):
        ConfigObject.__init__(self, "var", [])
        
class Filter(ConfigObject):
    rootNodeName = "filter"
    doNotParse = []
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
    
    def applyFilter(self, string):
        type =  self.getOption("type")
        value = self.getOption("value")
        if type == "contains":
            return self.applyContainsFilter(string, value, True)
        elif type == "!contains":
            return self.applyContainsFilter(string, value, False)
        elif type == "exact":
            return self.applyExactFilter(string, value)
        else:
            return string
        
    def applyExactFilter(self, string, value):
        if string == value:
            return string
        else:
            return ""
    
    def applyContainsFilter(self, string, value, positiv):
        if value in string and positiv:
            return string
        elif not value in string and not positiv:
            return string
        else:
            return ""
        
class Folder(ConfigObject):
    rootNodeName = "folder"
    doNotParse = ["filter"]
    pathtoken = "/"
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
        self.content = []
        self.filtered = []

    def getContent(self):
        return self.content
    
    def getFilteredContent(self):
        folder = self.getOption("name")
        for i in self.content:
            if folder + self.pathtoken in i:
                res = i.replace(folder + self.pathtoken, "")
                for x in self.subobjects["filter"]:
                    res = x.applyFilter(res)
                if res:
                    self.filtered.append(folder + self.pathtoken + res)
        return self.filtered
    
    def setContent(self, list):
        if list:
            self.content = list
    
#===============================================================================
# defines an input for a histogram
# @comment: does not know anything about files
#===============================================================================
class Input(ConfigObject):
    """defines an input for a histogram
    \comment: does not know anything about files
    """
    ##separator between shortcut and input
    separator = ":"
    rootNodeName = "input"
    ## marks complex objects which have their own class.
    doNotParse = ["folder"]
    ##    defines shortcuts for inputs \n
    ##   f:foldername uses a folder as input\n
    ##    i:inputName uses an input previously defined in the configuration file:\n
    ##    if it is no shortcut is present the input will be interpreted as direct input (full path to histogram)
    inputTypes = {"f":"folder", "i":"input"}
    def __init__(self):
        self.content = []
        #self.inputTypes = {"f"+ Input.separator:"folder", "i":"input"}
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
        
    def setContent(self, list):
        for dir in self.subobjects["folder"]:
            l = []
            for hist in list:
                if dir.getOption("name") in hist:
                    l.append(hist)
            if l:
                dir.setContent(l) 
                
    def getFilteredContent(self):
        content = []
        for dir in self.subobjects["folder"]:
            content.extend(dir.getFilteredContent())
        return content