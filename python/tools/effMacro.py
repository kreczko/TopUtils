from ROOT import gROOT, TCanvas, TH1F, TFile
from math import sqrt

class Macro:
    before = 'mbefore'
    after = 'mafter'
    debug = True
    cuts = [0.1, 0.2, 0.3]
    def __init__(self, outputfile, inputfiles, inputdirs):
        print 'starting macro'
        self.effsig = {}
        self.effbg = {}
        self.nloose = {}
        self.ntight = {}
        self.nTtruebg = {}
        self.nTtruebg = {}
        self.nTtruesig = {}
        self.nLtruebg = {}
        self.nLtruesig = {}
        self.nloose = {}
        self.ntight = {}
        self.effsig = {}
        for i in inputdirs:
            self.effbg[i] = {}
            
        
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
        print 'calculating efficiency for', type
        if type in ['qcd', 'top'] and type in self.inputfiles.keys():
            #just to be sure:
            self.output.Cd('/')
            self.createFolders(type, self.inputdirs)
            #write old histogramms
            self.doEff(type)            
            
        elif type == 'mixed' and len(self.inputfiles.keys()) > 0:
            self.output.Cd('/')
            self.createFolders(type, self.inputdirs)
            #for i in self.inputdirs: 
                #self.output.cd(type + '/' + i)
            self.doEffMixed()
                #eff.Write()
                #self.output.Cd('/')
        else:
            print 'Error'
            
    def doEff(self, type):
        qqq = self.inputfiles[type]
        for i in self.inputdirs:
            after = qqq.Get(i + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight')
            unwafter = qqq.Get(i + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple')
            before = qqq.Get(i + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose')
            unwbefore = qqq.Get(i + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple')
            if after == None or before == None or unwbefore == None:
                print 'Histogram not found'
            else:
                self.setMCTruth(type, i, before, after)
                self.output.cd(type + '/' + i)
                result = self.getEffHist(before, after, unwbefore)
                eff = result[0]
                eff.Write()
                after.Write()
                unwafter.Write()
                before.Write()
                unwbefore.Write()
                self.output.Cd('/')
                #caluclate MC Efficiency without binning
                mceff = after.Integral()/before.Integral()
                mcerr = sqrt((mceff*(1-mceff))/unwbefore.Integral())
                self.setEff(type, i, 0, mceff, mcerr)
                print 'MC Eff for', i, ':[', mceff,',', mcerr, ']' 
                for effs in result[1]:
                    print 'Eff for', i, 'disc <', effs[0], ':[', effs[1], ',', effs[2], ']'
                    
                
                
    def doEffMixed(self):
        for dir in self.inputdirs:
            #get first file
            file = self.inputfiles[self.inputfiles.keys()[0]]
            #global var needed, because the scope ends in 2nd for loop
            self.mixa = file.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight')
            self.mixunwa = file.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple')
            self.mixb = file.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose')
            self.mixunwb = file.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple')
            if self.mixa == None or self.mixb == None or self.mixunwb == None or self.mixunwa == None:
                print 'hist not found', dir, self.after, '/', self.hists['weighted']
            #sum over all files except the first one
            for x in range(1, len(self.inputfiles.keys())):
                file = self.inputfiles[self.inputfiles.keys()[x]]
                self.mixa.Add(file.Get(dir + self.after + '/' + self.hists['weighted']))
                self.mixunwa.Add(file.Get(dir + self.after + '/' + self.hists['simple']))
                self.mixb.Add(file.Get(dir + self.before + '/' + self.hists['weighted']))
                self.mixunwb.Add(file.Get(dir + self.before + '/' + self.hists['simple']))
            
            self.setN(dir, self.mixb.Integral(), self.mixa.Integral())
            self.output.cd('mixed' + '/' + dir)
            result = self.getEffHist(self.mixb, self.mixa, self.mixunwb)
            eff = result[0]
            eff.Write()
            self.mixa.Write()
            self.mixunwa.Write()
            self.mixb.Write()
            self.mixunwb.Write()
            self.output.Cd('/')
            for effs in result[1]:
                print 'Eff for', dir, 'disc <', effs[0], ':', round(effs[1],3), '+-', round(effs[2],3)
                self.setEff('mixed', dir, effs[0], round(effs[1],3), round(effs[2],3))                
                
    def getEffHist(self, before, after, unweightedBefore):
        effplot = after.Clone("efficiency")
        effplot.Divide(before)
        for i in range(1, effplot.GetNbinsX() + 1):
            eff = effplot.GetBinContent(i)
            err = sqrt((eff * (1 - eff)) / unweightedBefore.GetBinContent(i))
            if self.debug:
                print 'before:', before.GetBinContent(i), 'after:', after.GetBinContent(i), 'unw.:', unweightedBefore.GetBinContent(i)
                print round(eff,3), '+-', round(err,3)
            effplot.SetBinError(i, err)
            effplot.SetMinimum(0)
        #cut based eff
        effs = []
        for x in self.cuts:
            bin = after.GetXaxis().FindBin(x)
            na = after.Integral(1, bin)
            nb = before.Integral(1, bin)
            nunwb = unweightedBefore.Integral(1, bin)
            eff = na / nb
            err = sqrt((eff * (1 - eff)) / nunwb)
            effs.append([x, round(eff,3), round(err,3)])
        
        return [effplot, effs]
        
    def makeAditionalPlots(self):
        #summary plots:
        # - Efficiency for all Isotypes in one plot
        # - 
        print 'printing all plots'
        #walk through all types
        #walk through all inputs
        #get efficiency fom all imputs
        #put all histograms together (have a look in inspect)
        # Draw("samee")
        
    def getAllEfficienies(self, type):
        #will return a canvas
        for i in self.inputdirs:
            print i
        
    def matrixMethod(self, effbg, effsig, nloose, ntight):
        nbg = ((nloose * effsig) - ntight) / (effsig - effbg)
        nbga = nbg * effbg
        nsig = (ntight - (nloose * effbg)) / (effsig - effbg)
        nsiga = nsig*effsig
        return {'NQL':int(nbg), 'NQT':int(nbga), 'NSL':int(nsig), 'NST':int(nsiga)}
        
    def createFolders(self, parentfolder, folderlist):
        for i in folderlist:
                self.output.GetDirectory(parentfolder).mkdir(i)
    
    def setMCTruth(self, type, input, before, after):
        if type == 'top':
            self.nTtruesig[input] = after.Integral()
            self.nLtruesig[input] = before.Integral()
        else:
            self.nTtruebg[input] = after.Integral()
            self.nLtruebg[input] = before.Integral()
            
    def setEff(self, type, input, cut, eff, err):
        #signal from monte carlo, bg from mixed sample control region
        if type == 'top':
            self.effsig[input] = [round(eff,3),round(err,3)]
        elif type == 'mixed':
            self.effbg[input][cut] = [round(eff,3),round(err,3)]
    
    def setN(self, input, loose, tight):
        self.nloose[input] = int(loose)
        self.ntight[input] = int(tight)
            
    def printEffSummary(self):
        print ''
        print '########################################'
        print '###       Efficiency Summary:        ###'
        print '########################################'
        for x in self.cuts:
            print 'for cut: disc <', x
            print 'isolation & $\epsilon_{sig}$ & $\epsilon_{qcd}$ & $N_L$ & $N_T$ \\\\'
            for i in self.inputdirs:
                print i, '&', '[',self.effsig[i][0],round(self.effsig[i][1],3),'] &', '[',round(self.effbg[i][x][0],3),round(self.effbg[i][x][1],3),'] &', self.nloose[i], '&', self.ntight[i], '\\\\'
            print ''
            print 'isolation & TrueTight(sig) & TrueTight(qcd) & TrueLoose(sig) & TrueLoose(qcd) \\\\'
            for i in self.inputdirs:
                print i, '&', int(self.nTtruesig[i]), '&', int(self.nTtruebg[i]), '&', int(self.nLtruesig[i]), '&', int(self.nLtruebg[i]), '\\\\'
            print ''
            print 'isolation & $N_T(sig)$ & $N_T(qcd)$ & Rel(sig)in \%  & Rel(qcd)in \%\\\\'
            for i in self.inputdirs:
                matrix = self.matrixMethod(self.effbg[i][x][0], self.effsig[i][0], self.nloose[i], self.ntight[i])
                relsig = (self.nTtruesig[i] - matrix['NST'])/self.nTtruesig[i]*100
                relqcd = (self.nTtruebg[i] - matrix['NQT'])/self.nTtruebg[i]*100
                print i, '&', int(matrix['NST']), '&', int(matrix['NQT']), '&',round(relsig,2) , '&', round(relqcd,2), '\\\\'
            print ''
        
if __name__ == '__main__':
    gROOT.Reset()
    inputfiles = {}
    inputfiles['qcd'] = "MatrixMethod_qcdmu_041208.root"
    inputfiles['top'] = "MatrixMethod_top_031208.root"
    inputdirs = ["all", "jetIso", "calo", "track"]
    mac = Macro('tst.root', inputfiles, inputdirs)
    mac.debug = False
    mac.calcEff('qcd')
    mac.calcEff('top')
    mac.calcEff('mixed')
    mac.makeAditionalPlots()
    mac.printEffSummary()
        