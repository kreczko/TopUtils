from ROOT import gROOT, TCanvas, TH1F, TFile
from math import sqrt

class Macro:
    before = 'mbefore'
    after = 'mafter'
    debug = True
    def __init__(self, outputfile, inputfiles, inputdirs):
        print 'starting macro'
        self.hists = {}
        self.hists['weighted'] = 'mbg_nVSdisc'
        #self.hists['weightedTight'] = 'mbg_nVSdiscTight'
        self.hists['simple'] = 'mbg_nVSdiscSimple'
        #self.hists['simpleTight'] = 'nVSdiscSimpleTight'
        
        self.inputdirs = inputdirs        
        
        self.output = TFile(outputfile, 'RECREATE')
        self.inputfiles = {}
        for i in inputfiles.keys():
            self.inputfiles[i] = TFile(inputfiles[i])
        
        self.createFolders('/', self.inputfiles.keys())
        self.output.mkdir('mixed')
            
    def __del__(self):
        self.output.Close()
        for i in self.inputfiles.keys():
            self.inputfiles[i].Close()
            
    def calcEff(self, type):
        print 'calculating efficiency'
        if type in ['qcd', 'top'] and type in self.inputfiles.keys():
            if self.debug:
                print 'for', type
            #just to be sure:
            self.output.Cd('/')
            self.createFolders(type, self.inputdirs)
            #write old histogramms
            self.doEff(type)            
            
        elif type == 'mixed' and len(self.inputfiles.keys()) > 0:
            if self.debug:
                print 'for', type
            self.output.Cd('/')
            self.createFolders(type, self.inputdirs)
            for i in self.inputdirs: 
                #self.output.cd(type + '/' + i)
                self.doEffMixed(i)
                #eff.Write()
                #self.output.Cd('/')
        else:
            print 'Error'
            
    def doEff(self, type):
        qqq = self.inputfiles[type]
        for i in self.inputdirs:
            after = qqq.Get(i + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight')
            unwafter  = qqq.Get(i + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple')
            before = qqq.Get(i + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose')
            unwbefore = qqq.Get(i + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple')
            if after == None or before == None or unwbefore == None:
                print 'Histogram not found'
            else:
                self.output.cd(type + '/' + i)
                eff = self.getEffHist(before, after, unwbefore)
                eff.Write()
                after.Write()
                unwafter.Write()
                before.Write()
                unwbefore.Write()
                self.output.Cd('/')
                
    def doEffMixed(self, dir):
        #get first file
        file = self.inputfiles[self.inputfiles.keys()[0]]
        #global var needed, because the scope ends in 2nd for loop
        self.mixa = file.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight')
        self.mixunwa = file.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple')
        self.mixb = file.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose')
        self.mixunwb = file.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple')
        if self.mixa == None or self.mixb == None or self.mixunwb == None or self.mixunwa == None:
            print 'hist not found', dir,self.after,'/',self.hists['weighted']
        #sum over all files except the first one
        for x in range(1,len(self.inputfiles.keys())):
            file = self.inputfiles[self.inputfiles.keys()[x]]
            self.mixa.Add(file.Get(dir + self.after + '/' + self.hists['weighted']))
            self.mixunwa.Add(file.Get(dir + self.after + '/' + self.hists['simple']))
            self.mixb.Add(file.Get(dir + self.before + '/' + self.hists['weighted']))
            self.mixunwb.Add(file.Get(dir + self.before + '/' + self.hists['simple']))
            
            
        self.output.cd('mixed' + '/' + dir)
        eff = self.getEffHist(self.mixb, self.mixa, self.mixunwb)
        eff.Write()
        self.mixa.Write()
        self.mixunwa.Write()
        self.mixb.Write()
        self.mixunwb.Write()
        self.output.Cd('/')
        #for x in range(1,len(self.inputfiles.keys())):
        #    file = self.inputfiles[self.inputfiles.keys()[x]]            
        #    for i in range(1, len(self.inputdirs)):
        #        print self.mixa.GetBinContent(1)
         #       dir = self.inputdirs[i]
         #       self.mixa.Add(file.Get(dir + self.after + '/' + self.hists['weighted']))
         #       self.mixb.Add(file.Get(dir + self.before + '/' + self.hists['weighted']))
          #      self.mixunwb.Add(file.Get(dir + self.before + '/' + self.hists['simple']))
        #return self.getEffHist(self.mixb, self.mixa, self.mixunwb)
                
    def getEffHist(self, before, after, unweightedBefore):
        effplot = after.Clone("efficiency")
        effplot.Divide(before)
        for i in range(1, effplot.GetNbinsX() + 1):
            eff = effplot.GetBinContent(i)
            err = sqrt((eff * (1 - eff)) / unweightedBefore.GetBinContent(i))
            if self.debug:
                print 'before:', before.GetBinContent(i), 'after:', after.GetBinContent(i), 'unw.:', unweightedBefore.GetBinContent(i)
                print eff, '+-', err
            effplot.SetBinError(i, err)
            effplot.SetMinimum(0)
        
        return effplot
        
    def printAllPlots(self):
        #summary plots:
        # - Efficiency for all Isotypes in one plot
        # - 
        print 'printing all plots'
        
    def createFolders(self, parentfolder, folderlist):
        for i in folderlist:
                self.output.GetDirectory(parentfolder).mkdir(i)
    
        
if __name__ == '__main__':
    gROOT.Reset()
    inputfiles = {}
    inputfiles['qcd'] = "MatrixMethod_qcdmu_011208.root"
    inputfiles['top'] = "MatrixMethod_top_011208.root"
    inputdirs = ["all", "jetIso", "calo", "track"]
    mac = Macro('tst.root', inputfiles, inputdirs)
    mac.debug = False
    mac.calcEff('qcd')
    mac.calcEff('top')
    mac.calcEff('mixed')
        