from ConfigWrapper import ConfigWrapper
import SourceInput as input

"a config for qcd analysis"
class Config(ConfigWrapper):
    allowedTypes = "top,topbg,qcd,wjets,zjets,qcdmu"
    allowedPathTypes = 'analysis,track,calo,jet,all'
    __fileNameAddition = ''
    __lumi = 50 #pb^-1
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
        self._options['eventWeight'] = ''
#        self.__ttbarMC = 'false'
        self.__path['basic'] = 'makeWeights & makeGenEvt'
        self.__path['analysis'] = 'analyzeEventShapeMuon, analyzeisolationMET'
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
            print x
            if x in self.allowedPathTypes.split(','):
                #standard path:
                self.__path['standard'] = self.__path[x]
                self.__path['top'] = self.join(top, self.__path[x])
                self.__path['topbg'] = self.join(topbg, self.__path[x])
                self.addPath(self.__path[x])
        
        
        if type == 'top':
            self.modifyOption('ttbarMC', 'true')
        else:
            self.modifyOption('ttbarMC', 'false')
            
        if type == 'qcdmu':
            eventWeight = 'include "TopAnalysis/TopUtils/data/EventWeightPlain.cfi"\n'
            eventWeight += 'replace eventWeight.eff = 0.00028\n '
            eventWeight += 'replace eventWeight.xsec  = 819900000. \n' #pb-1
            eventWeight += 'replace eventWeight.nevts = 2037232 \n'  
            eventWeight += 'replace eventWeight.lumi ='+ self.__lumi.__str__()
        else:
            eventWeight = 'include "TopAnalysis/TopUtils/data/EventWeight.cfi"\n'
            eventWeight += 'replace csa07Event.overallLumi = '+ self.__lumi.__str__()
        
            
        self.modifyOption('eventWeight', eventWeight)
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