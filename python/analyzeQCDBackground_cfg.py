from ConfigWrapper import ConfigWrapper

"a config for qcd analysis"
class Config(ConfigWrapper):
    allowedTypes = "top, topbg, qcd, wjets, zjets"
    "constructor"
    def __init__(self, type):
        ConfigWrapper.__init__(self, 'TopAnalysis/TopUtils/python/QCDConfigTemplate.cfg')
        #paths:
        self.path = {}
        self.ttbarMC_ = 'false'

    

        p0a = "makeWeights & makeGenEvt"
        p0 = "analyzeisolationMET"
        p1 = "looseSelection,trackmbefore, trackIsoFilter, trackmafter"
        p2 = "looseSelection,calombefore, caloIsoFilter, calomafter"
        p3 = "looseSelection,jetIsombefore, jetIsoFilter, jetIsomafter"
        p4 = "looseSelection,allmbefore,allFilter,  allmafter"

        #prefiltering on MC Truth (p2)
        top = "ttSemiLeptonicFilter"
        topbg = "!ttSemiLeptonicFilter"

        #standard path:
        self.path['standard'] = self.makePaths(p0a, p0, p1, p2, p3, p4)
        self.path['top'] = self.makePaths(p0a, self.join(top, p0), self.join(top, p1), self.join(top, p2), self.join(top, p3), self.join(top, p4))
        self.path['topbg'] = self.makePaths(p0a, self.join(topbg, p0), self.join(topbg, p1), self.join(topbg, p2), self.join(topbg, p3), self.join(topbg, p4))
        if not type in self.path.keys():
            for a in self.path['standard']:
                self.addPath(a.__str__())
        else:
           for a in self.path[type]:
                self.addPath(a.__str__())
        
        
        if type == 'top':
            self.ttbarMC('true')
        else:
            self.ttbarMC('false')
        if 'top' in type:
            self.source('ttbar')
        else:
            self.source(type)
    
    "joins two paths together"
    def join(self, e1, e2):
        return e1 + "," + e2
    
    "creates and array of paths"
    def makePaths(self, p0, p1, p2, p3, p4, p5):
        return [p0, p1, p2, p3, p4, p5]
    
    def ttbarMC(self, bool):
        self.ttbarMC_ = bool
        
    def replaceAll(self):
        ConfigWrapper.replaceAll(self)
        self.replaceInFile("{$ttbarMC}", self.ttbarMC_)

