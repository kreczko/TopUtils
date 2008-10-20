from TopInspectWrapper import InspectWrapper as iw
  
class Config(iw):
    allowedTypes = "top, topbg, qcd, wjets, zjets"
    #move to makeIsolationPlots
#    __isoHist = "Isolation.hist"
#    __iso2Dhist = "Isolation2D.hist"
    __qcdHist = "QCDMatrix.hist"
    __maccroHist = 'Macro.hist'
    __cfgFile = '../test/TopInspectTemplate.cfg'
    __module = 'allmafter'
#    __samples = ''
    "constructor"
    def __init__(self, inspectType):
        iw.__init__(self, self.__cfgFile, inspectType)
        #number of hists
        self.__noh = len(self.__readFromFile('Macro.hist').split('\n'))-1
        #standard values
        #hist file
        self.modifyOption('histInput', 'Macro.hist')
        #rootfile
        self.modifyOption('rootInput' , 'MatrixEfficienciesWithMu_130908.root')
        #root file directories
        self.modifyOption('inputDirs', 'qcd/trackmafter')
        self.modifyOption('inputDirs', 'top/trackmafter')
        self.modifyOption('inputDirs', 'mixed/trackmafter')
        #hist manipulation
        for x in range(1,self.__noh + 1):
            #histminimum
            self.modifyOption('histMinimum', '0.0')
            #show errors or not
            self.modifyOption('histErrors', '1')
            
            self.modifyOption('histType', '0')
            self.modifyOption('xAxes', 'MET[GeV]')
            if x == 1:
                self.modifyOption('yAxes', 'efficiency')
            else:
                self.modifyOption('yAxes', 'events')
        #next variables left to InspectWrapper values
        
    def __readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str
    
    def setModule(self, module):
        self.__module = module
        