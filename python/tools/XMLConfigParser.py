import xml.dom.minidom as parser
import os, sys
from ROOT import TFile
from sets import Set
#from types import type

# xmlTag => Python class
classmap = {}
classmap["canv"] = "Histogram"
classmap["input"] = "Input"
#classmap["legend"] = "Legend"
classmap["filter"] = "Filter"
classmap["folder"] = "Folder"
classmap["plots"] = "Plots"
classmap["canvlist"] = "HistogramList"
classmap["hist"] = "Variable"
##default configurations for various ConfigObjects
pathToDir = "TopAnalysis/TopUtils/python/tools/"
defaults = {"plots"  : "TopAnalysis/TopUtils/data/defaults/PlotConfig.xml",
            "canv"   : "TopAnalysis/TopUtils/data/defaults/CanvasConfig.xml",
            "legend" : "TopAnalysis/TopUtils/data/defaults/LegendConfig.xml",
            "hist"    : "TopAnalysis/TopUtils/data/defaults/HistogramConfig.xml"
                }

## 1= Info, 2 = Warning, 3 = Error. outputlevel = 2 will only display warnings and errors
outputlevel = 2
verbose = False
#===============================================================================
#    prints a message if the verbose option was used
#===============================================================================
def Print(msg, lvl=1):
        """
        prints a message if the verbose option was used
        @param msg: the message to be printed 
        """
        if verbose and outputlevel <= lvl:
            print "======== " + msg + " ========="
            
class Configuration:
    
    """ \brief The definitions of the configuration"""
    def __init__(self, configRoot="Plotter"):
        Print("Begin configuration")
        Print("Creating configuration with '%s' as document root" % configRoot)
        if not configRoot == "":
            self.root = configRoot
        else:
            raise ConfigError, "Configuration cannot have an empty rootnode"
        self.files = {}
        self.inputs = {}
        self.includes = []
        self.plots = None
        self.setups = {}
        
#===============================================================================
#    loads all the data from the XML configuration file
#===============================================================================
    def loadConfiguration(self, xmlfile):
        """ loads all the data from the XML configuration file
        \param xmlfile the XML configuration file
        """
        xmlfile = pathToDir + xmlfile
        if not os.path.exists(xmlfile):
            raise IOError, "Configuration file '%s' does not exist" %xmlfile
        Print("Loading configuration file %s" % xmlfile)
        rootnode = Parser.getDocumentRoot(xmlfile)
        if self.root == rootnode.localName:#right configuration file
            #get includes, sources, plots
            sources = Parser.getChildNodes(rootnode, "sources")[0] #only the first <sources> is read
            files = Parser.getChildNodes(sources, "file")
            inputs = Parser.getChildNodes(sources, "input")
            plots = Parser.getChildNodes(rootnode, "plots")[0]#only the first <plots> is read
            includes = Parser.getChildNodes(rootnode, "include")
            self.readFiles(files)
            self.readInputs(inputs)
            self.readPlots(plots)
            self.readIncludes(includes)
            self.applySetups()
            self.resolveInputs()
        else:
            raise ConfigError, "Error: the configuration file is not valid for the given config type"
    
    def applySetups(self):
        for i in self.getAllCanvas():
            i = self.applySetup(i)
            
    def applySetup(self, canv):
        setup = canv.getOption("setup")
        if self.setups.has_key(setup):
             canv.Merge(self.setups[setup])
        return canv
            
    def mergeSetups(self):
        for s in self.includes:
            setups = s.getListOfSetups()
            #alternative: use self.setups.update()
            for i in setups.keys():
                if self.setups.has_key(i):
                    Print("Warning: Setup %s is defined more than one time." %i, lvl = 2)
                self.setups[i] = setups[i]
                
    def resolveInputs(self):
        for hist in self.getPlots().subobjects["canv"]:
            hist.resolveInputs(self.getInputFiles(), self.getInputs())
        for list in self.getPlots().subobjects["canvlist"]:
            list.resolveInputs(self.getInputFiles(), self.getInputs())
            
    def readPlots(self, node):
        self.plots = Plots()
        self.plots.parseFromNode(node)
    
    def getPlots(self):
        return self.plots
    
    def getAllCanvas(self):
        histlist = self.plots.subobjects["canv"]
        for list in self.plots.subobjects["canvlist"]:
            histlist.extend(list.subobjects["canv"])
        return histlist
    
    def readFiles(self, nodelist):
        Print("Reading files...")
        for i in nodelist:
            file = ConfigObject(rootNodeName = "file")
            file.parseFromNode(i)
            id = file.getOption("id")
            name = file.getOption("name")
            if not name.startswith("/"):
                name = pathToDir + name
                name = os.path.abspath(name)
            if self.files.has_key(id):
                raise ConfigError, "multiple entries for file id %s" %id
            else:
                Print("Adding file '%s' with id='%s'" %(name, id))
                self.files[id] = name
                
    def readInputs(self, nodelist):
        for i in nodelist:
            input = Input()
            input.parseFromNode(i)
            name = input.getOption("name")
            if self.inputs.has_key(name):
                raise ConfigError, "multiple entries for file id %s" %name
            else:
                Print("Adding input '%s'" %name)
                self.inputs[name] = input

    def readIncludes(self, nodelist):
        for node in nodelist:
            include = Include()
            include.parseFromNode(node)
            self.includes.append(include)
            Print("Including setup file %s" %include.getOption("file"))
        self.mergeSetups()
    
