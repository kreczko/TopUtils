from ConfigWrapper import ConfigWrapper
import SourceInput as input

"a config for qcd analysis"
class Config(ConfigWrapper):
    allowedTypes = "top, topbg, qcd, wjets, zjets, qcdmu"
    allowedPathTypes = 'analysis, track, calo, jet, all'
    __fileNameAddition = ''
#    __ttbarMC = 'false'
    "constructor"
    def __init__(self, type, pathtypes):
        self.__fileNameAddition = pathtypes.replace(';', '_')
        ConfigWrapper.__init__(self, 'TopAnalysis/TopUtils/test/QCDConfigTemplate.cfg', type)
        #paths:
        self.__path = {}
        #self.__options = ConfigWrapper.__options
        #self.__options['ttbarMC'] = ''
        self._options['ttbarMC'] = 'false'
#        self.__ttbarMC = 'false'
        self.__path['basic'] = 'makeWeights & makeGenEvt'
        self.__path['analysis'] = 'analyzeisolationMET'
        self.__path['track'] = 'looseSelection,trackmbefore, trackIsoFilter, trackmafter'
        self.__path['calo'] = 'looseSelection,calombefore, caloIsoFilter, calomafter'
        self.__path['jet'] = 'looseSelection,jetIsombefore, jetIsoFilter, jetIsomafter'
        self.__path['all'] = 'looseSelection,allmbefore,allFilter,  allmafter'

        #prefiltering on MC Truth (p2)
        top = "ttSemiLeptonicFilter"
        topbg = "!ttSemiLeptonicFilter"

        paths = pathtypes.split(';')
        
        self.addPath(self.__path['basic'])
        for x in paths:
            if x in self.allowedPathTypes:
                #standard path:
                self.__path['standard'] = self.__path[x]
                self.__path['top'] = self.join(top, self.__path[x])
                self.__path['topbg'] = self.join(topbg, self.__path[x])
        if type in self.__path.keys():
            self.addPath(self.__path[type])
        
        
        if type == 'top':
            self.modifyOption('ttbarMC', 'true')
        else:
            self.modifyOption('ttbarMC', 'false')
            
        if 'top' in type:
            type = 'ttbar'
        
        if type in input.source.keys():
            self.modifyOption('source', input.source[type])
        else:
            print 'Unknown type "', type, '" for source'
    
    "joins two paths together"
    def join(self, e1, e2):
        return e1 + "," + e2
        
    #def _replaceAll(self):
       # self._replaceInFile("{$ttbarMC}", self.__ttbarMC)
        #ConfigWrapper._replaceAll(self)

    def GetFileNameAddition(self):
        return self.__fileNameAddition