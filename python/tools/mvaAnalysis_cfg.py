from ConfigWrapper import ConfigWrapper
import SourceInput as input
import sys

"a config for mca analysis"
class Config(ConfigWrapper):
    allowedTypes = "top, topbg, thad, qcd, wjets, zjets, qcdmu, topr"
    allowedPathTypes = 'training,compute,save,filtertest'
    __fileNameAddition = ''
    __lumi = 50 #pb^-1
    __savefile = 'TopAnalysis/TopFilter/data/TreeSaver.xml'
    __trainfile = 'TopAnalysis/TopFilter/data/TtSemiLepSignalSelectorMVATrainer_Muons.xml'
    "constructor"
    def __init__(self, type, pathtypes):
        self.__fileNameAddition = pathtypes.replace(';', '_')
        ConfigWrapper.__init__(self, 'TopAnalysis/TopUtils/test/mvaTemplate.cfg', type)
        self.__path = {}
        self._options['eventWeight'] = ''
        self._options['looper'] = ''
        self._options['mvamodule'] = ''

        #paths:
        self.__path['basic'] = 'makeWeights & makeGenEvt'
        self.__path['training'] = 'looseSelection, makeMVATraining'
        self.__path['compute'] = 'looseSelection, findTtSemiLepSignalSelectorMVA, analyzeDisc'
        self.__path['filtertest'] = 'looseSelection, findTtSemiLepSignalSelectorMVA, mvaDiscFilter'
        self.__path['save'] = self.__path['training']

        #prefiltering on MC Truth (p2)
        top = "ttSemiLeptonicFilter"
        topbg = "!ttSemiLeptonicFilter"
        thad = "ttFullyHadronicFilter"
        topr = "!ttFullyHadronicFilter, !ttSemiLeptonicFilter";

        paths = pathtypes.split(';')
        
        self.addPath(self.__path['basic'])
        for x in paths:
            if x in self.allowedPathTypes.split(','):
                #standard path:
                self.__path['standard'] = self.__path[x]
                self.__path['top'] = self.join(top, self.__path[x])
                self.__path['topbg'] = self.join(topbg, self.__path[x])
                self.__path['thad'] = self.join(thad, self.__path[x])
                self.__path['topr'] = self.join(topr, self.__path[x])
                if ('top' in type) or type == 'thad':
                    self.addPath(self.__path[type])
                else:
                    self.addPath(self.__path['standard'])
            else:
                print 'Wrong path!'
                sys.exit(2)
            
        if type == 'qcdmu':
            eventWeight = 'include "TopAnalysis/TopUtils/data/EventWeightPlain.cfi"\n'
            eventWeight += 'replace eventWeight.eff = 0.00028\n '
            eventWeight += 'replace eventWeight.xsec  = 819900000. \n' #pb-1
            eventWeight += 'replace eventWeight.nevts = 2037232 \n'  
            eventWeight += 'replace eventWeight.lumi =' + self.__lumi.__str__()
        else:
            eventWeight = 'include "TopAnalysis/TopUtils/data/EventWeight.cfi"\n'
            eventWeight += 'replace csa07Event.overallLumi = ' + self.__lumi.__str__()
        
            
        self.modifyOption('eventWeight', eventWeight)
        if ('top' in type) or type == 'thad':
            type = 'ttbar'
            
        looper = ''
        mvamodule = ''
        if 'training' in pathtypes or 'save' in pathtypes:
            mvamodule = 'include "TopAnalysis/TopFilter/data/TtSemiLepSignalSelectorMVATrainer_Muons.cff"'
            type = 'mvaT' + type
            looper += 'looper = TtSemiLepSignalSelectorMVATrainerLooper {' + '\n'
            looper += 'VPSet trainers = {' + '\n'
            looper += '{' + '\n'
            looper += 'string calibrationRecord = "ttSemiLepSignalSelectorMVA"' + '\n'
            if 'save' in pathtypes:
                looper += ' untracked string trainDescription = "' + self.__savefile + '"' + '\n'
                looper += 'untracked bool saveState = false' + '\n'
            else:
                looper += ' untracked string trainDescription = "' + self.__trainfile + '"' + '\n'
                looper += 'untracked bool saveState = true' + '\n'
            looper += 'untracked bool loadState = false' + '\n'
            looper += 'untracked bool monitoring = true' + '\n'
            looper += '}' + '\n'
            looper += '}' + '\n'
            looper += '}' + '\n'
            
        if 'compute' in pathtypes or 'filtertest' in pathtypes:
            type = 'mvaC' + type
            mvamodule = 'include "TopAnalysis/TopFilter/data/TtSemiLepSignalSelectorMVAComputer_Muons.cff"'
            looper += '# define the event content' '\n'
            looper += 'block myEventContent = {' '\n'
            looper += 'untracked vstring outputCommands = {' + '\n'
            looper += '"drop *"' + '\n'
            looper += ',"keep double_*_DiscSel_*"' + '\n'
            looper += '}' + '\n'
            looper += '}' + '\n'
            looper += '# the actual output module' + '\n'
            looper += ' module out = PoolOutputModule {' + '\n'
            looper += 'untracked string fileName = "Lkh_output"' + '\n'
            looper += 'using myEventContent' + '\n'
            looper += 'untracked bool verbose = false' + '\n'
            looper += '}' + '\n'
            looper += 'endpath outpath = { out }' + '\n'
            
        if type in input.source.keys():
            self.modifyOption('source', input.source[type])
        else:
            print 'Unknown type "', type, '" for source'
            sys.exit(1)
        self.modifyOption('mvamodule', mvamodule)
        self.modifyOption('looper', looper)
    
    "joins two paths together"
    def join(self, e1, e2):
        return e1 + "," + e2
        
    #def _replaceAll(self):
       # self._replaceInFile("{$ttbarMC}", self.__ttbarMC)
        #ConfigWrapper._replaceAll(self)

    def GetFileNameAddition(self):
        return self.__fileNameAddition