#    def readSetups(self):
#        if self.includes:
#            for inc in self.includes:
#                Print("Including setup file %s" %inc)
#                self.setups.update(inc.getListOfSetups())
            
    def getInputByName(self, name):
        return self.getInputs()[name]
    
    def getInputs(self):
        return self.inputs
    
            
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
class OptionSet:
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
            #raise ConfigError, "Option '%s' is not known."%option
            return ""

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
    
    def getOptions(self):
        return self.options
    
    def Merge(self, opt):
        if opt:
            opts = opt.getOptions()
            for i in opts.keys():
                if not self.options.has_key(i): #if the option allready exists; no overwriting!
                    self.addOption(i, opts[i])
                elif self.options[i] == "" and not self.defaults[i] == opts[i]:
                    self.setOption(i, opts[i])
                
        
    def Copy(self):
        newset = OptionSet()
        newop = {}
        for i in self.options.keys():
            newkey = "%s" %i
            newitem = "%s" %self.options[i]
            newop[newkey] = newitem
        newset.setOptions(newop)
        return newset
                
                
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
                      Print( "Warning: given value for '%s' is default value (%s)." % (option, value), 2)
                self.options[option] = value
                if not self.options[option] == self.defaults[option]:
                   Print("Warning: multiple definition of '%s'. Overwriting last value" % option, 2)
            else:
                #raise ConfigError, "Error: option %s is not known" % option
                self.addOption(option, value)

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
            
            
class ConfigObject:
    """An simple configurable object"""
    
    def __init__(self, rootNodeName="", doNotParse=[], mandatoryObjects=[], mandatoryOptions=[]):
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
        self.mandatoryOptions = mandatoryOptions
        self.subobjects = {}
        for i in self.doNotParse:
            self.subobjects[i] = []
        for i in self.mandatoryObjects:
            self.subobjects[i] = []
    
    def setOptions(self, optionset):
        self.options.setOptions(optionset.getOptions())
    
    def setOption(self, option, value):
        self.options.setOption(option, value)
        
    def getOptions(self):
        return self.options
    def hasOption(self, option):
        return self.options.hasOption(option)

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
            self.readDefaults(pathToDir + defaults[root])
        atts = Parser.getListOfAttributes(node)
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
                else:
                    obj = ConfigObject(rootNodeName = i.localName)
                obj.parseFromNode(i)
                self.subobjects[i.localName].append(obj)
            
            for opt in self.mandatoryOptions:
                if not self.hasOption(opt):
                    self.options.addOption(opt, "")
            for obj in self.mandatoryObjects:
                if not self.subobjects[obj]:
                    raise ConfigError, "Mandatory object '%s' not found" % obj
                
    def Copy(self):
        obj = ConfigObject(self.rootName)
        obj.options = self.options.Copy()
        return obj
    
    def Merge(self, obj):
        self.options.Merge(obj.getOptions())
                
    
    

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
class Parser:
    
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
        return Parser.getDocument(pathToDir + xmlFile).getElementsByTagName(nodename)
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
                Print("Warning: multiple definitions of %s" %name, 2)
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
class FileService:
#===============================================================================
#    @return if a file exists. If it doesn't an IOError is raised
#===============================================================================
    def exists(filename):
        ret = os.path.exists(filename)
        if not ret:
            raise IOError, "Error: file %s does not exist." % filename
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
    
    
class Plots(ConfigObject):
    rootNodeName = "plots"
    doNotParse = ["canv", "canvlist"]
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
  
class Histogram(ConfigObject):
    rootNodeName = "canv"
    doNotParse = ["hist", "legend", "statbox"]      
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse)
        self.variables = {}
        self.legend = None
    
    def getVariable(self, name):
        ret = None
        for i in self.subobjects["hist"]:
            if i.getOption("name") == name:
                ret = i
        return ret
        
    def resolveInputs(self, files, inputs):
