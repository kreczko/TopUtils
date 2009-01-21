from ROOT import gROOT, TCanvas, TH1F, TFile, TF1, TPad
from math import sqrt
import os
from MatrixMethod import MatrixMethod
from DrawHelper import Helper
import PadService as ps
from ConfigParser import *
from Timer import Timer
import copy
from analysisFiles import *

class Macro:
    outputdir = 'testing'
    plotdir = 'plots'
    texdir = 'tex'
    before = 'mbefore'
    after = 'mafter'
    debug = True
    #cuts = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0] 
    latexfile = 'Macro.tex'
    
    def __init__(self, outputfile, inputfiles, inputdirs):
        self.cuts = self.frange(0.1, 1.05, 0.05)
        self.plotsavefolder = self.outputdir + '/' + self.plotdir
        if not os.path.exists(self.outputdir):
            os.mkdir(self.outputdir)
        if not os.path.exists(self.plotsavefolder):
            os.mkdir(self.plotsavefolder)
        self.effsig = {}
        self.effwjet = {}
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
        self.hists['simple'] = 'mbg_nVSdiscSimple'
        
        self.inputdirs = inputdirs        
        
        self.output = TFile(outputfile, 'RECREATE')
        self.inputfiles = {}
        for i in inputfiles.keys():
            self.inputfiles[i] = TFile(inputfiles[i])
        
        self.createFolders('/', self.inputfiles.keys())
        self.output.mkdir('mixed')        
        self.output.mkdir('applied')

    def __del__(self):
