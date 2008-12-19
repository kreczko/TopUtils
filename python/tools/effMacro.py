from ROOT import gROOT, TCanvas, TH1F, TFile, TF1
from math import sqrt
import os
from MatrixMethod import MatrixMethod

class Macro:
    before = 'mbefore'
    after = 'mafter'
    debug = True
    cuts = [0.1, 0.2, 0.3, 0.4, 0.5, 0.9]
    latexfile = 'Macro.tex'
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
        if type in ['qcd', 'top', 'wjets'] and type in self.inputfiles.keys():
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
                mceff = after.Integral() / before.Integral()
                mcerr = sqrt((mceff * (1 - mceff)) / unwbefore.Integral())
                self.setEff(type, i, 0, mceff, mcerr)
                print 'MC Eff for', i, ':[', mceff, ',', mcerr, ']' 
                for effs in result[1]:
                    print 'Eff for', i, 'disc <', effs[0], ':[', effs[1], ',', effs[2], ']'
                    
                
                
    def doEffMixed(self):
        for dir in self.inputdirs:
            #get first file
            #file = self.inputfiles[self.inputfiles.keys()[0]]
            i = 0
            for x in self.inputfiles.keys():
                
                file = self.inputfiles[x]
                if (i == 0):
                    self.mixa = file.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight')
                    self.mixunwa = file.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple')
                    self.mixb = file.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose')
                    self.mixunwb = file.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple')
                    if self.mixa == None or self.mixb == None or self.mixunwb == None or self.mixunwa == None:
                        print 'hist not found', dir, self.after, '/', self.hists['weighted']
                else:
                    self.mixa.Add(file.Get(dir + self.after + '/' + self.hists['weighted']))
                    self.mixunwa.Add(file.Get(dir + self.after + '/' + self.hists['simple']))
                    self.mixb.Add(file.Get(dir + self.before + '/' + self.hists['weighted']))
                    self.mixunwb.Add(file.Get(dir + self.before + '/' + self.hists['simple']))
                i += 1           
            
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
                print 'Eff for %s disc < %1.2f : %1.3f +- %1.3f' % (dir, effs[0], effs[1], effs[2])
                self.setEff('mixed', dir, effs[0], effs[1], effs[2])                
                
    def getEffHist(self, before, after, unweightedBefore):
        effplot = after.Clone("efficiency")
        effplot.Divide(before)
        for i in range(1, effplot.GetNbinsX() + 1):
            eff = effplot.GetBinContent(i)
            err = sqrt((eff * (1 - eff)) / unweightedBefore.GetBinContent(i))
            if self.debug:
                print 'before: %d after: %d undw.: %d' % (before.GetBinContent(i), after.GetBinContent(i), unweightedBefore.GetBinContent(i))
                print '%1.3f +- 1.3f' % (eff, err)
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
            effs.append([x, eff, err])
        
        return [effplot, effs]
    
    def makeSignalToBKG(self, signal, background):
        sb = signal.Clone('SiignalOverAll')
        all = signal.Clone('all')
        all.Add(background)
        sb.Divide(all)
        return sb
        
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
        
    def matrixMethod(self, effqcd, effsig, Nloose, Ntight):
        nbg = ((Nloose * effsig) - Ntight) / (effsig - effqcd)
        nbga = nbg * effqcd
        nsig = (Ntight - (Nloose * effqcd)) / (effsig - effqcd)
        nsiga = nsig * effsig
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
            self.effsig[input] = [eff, err]
        elif type == 'mixed':
            self.effbg[input][cut] = [eff, err]
    
    def setN(self, input, loose, tight):
        self.nloose[input] = int(loose)
        self.ntight[input] = int(tight)
            
    def printEffSummary(self):
        print ''
        print '########################################'
        print '###       Efficiency Summary:        ###'
        print '########################################'
        for x in self.cuts:
            print 'for cut: disc < %1.3f' % x
            print 'isolation | eff{sig} | eff{qcd} | N(L) | N(T)'
            for i in self.inputdirs:
                print '%s | %1.3f +- %1.3f | %1.3f +- %1.3f | %d | %d' % (i, self.effsig[i][0], self.effsig[i][1], self.effbg[i][x][0], self.effbg[i][x][1], self.nloose[i], self.ntight[i])
                #print i, '&', '[', self.effsig[i][0], round(self.effsig[i][1], 3), '] &', '[', round(self.effbg[i][x][0], 3), round(self.effbg[i][x][1], 3), '] &', self.nloose[i], '&', self.ntight[i], '\\\\'
            print ''
            print 'isolation | TrueTight(sig) | TrueTight(qcd) | TrueLoose(sig) | TrueLoose(qcd)'
            for i in self.inputdirs:
                print '%s | %9d | %9d | %9d | %9d' % (i, self.nTtruesig[i], self.nTtruebg[i], self.nLtruesig[i], self.nLtruebg[i])
                #print i, '&', int(self.nTtruesig[i]), '&', int(self.nTtruebg[i]), '&', int(self.nLtruesig[i]), '&', int(self.nLtruebg[i]), '\\\\'
            print ''
            print 'isolation & $N_T(sig)$ & $N_T(qcd)$ & Rel(sig)in \%  & Rel(qcd)in \%\\\\'
            for i in self.inputdirs:
                matrix = self.matrixMethod(self.effbg[i][x][0], self.effsig[i][0], self.nloose[i], self.ntight[i])
                relsig = (self.nTtruesig[i] - matrix['NST']) / self.nTtruesig[i] * 100
                relqcd = (self.nTtruebg[i] - matrix['NQT']) / self.nTtruebg[i] * 100
                print '%s | %d | %d | %3.2f | %3.2f' % (i, int(matrix['NST']), int(matrix['NQT']), relsig, relqcd)
                #print i, '&', int(matrix['NST']), '&', int(matrix['NQT']), '&', round(relsig, 2) , '&', round(relqcd, 2), '\\\\'
            print ''
            
    def exportLatex(self):
        print '################################################'
        print '###         Exporting Latex and PDF          ###'
        print '################################################'
        content = '\\documentclass[a4paper,12pt]{report}\n'
        content += '\\usepackage[ngerman, english]{babel}\n'
        content += '\\begin{document}\n'
        col = ' & '
        #now fill the tables
        for x in self.cuts:
            content += '\\section{efficiencies for control region: discriminator $<$ %1.3f}\n' % x
            content += '\\begin{table}[h] \n'
            content += '\\centering \n'
            content += '\\begin{tabular}{c|c|c|c|c} \n'
            content += 'isolation & $\epsilon_{sig}$ & $\epsilon_{qcd}$ & $N_L$ & $N_T$ \\\\ \n'
            for i in self.inputdirs:
                content += ' %s & %1.3f $\pm$ %1.3f] & [%1.3f, %1.3f] & %d & %d  \\\\ \n' % (i, self.effsig[i][0], self.effsig[i][1], self.effbg[i][x][0], self.effbg[i][x][1], self.nloose[i], self.ntight[i])
               # print i, '&', '[', self.effsig[i][0], round(self.effsig[i][1], 3), '] &', '[', round(self.effbg[i][x][0], 3), round(self.effbg[i][x][1], 3), '] &', self.nloose[i], '&', self.ntight[i], '\\\\'
            print ''
            content += '\\end{tabular} \n'
            content += '\\end{table} \n'
            content += '\n\n'
            content += '\\begin{table}[h] \n'
            content += '\\centering \n'
            content += '\\begin{tabular}{c|c|c|c|c} \n'
            content += 'isolation & TrueTight(sig) & TrueTight(qcd) & TrueLoose(sig) & TrueLoose(qcd) \\\\  \n'
            #print 'isolation & TrueTight(sig) & TrueTight(qcd) & TrueLoose(sig) & TrueLoose(qcd) \\\\  \n'
            for i in self.inputdirs:
                content += '%s & %d & %d & %d & %d \\\\ \n' % (i, int(self.nTtruesig[i]), int(self.nTtruebg[i]), int(self.nLtruesig[i]), int(self.nLtruebg[i]))
                #print i, '&', int(self.nTtruesig[i]), '&', int(self.nTtruebg[i]), '&', int(self.nLtruesig[i]), '&', int(self.nLtruebg[i]), '\\\\'

            content += '\\end{tabular} \n'
            content += '\\end{table} \n'
            content += '\n\n'
            content += '\\begin{table}[h] \n'
            content += '\\centering \n'
            content += '\\begin{tabular}{c|c|c|c|c} \n'
            content += 'isolation & $N_T(sig)$ & $N_T(qcd)$ & Rel(sig)in \%  & Rel(qcd)in \%\\\\ \n'
            #print 'isolation & $N_T(sig)$ & $N_T(qcd)$ & Rel(sig)in \%  & Rel(qcd)in \%\\\\ \n'
            for i in self.inputdirs:
                matrix = self.matrixMethod(self.effbg[i][x][0], self.effsig[i][0], self.nloose[i], self.ntight[i])
                relsig = (self.nTtruesig[i] - matrix['NST']) / self.nTtruesig[i] * 100
                relqcd = (self.nTtruebg[i] - matrix['NQT']) / self.nTtruebg[i] * 100
                content += '%s & %d & %d & %3.2f & %3.2f \\\\ \n' % (i, int(matrix['NST']), int(matrix['NQT']), relsig, relqcd)
                #print i, '&', int(matrix['NST']), '&', int(matrix['NQT']), '&', round(relsig, 2) , '&', round(relqcd, 2), '\\\\'
            content += '\\end{tabular} \n'
            content += '\\end{table} \n'
            content += '\clearpage'
        content += '\n \\end{document} \n'
        file = open(self.latexfile, 'w')
        file.write(content)
        file.close()
        os.system('pdflatex ' + self.latexfile)
        
    def applyToOthers(self, inputfiles, dir):
        qcdfile = TFile(inputfiles['qcd'])
        topfile = TFile(inputfiles['top'])
        qcdhist = 0;
        tophist = 0;
        #get all histograms: qcdafter, top after, mixed after/before
        self.mixa = qcdfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nMixedVSdiscTight')
        self.mixb = qcdfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nMixedVSdiscLoose')
        
        self.qcdhisttight = qcdfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscQCDTight')
        self.qcdhistloose = qcdfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscQCDLoose')
        
        self.tophisttight = topfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscSignalTight')
        self.tophistloose = topfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscSignalLoose')
        
        self.mixa.Add(self.tophisttight)
        self.mixb.Add(self.tophistloose)
        
        if self.mixa == None or self.mixb == None or self.tophistloose == None or self.tophisttight == None:
            print 'hist not found', dir, self.after, '/', self.hists['weighted']
        self.output.Cd('/')
        self.output.mkdir('test')
        self.output.cd('test')
        
        estimatedtop = self.tophisttight.Clone('estimatedNStight')
        estimatedqcd = self.qcdhisttight.Clone('estimatedNQCDtight')
        y = [self.effbg[dir][0.1][0], self.effbg[dir][0.2][0]]
        x = [0.05, 0.1]
        
        ret = self.gerade(y, x, 0)
        #effqcd = ret[0]
        # a*x + b
        formula = '%f*x + %f' % (ret[1], ret[2])
        fitgraph = TF1('qcdEffFit', formula, 0, 1)
        tmphist = self.tophisttight.Clone('MCTopeff')
        tmphist.Divide(self.tophistloose)
        
        tmphist2 = self.qcdhisttight.Clone('MCQCDeff')
        tmphist2.Divide(self.qcdhistloose)
        #test = TF1('fitfunc', 'exp(x)', 0, 1)
        #test = TF1("f1","expo",0,1)
        
        #tmphist2.Fit('f1','R')
        #tmphist2.Fit('exp(x)', '', '', 0, 1)
        #getFitFunction, lookup naming in CVS
        #print test.GetParameters()
        bin1 = self.mixa.GetBinContent(1)
        bin2 = self.mixa.GetBinContent(2)
       # self.qcdhisttight
        qbin1 = self.qcdhisttight.GetBinContent(1)
        qbin2 = self.qcdhisttight.GetBinContent(2)
        f1 = bin1 / qbin1
        f2 = bin2 / qbin2
        fa = (f1 + f2) / 2
        #self.qcdhisttight
        
        bin1 = self.mixb.GetBinContent(1)
        bin2 = self.mixb.GetBinContent(2)
       # self.qcdhistloose
        qbin1 = self.qcdhistloose.GetBinContent(1)
        qbin2 = self.qcdhistloose.GetBinContent(2)
        f1 = bin1 / qbin1
        f2 = bin2 / qbin2
        fb = (f1 + f2) / 2
        
        
        tmphist3 = self.qcdhisttight.Clone('QCDeff')
        tmphist3.Divide(self.qcdhistloose)
        
        
        for i in range(1, self.mixa.GetNbinsX() + 1):
            Nloose = self.mixb.GetBinContent(i)
            Ntight = self.mixa.GetBinContent(i)
            realns = self.tophisttight.GetBinContent(i)
           # print dir, self.effbg.keys()
            
            #estimatedtop.SetBinContent(i, mat['NST'])
            #dNWttLoose_deffqcd
            #tmp1 = after - self.effsig[dir][0] * before;
            #tmp2 = (self.effsig[dir][0]-self.effbg[dir][0.1][0]) *  (self.effsig[dir][0]-self.effbg[dir][0.1][0])
            #tmp3 = tmp1/tmp2
            
            effqcd = self.effbg[dir][0.4][0]
            #ret = self.gerade(y, x, estimatedtop.GetBinCenter(i))
            #effqcd = ret[0]
            #fitFunc.GetYValue(estimatedtop.GetBinCenter(i))
            #effqcd = (self.qcdhisttight.GetBinContent(i)*fa)/(self.qcdhistloose.GetBinContent(i)*fb)
            #effqcd = self.effbg[dir][0.1][0]
            effqcd2 = tmphist2.GetBinContent(i)
            effsig = tmphist.GetBinContent(i)
            effsigerr = self.effsig[dir][1]
            effqcderr = self.effbg[dir][0.1][1]
            
          #  mat = self.matrixMethod2(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight)
            mat = MatrixMethod.getNumbers(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight)
            print 'real top:%d , estimated: %d +- %d' % (realns, mat['NST'], mat['NSTerr'])
            print 'comp effQ: %1.4f, real: %1.4f' % (effqcd, effqcd2)
            estimatedtop.SetBinContent(i, mat['NST'])
            estimatedtop.SetBinError(i, mat['NSTerr'])
            estimatedqcd.SetBinContent(i, mat['NQT'])
            estimatedqcd.SetBinError(i, mat['NQTerr'])
            #print mat['NSTerr']
        fitgraph.Write()    
        tmphist2.Write()
        estimatedtop.Write()
        self.tophisttight.Write()
        self.qcdhisttight.Write()
        self.mixa.Write()
        tmphist3.Write()
        estimatedqcd.Write()
        tmphist.Write()
        #test.Write()
        
    def gerade(self, y, x, wert):
        a = (y[0] - y[1]) / (x[0] - x[1])
        b = y[1] - a * x[1]
        return [a * wert + b, a, b]
        
        #do matrix method for each bin, for each cut
        
        
        

if __name__ == '__main__':
    gROOT.Reset()
    inputfiles = {}
    inputfiles['qcd'] = "MM_qcdmu_calib.root"
    inputfiles['top'] = "MM_top_calib.root"
    inputfiles['wjets'] = "MM_wjets_calib.root"
    inputdirs = ["all", "jetIso", "calo", "track"]
    inputs = {}
    inputs['qcd'] = 'MM_qcdmu_validation.root'
    inputs['top'] = "MM_top_validation.root"
    inputs['wjets'] = "MM_wjets_validation.root"
    mac = Macro('complete_hists_MM.root', inputfiles, inputdirs)
    mac.debug = False
    mac.calcEff('qcd')
    mac.calcEff('top')
    mac.calcEff('wjets')
    mac.calcEff('mixed')
    mac.makeAditionalPlots()
    #mac.printEffSummary()
    mac.exportLatex()
    mac.applyToOthers(inputs, "all")
        