#        histinput = self.getOption("input")
        for var in self.subobjects["hist"]:
            varinput = var.getOption("source")
            varsource = self.getVarSourceFile(var)
            if isinstance(varsource, unicode) and varsource:
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
#        elif histinput and not varinput:
#            ret = histinput
#        elif varinput and not histinput
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
    rootNodeName = "canvlist"
    doNotParse = ["hist", "legend"]
    mandatoryOptions = ["name", "input", "savefolder"]      
    mandatoryObjects = ["hist"]
    ALL = "ALL" #keyword for the "for" attribute
    
    def __init__(self):
        ConfigObject.__init__(self, self.rootNodeName, self.doNotParse, 
                              mandatoryObjects = self.mandatoryObjects, 
                              mandatoryOptions= self.mandatoryOptions)
        self.subobjects["canv"] = []
        self.attributelist = {}
        Print("Creating HistogramList Object")
        #TODO: only vars with file input!
        #additional parameter "for"
        
    def parseFromNode(self, node):
        """
        gets all attributes of the node and textnodes which are not in "doNotParse" and and adds them as options
        """
        Print("Executing special parseFromNode in HistogramList")
        if not node:
            raise ConfigError, "Cannot parse an empty node"
        root = node.localName 
        if defaults.has_key(root):#if default configuration ist defined
            self.readDefaults(pathToDir + defaults[root])
        atts = Parser.getListOfAttributes(node)
        children = Parser.getAllChildNodes(node)
        for i in atts:
            self.options.addOption(i, atts[i])
        for i in children:
            if i.localName not in self.doNotParse:#attribute
                fatt = HistogramList.ALL
                if Parser.hasAttribute(i, "for"):
                    fatt = Parser.getAttributeValue(i, "for")
                if not self.attributelist.has_key(fatt):
                    self.attributelist[fatt] = OptionSet()
                if Parser.hasAttribute(i, "v"):#get value from <tag v="value" />
                    self.attributelist[fatt].addOption(i.localName, Parser.getAttributeValue(i, "v"))
                else:#get value from <tag>value</tag>
                    self.attributelist[fatt].addOption(i.localName, Parser.getText(i))
            else:
                obj = None
                if classmap.has_key(i.localName):
                    str = classmap[i.localName]
                    src = str + "()"
                    obj = eval(src)
                else:
                    obj = ConfigObject(rootNodeName = i.localName)
                obj.parseFromNode(i)
                self.subobjects[i.localName].append(obj)
            for opt in self.mandatoryOptions:
                if not self.hasOption(opt):
                    Print("mandatory option '%s' not found, creating empty option " % opt)
                    self.options.addOption(opt, "")
            for obj in self.mandatoryObjects:
                if not self.subobjects[obj]:
                    raise ConfigError, "Mandatory object '%s' not found" % obj
    
    def getOptionFor(self, option, fatt):
        ret = ""
        if self.attributelist.has_key(fatt) and self.attributelist[fatt].hasOption(option):
            ret = self.attributelist[fatt].getOption(option)
        elif self.attributelist.has_key(HistogramList.ALL) and self.attributelist[HistogramList.ALL].hasOption(option):
            ret = self.attributelist[HistogramList.ALL].getOption(option)
        return ret
    
    def getOptionSetFor(self, name):
        ret = OptionSet()
        allOptions = None
        ownOptions = None
        if self.attributelist.has_key(HistogramList.ALL):
            allOptions = self.attributelist[HistogramList.ALL].Copy()
        if self.attributelist.has_key(name):
            ownOptions = self.attributelist[name].Copy()
        #own options have priority over all options
        ret.Merge(ownOptions)
        ret.Merge(allOptions)
        return ret
        
        
    def resolveInputs(self, files, inputs):
        Print("Resolving HistogramList inputs")
        input = None
#        if self.hasOption("input") and "i%s"%Input.separator in self.getOption("input"):
#            input = self.getOption("input").replace("i" + Input.separator, "")
        var0 = self.subobjects["hist"][0]
        input = self.getVarInput(var0, inputs, self.getOption("input"))
        if not input:
            input = Input.createInput()
#        elif inputs.has_key(input):
#            input = inputs[input]
#        else:
#            raise ConfigError, "Input '%s' could not be found"%input
       
        varsource = self.getVarSourceFile(var0)
        content = FileService.getFileContent(files[varsource])
        input.setContent(content)
        content = input.getFilteredContent()
        for i in range(0, len(content)):
            hist = Histogram()
            hist.readDefaults(pathToDir + defaults["canv"])#read only once and use copy() then!