#        print self.inputfiles
       # self.output.Close()
        for i in self.inputfiles.itervalues():
            if i:
                print i
                i.Close()
                
    def frange(self, start, end, inc):
        import math
        if end == None:
            end = start + 0.0
            start = 0.0
        else: 
            start += 0.0 # force it to be a float

        if inc == None:
            inc = 1.0
        count = int(math.ceil((end - start) / inc))

        L = [None,] * count

        L[0] = round(start, 2)
        for i in xrange(1,count):
            L[i] = L[i-1] + inc
        return L
            
    def calcEff(self, type, inputfiles, dir='all', outdir=''):
        if self.debug:
            print 'calculating efficiency for', type
        if type in ['qcd', 'top', 'wjets', 'mixed'] and (type in inputfiles.keys() or type == 'mixed'):
            #just to be sure:
            self.output.Cd('/')
            self.createFolders(type, [outdir])
            self.createFolders(type + '/' + outdir, [dir])
            #write old histogramms
            if not type=='mixed':
                self.doEff(type, inputfiles[type], dir, outdir)
            else:            
                self.doEffMixed(['qcd', 'top', 'wjets', 'mixed'], dir, outdir)
        else:
            print 'Error occured in calcEff()'
            
    def createFolders(self, parentfolder, folderlist):
        for i in folderlist:
                self.output.GetDirectory(parentfolder).mkdir(i)
            
    def makeMixed(self, inputfiles, dir):
        self.mixa = None
        self.mixb = None
        i = 0
        for x in inputfiles.itervalues():
            file = TFile(x)
            if (i == 0):
                self.mixa = copy.deepcopy(file.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight'))
                self.mixunwa = copy.deepcopy(file.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple'))
                self.mixb = copy.deepcopy(file.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose'))
                self.mixunwb = copy.deepcopy(file.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple'))
                if self.mixa == None or self.mixb == None or self.mixunwb == None or self.mixunwa == None:
                    print 'hist not found', dir, self.after, '/', self.hists['weighted']
            else:
                self.mixa.Add(file.Get(dir + self.after + '/' + self.hists['weighted']))
                self.mixunwa.Add(file.Get(dir + self.after + '/' + self.hists['simple']))
                self.mixb.Add(file.Get(dir + self.before + '/' + self.hists['weighted']))
                self.mixunwb.Add(file.Get(dir + self.before + '/' + self.hists['simple']))
            i += 1           
        if self.debug:
            print 'summed in', dir, 'over', i, 'hists'
            
    def doEffMixed(self, types, dir, outdir):   
        self.output.cd('mixed' + '/' + outdir + '/' +  dir)
        result = self.getEffHist(self.mixb, self.mixa, self.mixunwb)
        eff = result[0]
        eff = Helper.setHistLabels(eff, 'mva disc.', 'effieciency in %')
        self.mixa = Helper.setHistLabels(self.mixa, 'mva disc.', 'events')
        self.mixunwa = Helper.setHistLabels(self.mixunwa, 'mva disc.', 'unweigthed events')
        self.mixb = Helper.setHistLabels(self.mixb, 'mva disc.', 'events')
        self.mixunwb = Helper.setHistLabels(self.mixunwb, 'mva disc.', 'unweigthed events')
        eff = Helper.setMarker(eff, 4, 3, 1)
        #Helper.saveHist(eff, 'eff', 'testing/' + dir)
        eff.Write()            
        self.mixa.Write()
        self.mixunwa.Write()
        self.mixb.Write()
        self.mixunwb.Write()
        self.output.Cd('/')
        for effs in result[1]:
            if self.debug:
                print 'Eff for %s disc < %1.2f : %1.3f +- %1.3f' % (dir, effs[0], effs[1], effs[2])
            self.setEff('mixed', dir, round(effs[0], 2), effs[1], effs[2])     
                
    def setN(self, input, loose, tight):
        self.nloose[input] = int(loose)
        self.ntight[input] = int(tight)

    def setMCTruth(self, type, input, before, after):
        if type == 'top':
            self.nTtruesig[input] = after.Integral()
            self.nLtruesig[input] = before.Integral()
        else:
            self.nTtruebg[input] = after.Integral()
            self.nLtruebg[input] = before.Integral()
            
    def setEff(self, type, input, cut, eff, err):
        cut = '%1.2f' % cut
        #signal from monte carlo, bg from mixed sample control region
        if type == 'top':
            self.effsig[input] = [eff, err]
        elif type == 'wjets':
            self.effwjet[input] = [eff, err]
        elif type == 'mixed':
            self.effbg[input][cut] = [eff, err]
        
        
    def getEffHist(self, before, after, unweightedBefore):
        effplot = after.Clone("efficiency")
        effplot.Divide(before)
        #for performance, ROOT caching
        eGBC = effplot.GetBinContent
        eSBE = effplot.SetBinError
        uGBC = unweightedBefore.GetBinContent
        bGBC = before.GetBinContent
        aGBC = after.GetBinContent
        
        for i in range(1, effplot.GetNbinsX() + 1):
            eff = eGBC(i)
            err = self.getStatError(eff, uGBC(i))#sqrt((eff * (1 - eff)) / unweightedBefore.GetBinContent(i))
            if self.debug:
                print 'before: %d after: %d undw.: %d' % (bGBC(i), aGBC(i), uGBC(i))
                print '%1.3f +- %1.3f' % (eff, err)
            eSBE(i, err)
        effplot.SetMinimum(0)
        #cut based eff
        effs = []
        aGXAFB = after.GetXaxis().FindBin
        aI = after.Integral
        bI = before.Integral
        uI = unweightedBefore.Integral
        for x in self.cuts:
            bin = aGXAFB(x)
            na = aI(1, bin)
            nb = bI(1, bin)
            nunwb = uI(1, bin)
            eff = na / nb
            err = sqrt((eff * (1 - eff)) / nunwb)
            effs.append([x, eff, err])
        
        return [effplot, effs]
    
    def doEff(self, type, file, dir, outdir=''):
        file = TFile(file)
        fGet = file.Get
#        for i in self.inputdirs:
        after = fGet(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight')
        unwafter = fGet(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple')
        before = fGet(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose')
        unwbefore = fGet(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple')
        if after == None or before == None or unwbefore == None:
            print 'Histogram not found'
        else:
            self.setMCTruth(type, dir, before, after)
            self.output.cd(type + '/' + outdir + '/' +  dir)
            result = self.getEffHist(before, after, unwbefore)
            #caluclate MC Efficiency without binning
            mceff = after.Integral() / before.Integral()
            mcerr = self.getStatError(mceff,  unwbefore.Integral())#sqrt((mceff * (1 - mceff)) / unwbefore.Integral())
                
#                print 'MC Eff for', i, ':[', mceff, ',', mcerr, ']' 
            self.setEff(type, dir, -1, mceff, mcerr)
#                for effs in result[1]:          
#                    print 'Eff for %s disc < %1.2f : %1.3f +- %1.3f' % (i, effs[0], effs[1], effs[2])
#                    self.setEff(type, i,  effs[0], effs[1], effs[2])
            leg = Helper.makePlainLegend( 25 , 95, 70, 25)
            title1 = 'overall eff. %1.4f \pm %1.4f' % (mceff, mcerr)   
            leg.SetHeader( title1)
            leg = Helper.setLegendStyle(leg)
            eff = result[0]
            eff.SetTitle(title1)
#                eff = Helper.setHistLabels(eff, 'mva disc.', 'efficiency in %')
#                savefolder = self.plotsavefolder + '/' + i
#                Helper.saveHist(eff, 'eff_MC_'+ type, savefolder, leg)
            eff.Write()
#                leg.Write()
            after.Write()
            unwafter.Write()
            before.Write()
            unwbefore.Write()
            self.output.Cd('/')        
                    
    def applyToOthers(self, inputfiles, dir, outdir=''):
        files = {}
        tf = TFile
        for i in inputfiles.keys():
            files[i] =  tf(inputfiles[i])
        self.makeMixed(inputfiles, dir)
        qcdfile = files['qcd']
        topfile = files['top']
        wjetfile = files['wjets']
        self.qcdhisttight = None
        self.qcdhistloose = None
        self.qcdhisttight = qcdfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscQCDTight')
        self.qcdhistloose = qcdfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscQCDLoose')   
        
        self.tophisttight = topfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscSignalTight')
        self.tophistloose = topfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscSignalLoose')
        
        self.wjethisttight = wjetfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscWjetTight')
        self.wjethistloose = wjetfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscWjetLoose')
        
#        unw =  qcdfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose')   
        unw = topfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose')
        unw.Add(wjetfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose'))
        
        signalEff = self.tophisttight.Clone('signalEff')
        signalEff.Add(self.wjethisttight)
        sig = self.tophistloose.Clone('signal')
        sig.Add(self.wjethistloose)
        sigalla = signalEff.Integral()
        sigallb = sig.Integral()
        signalEff.Divide(sig)
        #topeff.Divide(self.tophistloose)
        tight = self.mixa.Integral() #+ self.qcdhisttight.Integral()
        loose = self.mixb.Integral() #+self.qcdhistloose.Integral()
#        print "loose, tight ", loose, tight
#        print "sLoose, sTight", sigallb, sigalla
#        print "Qloose, Qtight", self.qcdhistloose.Integral(), self.qcdhisttight.Integral()
        effsig = sigalla/sigallb
        effsigerr = self.getStatError(effsig, unw.Integral())
        reweights = {'qcd':1, 'top':1, 'wjets':1}
        
        self.output.cd('applied')
        completedir = 'applied/'
        if outdir:
            self.output.GetDirectory('applied').mkdir(outdir)
            self.output.GetDirectory('applied').GetDirectory(outdir).mkdir(dir)
            self.output.GetDirectory('applied').GetDirectory(outdir).GetDirectory(dir).mkdir('sig')
            self.output.GetDirectory('applied').GetDirectory(outdir).GetDirectory(dir).mkdir('bkg')
            completedir+= outdir + '/' + dir
            self.output.cd(completedir)
        else:
            self.output.GetDirectory('applied').mkdir(dir)
            self.output.GetDirectory('applied').GetDirectory(dir).mkdir('sig')
            self.output.GetDirectory('applied').GetDirectory(dir).mkdir('bkg')
            completedir+= dir 
            self.output.cd('applied/' + dir)
        qu = self.makeQualityHist(loose, tight,effsig, effsigerr, dir, reweights, completedir)
        # 0,                     1            2          3            4         5
        #[qualityBL, qualityBT,qualitySL, qualityST, effB, effQB]
        self.output.cd(completedir + '/bkg')
        qu[0].Write()
        qu[1].Write()
        qu[4].Write()
        qu[5].Write()
        self.output.Cd('/')
        self.output.cd(completedir + '/sig')
        qu[2].Write()
        qu[3].Write()
#        for i in qu:
#            i.Write()
        
        self.output.Cd('/')
        
    def makeQualityHist(self, loose, tight, effsig, effsigerr, dir, scale = {'qcd':1, 'top':1, 'wjets':1}, basedir=''): 
        keys = self.effbg[dir].keys()
        keys.sort()
#        f = ''
#        for i in basedir.split('/'):
#            f = f + i +  '/'
#            if not gROOT.GetDirectory(i):
#                gROOT.mkdir(i)
#                gROOT.cd(i)
#        gROOT.cd(f)
#        if basedir:
##            gROOT.mkdir(basedir)
##            gROOT.cd(basedir)
#            gROOT.mkdir('sig')
#            gROOT.mkdir('bkg')
##            gROOT.cd(basedir+ '/bkg')
#        else:
        gROOT.mkdir('sig')
        gROOT.mkdir('bkg')
        gROOT.cd('bkg')
        
        
        qualityBL = TH1F('quality_loose', 'quality_loose_BG', int(len(keys)), 0., eval(keys[len(keys)-1]))
        qualityBT = TH1F('quality_tight', 'quality_tight_BG', int(len(keys)), 0., eval(keys[len(keys)-1]))
        effB = TH1F('effvsDiscCut_BG', 'effvsDiscCut_BG', int(len(keys)), 0., eval(keys[len(keys)-1]))
        effQB = TH1F('quality_eff_BG', 'quality_eff_BG', int(len(keys)), 0., eval(keys[len(keys)-1]))
#        if basedir:
#            gROOT.cd(basedir+ '/bkg')
#        else:
        gROOT.cd('bkg')
        qualitySL = TH1F('quality_loose', 'quality_loose_SIG', int(len(keys)), 0., eval(keys[len(keys)-1]))
        qualityST = TH1F('quality_tight', 'quality_tight_SIG', int(len(keys)), 0., eval(keys[len(keys)-1]))
        
#        effS = TH1F('effvsDiscCut_SIG', 'effvsDiscCut_SIG', int(len(keys)), 0., eval(keys[len(keys)-1]))
        
        #caching of PyROOT methods (faster)
        qSBCBL = qualityBL.SetBinContent
        qSBEBL = qualityBL.SetBinError
        qSBCBT = qualityBT.SetBinContent
        qSBEBT = qualityBT.SetBinError        
        
        qSBCSL = qualitySL.SetBinContent
        qSBESL = qualitySL.SetBinError
        qSBCST = qualityST.SetBinContent
        qSBEST = qualityST.SetBinError    
        
        eSBCB = effB.SetBinContent
        eSBEB = effB.SetBinError
        eSBCQ = effQB.SetBinContent
        eSBEQ = effQB.SetBinError
        
        wjetsL = self.wjethistloose.Integral() * scale['wjets']
        wjetsT = self.wjethisttight.Integral() * scale['wjets']
        topL = self.tophistloose.Integral() * scale['top']
        topT = self.tophisttight.Integral() * scale['top']
        qcdL = self.qcdhistloose.Integral() * scale['qcd']
        qcdT = self.qcdhisttight.Integral() * scale['qcd']
        sigT = wjetsT + topT
        sigL = wjetsL + topL
        trueQCDEff = qcdT/qcdL
        trueQCDEffErr = self.getStatError(trueQCDEff, qcdT + qcdL)
#        eSBCS = effS.SetBinContent
#        eSBES = effS.SetBinError
        x = 1
        for i in keys:  
            effqcd = self.effbg[dir][i][0]
            effqcderr = self.effbg[dir][i][1]
#            effsig = (self.effsig[dir][i][0] + self.effwjet[dir][i][0])/2
#            effsigerr = sqrt(self.effsig[dir][i][1]*self.effsig[dir][i][1] + self.effwjet[dir][i][1]*self.effwjet[dir][i][1])/2
            eSBCB(x,effqcd)
            eSBEB(x,effqcderr)
            res = MatrixMethod.getNumbers(effqcd, effqcderr, sigT/sigL, effsigerr, loose, tight)
            sigma =  (res['NQT'])/qcdT
            sigmaErr = res['NQTerr']/qcdT
            sigmaS = res['NST']/sigT
            sigmaErrS= res['NSTerr']/sigT
            
            if self.debug:
                print 'overall'
                print 'T est: ', res['NQT'], 'pm',res['NQTerr'], 'real: ', qcdT
                print 'T est: ', res['NST'], 'pm',res['NSTerr'], 'real: ', sigT
                print effqcd, effqcderr
                print effsig, effsigerr
                print i, sigma, '+-',sigmaErr 
                print i, sigmaS, '+-',sigmaErrS
            
            
            err = sqrt(trueQCDEffErr*trueQCDEffErr + effqcderr*effqcderr)
            eSBCQ(x, effqcd - trueQCDEff)
            eSBEQ(x,err)
            
            qSBCBT(x, sigma)
            qSBEBT(x, sigmaErr)
            qSBCST(x, sigmaS)
            qSBEST(x, sigmaErrS)
            
            
            sigma =  (res['NQ'])/qcdL
            sigmaErr = res['NQerr']/qcdL
            sigmaS = res['NS']/sigL
            sigmaErrS= res['NSerr']/sigL
            if self.debug:
                print 'L est: ', res['NQ'], 'pm',res['NQerr'], 'real: ', qcdL
                print 'L est: ', res['NS'], 'pm',res['NSerr'], 'real: ', sigL
                print i, sigma, '+-',sigmaErr
                print i, sigmaS, '+-',sigmaErrS 
#            
            qSBCBL(x, sigma)
            qSBEBL(x, sigmaErr)
            
            qSBCSL(x, sigmaS)
            qSBESL(x, sigmaErrS)
            x += 1
#        qualityL = Helper.setMarker(qualityL,4,3,1)
#        qualityL = Helper.setHistLabels(qualityL, 'mva disc. cut', 'N_{est}/N_{MC}')
#        qualityT = Helper.setMarker(qualityT,4,3,1)
#        qualityT = Helper.setHistLabels(qualityT, 'mva disc. cut', 'N_{est}/N_{MC}')
#        eff = Helper.setMarker(eff,4,3,1)
#        eff = Helper.setHistLabels(eff, 'mva disc. cut', 'efficiency')
#        savedir = self.plotsavefolder + '/' + dir
#        Helper.saveHist(qualityL, "quality_loose_qcd", savedir)
#        Helper.saveHist(qualityT, "quality_tight_qcd", savedir)
#        Helper.saveHist(eff, "eff_qcd", savedir)
        return [qualityBL, qualityBT,qualitySL, qualityST, effB, effQB]
            
            
    def makeEstimationHist(self, effqcd, effqcderr, sigEffHist, sigLooseHist, looseHist, tightHist):
        looseGBC = looseHist.GetBinContent
        tightGBC = tightHist.GetBinContent
        sigEffGBC = sigEffHist.GetBinContent
        #unweighted
        sigLGBC = sigLooseHist.GetBinContent
        for x in range(1, signalEff.GetNbinsX() + 1):
                tight = tightGBC(x)
                loose = looseGBC(x)
                effsig = sigEffGBC(x)
                effsigerr = self.getStatError(effsig, sigEffGBC(x))
                res = MatrixMethod.getNumbers(effqcd, effqcderr, effsig, effsigerr, loose, tight)
                print 'est: ', res['NQT'], 'pm',res['NQTerr'], 'real: ', self.qcdhisttight.GetBinContent(x)
                
    def getStatError(self ,eff, unweighted):
        if unweighted > 0:
            err = sqrt((eff * (1 - eff)) / unweighted)
        else:
            err = 0
        return err
    
    
    def saveReferencePlots(self, outputdir, files):
        save = '/playground/ThesisPlots/reference/%s/' % outputdir 
        f = open('LKThesis.xml')
        cfg = f.read()
        f.close()
        cfg = cfg.replace('{$QCD}', files['qcd'])
        cfg = cfg.replace('{$TOP}',  files['top'])
        cfg = cfg.replace('{$WJETS}',  files['wjets'])
        self.savePlotsFromCfg(cfg, save)
        
    def saveMacroPlots(self, outputdir, file):
        save = '/playground/ThesisPlots/macro/%s/' % outputdir 
        f = open('LKMacro.xml')
        cfg = f.read()
        f.close()
        cfg = cfg.replace('{$macro}', file)
        self.savePlotsFromCfg(cfg, save)
        
        
    def savePlotsFromCfg(self,cfg, savedir):
        f = open('tmp.xml', 'w')
        f.write(cfg)
        f.close()
        
        cp = ConfigParser('tmp.xml')
        plots = cp.getPlots()
        histlist = plots.histlist
        for hist in histlist:
            list = []
            legend = hist.legend
            h = None
            leg = Helper.makePlainLegend(legend.opt['posX'], legend.opt['posY'], legend.opt['sizeX'],legend.opt['sizeX'])
            for var in hist.varlist:
                
                file = var.rootsource
                h1 =  var.hist
                if var.opt['operation'] == 'none':
                    f = TFile(file)
                    h = f.Get(h1)
                else:
                    integ = 0
                    for xxx in var.rootsource.split(Variable.entryDelimiter):
                        f = TFile(xxx)
                        if integ ==0:
                            h = copy.deepcopy(f.Get(h1).Clone())
                        else:
                            #only add so far
                            h.Add(f.Get(h1))
                        integ += 1
                
                h = Helper.applyHistConfig(h, hist, var)
                if var.opt['draw'] == '1':
                    list.append(copy.deepcopy(h.Clone()))
                # Helper.saveHist(h, "quality_loose_qcd", 'testing/plots/else')
            log = [hist.opt['logX'], hist.opt['logY']]
            Helper.saveHistsInOne(list,hist.opt['name'], savedir + hist.opt['savefolder'], leg, log)        
        os.remove('tmp.xml')
        
    def makeCutDependencyPlots(self, inputdirs, varcut):
        keys = qcdCfilesJ.keys()
        keys.sort()
        for i in keys:
            filesC = {'qcd' : qcdCfilesJ[i], 'top' : topCfilesJ[i], 'wjets' : wjetsCfilesJ[i]}
            filesV = {'qcd' : qcdVfilesJ[i], 'top' : topVfilesJ[i], 'wjets' : wjetsVfilesJ[i]}
        
            outputdir = 'jetcut_%s' % i
            for dir in inputdirs:
                self.makeMixed(filesC, dir)
                self.setN(dir, self.mixb.Integral(), self.mixa.Integral())
                for type in ['qcd', 'top', 'wjets', 'mixed']:
                    self.calcEff(type, filesC, dir, outputdir)
                self.applyToOthers(filesV, dir, outputdir)
        
        keys = qcdCfilesM.keys()
        keys.sort()
        for i in keys:
#            files = {'qcd' : qcdCfilesM[i], 'top' : topCfilesM[i], 'wjets' : wjetsCfilesM[i]}
            filesC = {'qcd' : qcdCfilesM[i], 'top' : topCfilesM[i], 'wjets' : wjetsCfilesM[i]}
            filesV = {'qcd' : qcdVfilesM[i], 'top' : topVfilesM[i], 'wjets' : wjetsVfilesM[i]}
            outputdir = 'muoncut_%s' % i
            for dir in inputdirs:
                self.makeMixed(filesC, dir)
                self.setN(dir, self.mixb.Integral(), self.mixa.Integral())
                for type in ['qcd', 'top', 'wjets', 'mixed']:
                    self.calcEff(type, filesC, dir, outputdir)
                self.applyToOthers(filesV, dir, outputdir)
        
        
    def saveAllPlots(self):
        keys = qcdCfilesJ.keys()
        keys.sort()
        for i in keys:
            files = {'qcd' : qcdCfilesJ[i], 'top' : topCfilesJ[i], 'wjets' : wjetsCfilesJ[i]}
            outputdir = 'jetcut_%s' % i
            self.saveReferencePlots(outputdir, files)
        
        keys = qcdCfilesM.keys()
        keys.sort()
        for i in keys:
            files = {'qcd' : qcdCfilesM[i], 'top' : topCfilesM[i], 'wjets' : wjetsCfilesM[i]}
            outputdir = 'muoncut_%s' % i
            self.saveReferencePlots(outputdir, files)
        
        self.saveMacroPlots("", self.output.GetName())

if __name__ == '__main__':
    gROOT.Reset()
    #for white background
    gROOT.SetStyle("Plain")
    gROOT.SetBatch(True)
    inputfiles = {}
    folder = '/playground/rootfiles/FINAL/'
    inputfiles['qcd'] = folder + "MM_qcdmu_calib_final_20j20m.root"
    inputfiles['top'] = folder + "MM_top_calib_final_20j20m.root"
    inputfiles['wjets'] =folder + "MM_wjets_calib_final_20j20m.root"
    inputdirs = ["all", "jetIso", "calo", "track"]
    inputs = {}
    inputs['qcd'] = folder + 'MM_qcdmu_valid_final_20j20m.root'
    inputs['top'] =folder +  "MM_top_valid_final_20j20m.root"
    inputs['wjets'] = folder + "MM_wjets_valid_final_20j20m.root"
    mac = Macro(folder + 'complete_hists_MM.root', inputfiles, inputdirs)
    mac.debug = False
#    mac.calcEff('qcd')
#    mac.calcEff('top')
#    mac.calcEff('wjets')
#    mac.calcEff('mixed')
   # mac.applyToOthers(inputs, "track")
    mac.makeCutDependencyPlots(inputdirs, 0.3)
#    for i in inputdirs:
#        mac.applyToOthers(inputs, i)
    mac.output.Close()
    mac.saveAllPlots()