#            hist.setOption("input", i)
            #TODO: set savefolder
            nameAndFolder = self.getNameAndFolder(content[i])
            hist.setOption("input", nameAndFolder[0])#same hist for all vars
            hist.setOption("name", nameAndFolder[0])
            hist = self.applyAttributes(hist)
            sf = self.getOptionFor("savefolder", nameAndFolder[0]) #savefolder
#            if self.attributelist.has_key(nameAndFolder[0]) and self.attributelist[nameAndFolder[0]].hasOption("savefolder"):
#                sf = self.attributelist[nameAndFolder[0]].getOption("savefolder")
#            elif self.attributelist.has_key(HistogramList.ALL):
#                sf = self.attributelist[HistogramList.ALL].getOption("savefolder")
            
            hist.setOption("savefolder", sf + "/" + nameAndFolder[1])
            if self.subobjects.has_key("legend"):
                hist.subobjects["legend"] = self.subobjects["legend"]
            for var in self.subobjects["hist"]:
                    inputtmp = self.getVarInput(var, inputs, self.getOption("input"))
                    varsource = self.getVarSourceFile(var)
                    contenttmp = content
                    if not inputtmp:
                        inputtmp = Input.createInput()
                    inputtmp.setContent(contenttmp)
                    contenttmp = inputtmp.getFilteredContent()
                    nameAndFoldertmp = self.getNameAndFolder(contenttmp[i])
                    var.setOption("input","f%s%s" %(Input.separator, nameAndFoldertmp[1]))
                    hist.subobjects["hist"].append(var.Copy())
            hist.resolveInputs(files, inputs)
            self.subobjects["canv"].append(hist)
            
    def applyAttributes(self, hist):
            name = hist.getOption("name")
            opts = self.getOptionSetFor(name)
            hist.setOptions(opts)
            return hist
            
            
    def getNameAndFolder(self, input):
        dirs = input.split("/")
        folder = ""
        hist = dirs[len(dirs)-1]
        for x in range(0, len(dirs)-1):
            folder += "%s/" %dirs[x]
        return [hist, folder]
                
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
    
    def getVarInput(self, var, inputs, histinput = ""):
#        histinput = self.getOption("input")
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
        elif "i"+ Input.separator in histinput or "i" + Input.separator in varinput:
            if varinput:
                ret = varinput.replace("i"+Input.separator, "")
            else:
                ret = histinput.replace("i"+Input.separator, "")
            if inputs.has_key(ret):
                ret = inputs[ret]
#        elif histinput and not varinput:
#            ret = histinput
#        elif varinput and not histinput
        return ret
        
class Variable(ConfigObject):
    ksourceFile = 'file'
    ksourceVar = 'hist'
    typeDelimiter = ':'
    entryDelimiter = ','
    
    def __init__(self):
        ConfigObject.__init__(self, "hist", [])
        
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
        folder = ""
        self.filtered = []
        if self.isMetaFolder():
            self.filtered = self.content
        else:
            folder = self.getOption("name")
            if not self.subobjects["filter"]:
                f = Filter()
                #TODO: change mandatory options
                f.options.addOption("type", "")
                f.options.addOption("value", "")
                self.subobjects["filter"].append(f)
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
            
    def isMetaFolder(self):
        ret = False
        if "*" in self.getOption("name"):
            ret = True
        return ret
    
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
                if dir.getOption("name") in hist or dir.isMetaFolder() :
                    l.append(hist)
            if l:
                dir.setContent(l) 
                
    def getFilteredContent(self):
        content = []
        for dir in self.subobjects["folder"]:
            content.extend(dir.getFilteredContent())
        return content
    
    def createInput():
        inp = Input()
        folder = Folder()
        folder.options.addOption("name", "*")
        inp.subobjects["folder"] = [folder]
        return inp
    createInput = staticmethod(createInput)
    
class Include(ConfigObject):

    def __init__(self):
        ConfigObject.__init__(self, rootNodeName= "include", doNotParse = [], mandatoryObjects = [], mandatoryOptions = [])
        self.setups = {}
    
    def parseFromNode(self, node):
        file = ""
        if Parser.hasAttribute(node, "file"):
            file = Parser.getAttributeValue(node, "file")
        else:#try to read as TextNode
            file = Parser.getText(node)
        file = pathToDir + file
        FileService.exists(file)
        self.options.addOption("file", file)
        
    def getListOfSetups(self):
        setups = {}
        file = self.getOption("file")
        hists = Parser.getChildNodes(Parser.getDocumentRoot(file), "canv")
        if not hists:
            Print('Warning: empty setup file', 2)
        for i in hists:
            hist = Histogram()
            hist.parseFromNode(i)
            setups[hist.getOption("name")] = hist
        return setups
    
