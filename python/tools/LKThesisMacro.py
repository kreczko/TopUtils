from ROOT import gROOT, TCanvas, TH1F, TFile, TF1, TPad, TGaxis, TGraph
from math import sqrt
import os
from MatrixMethod import MatrixMethod
from DrawHelper import Helper
import PadService as ps
#from ConfigParser import *
from BasicPlotter import Plotter
from Timer import Timer
import copy
from analysisFiles import *
from array import array

omg = {}

class Macro:
    scale = {'qcd':1, 'wjets':1, 'top':1}
    outputdir = 'testing'
    plotdir = 'plots'
    texdir = 'tex'
    before = 'mbefore'
    after = 'mafter'
    debug = True
    #cuts = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0] 
    latexfile = 'Macro.tex'
    
    def __init__(self, outputfile, inputdirs, var='MVA'):
        self.tables = ''
        self.hists = {}
        self.var = var
        
        if self.var == 'MVA':
            self.cuts = self.frange(0.05, 1.05, 0.05)
            self.hists['weighted'] = 'mbg_nVSdisc'
            self.hists['simple'] = 'mbg_nVSdiscSimple'
        elif self.var == 'met':
            self.cuts = [20., 40., 60., 100., 150., 300.]
            self.hists['weighted'] = 'mbg_nVSmet'
            self.hists['simple'] = 'mbg_nVSmetSimple'
        else:
            print 'error'
        self.plotsavefolder = self.outputdir + '/' + self.plotdir
        if not os.path.exists(self.outputdir):
            os.mkdir(self.outputdir)
        if not os.path.exists(self.plotsavefolder):
            os.mkdir(self.plotsavefolder)
        self.effsig = {}
        self.effwjet = {}
        self.effqcd = {}
        self.effbg = {}
        self.nloose = {}
        self.ntight = {}
        
        self.nTtruebg = {}
        self.nTtruebg = {}
        self.nTtruesig = {}
        self.nLtruebg = {}
        self.nLtruesig = {}
        
        for i in inputdirs:
            self.effbg[i] = {}
        for k in shapeFiles.keys():
            self.effbg[k] = {}
        
        self.inputdirs = inputdirs        
        
        self.output = TFile(outputfile, 'RECREATE')
#        self.inputfiles = {}
#        for i in inputfiles.keys():
#            self.inputfiles[i] = TFile(inputfiles[i])
        
        self.createFolders('/', ["top", "qcd", "wjets"])
        self.output.mkdir('mixed')        
        self.output.mkdir('applied')
        self.output.mkdir('shapes')
        self.createFolders('shapes', shapeFiles.keys())
#        for i in shapeFiles.keys():
#            self.createFolders('shapes/'+i, inputdirs)

#    def __del__(self):
#        print self.inputfiles
       # self.output.Close()
#        for i in self.inputfiles.itervalues():
#            if i:
#                print i
#                i.Close()
                
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

        L = [None, ] * count

        L[0] = round(start, 2)
        for i in xrange(1, count):
            L[i] = L[i - 1] + inc
        return L
            
    def calcEff(self, type, inputfiles, dir='all', outdir=''):
#        if self.debug:
#            print 'calculating efficiency for', type
        if type in ['qcd', 'top', 'wjets', 'mixed', "shapes"]:
            #just to be sure:
            self.output.Cd('/')
            self.createFolders(type, [outdir])
            self.createFolders(type + '/' + outdir, [dir])
            #write old histogramms
            if not type == 'mixed' and not  type== "shapes":
                self.doEff(type, inputfiles[type], dir, outdir)
            elif type== "shapes":
                print "shapes"
            else:            
                self.doEffMixed(['qcd', 'top', 'wjets', 'mixed'], dir, outdir)
        else:
            print 'Error occured in calcEff()'

    def doEffShapes(self):
        print "shapes"
    def createFolders(self, parentfolder, folderlist):
        for i in folderlist:
            if not self.output.GetDirectory(parentfolder).GetDirectory(i):
                self.output.GetDirectory(parentfolder).mkdir(i)
            
    def makeMixed(self, inputfiles, dir):
        scale = self.scale
        self.mixa = None
        self.mixb = None
        i = 0
        for k,x in inputfiles.iteritems():
            file = TFile(x)
            if (i == 0):
                self.mixa = copy.deepcopy(file.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscTight'))
                self.mixunwa = copy.deepcopy(file.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscTightSimple'))
                self.mixb = copy.deepcopy(file.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscLoose'))
                self.mixunwb = copy.deepcopy(file.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscLooseSimple'))
                
                self.mixa.Scale(scale[k])
                self.mixunwa.Scale(scale[k])
                self.mixb.Scale(scale[k])
                self.mixunwb.Scale(scale[k])
                if self.mixa == None or self.mixb == None or self.mixunwb == None or self.mixunwa == None:
                    print 'hist not found', dir, self.after, '/', self.hists['weighted']
            else:
                a = file.Get(dir + self.after + '/' + self.hists['weighted'])
                wa = file.Get(dir + self.after + '/' + self.hists['simple'])
                b = file.Get(dir + self.before + '/' + self.hists['weighted'])
                wb = file.Get(dir + self.before + '/' + self.hists['simple'])
                
                a.Scale(scale[k])
                wa.Scale(scale[k])
                b.Scale(scale[k])
                wb.Scale(scale[k])
                
                self.mixa.Add(a)
                self.mixunwa.Add(wa)
                self.mixb.Add(b)
                self.mixunwb.Add(wb)
            i += 1           
#        if self.debug:
#            print 'summed in', dir, 'over', i, 'hists'
            
    def doEffMixed(self, types, dir, outdir):   
        if not self.output.GetDirectory('mixed').GetDirectory(outdir):
            self.output.GetDirectory("mixed").mkdir(outdir)
        if not self.output.GetDirectory('mixed').GetDirectory(outdir).GetDirectory(dir):
            self.output.GetDirectory('mixed').GetDirectory(outdir).mkdir(dir)
        self.output.cd('mixed' + '/' + outdir + '/' + dir)
        result = self.getEffHist(self.mixb, self.mixa, self.mixunwb)
        eff = result[0]
        if self.var == 'MVA':
            eff = Helper.setHistLabels(eff, 'MVA disc.', 'effieciency in %')
            self.mixa = Helper.setHistLabels(self.mixa, 'MVA disc.', 'events')
            self.mixunwa = Helper.setHistLabels(self.mixunwa, 'MVA disc.', 'unweigthed events')
            self.mixb = Helper.setHistLabels(self.mixb, 'MVA disc.', 'events')
            self.mixunwb = Helper.setHistLabels(self.mixunwb, 'MVA disc.', 'unweigthed events')
        elif self.var == 'met':
            eff = Helper.setHistLabels(eff, 'missing E_{T} [GeV]', 'effieciency in %')
            self.mixa = Helper.setHistLabels(self.mixa, 'missing E_{T} [GeV]', 'events')
            self.mixunwa = Helper.setHistLabels(self.mixunwa, 'missing E_{T} [GeV]', 'unweigthed events')
            self.mixb = Helper.setHistLabels(self.mixb, 'missing E_{T} [GeV]', 'events')
            self.mixunwb = Helper.setHistLabels(self.mixunwb, 'missing E_{T} [GeV]', 'unweigthed events')
        eff = Helper.setMarker(eff, 4, 3, 1)
        eff.Write()            
        self.mixa.Write()
        self.mixunwa.Write()
        self.mixb.Write()
        self.mixunwb.Write()
        self.output.Cd('/')
        for effs in result[1]:
#            if self.debug:
#                print 'Eff for %s disc < %1.2f : %1.3f +- %1.3f' % (dir, effs[0], effs[1], effs[2])
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
        elif type == 'qcd':
            self.effqcd[input] = [eff, err]
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
#            if self.debug:
#                print 'before: %d after: %d undw.: %d' % (bGBC(i), aGBC(i), uGBC(i))
#                print '%1.3f +- %1.3f' % (eff, err)
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
            self.output.cd(type + '/' + outdir + '/' + dir)
            result = self.getEffHist(before, after, unwbefore)
            #caluclate MC Efficiency without binning
            mceff = after.Integral() / before.Integral()
            mcerr = self.getStatError(mceff, unwbefore.Integral())#sqrt((mceff * (1 - mceff)) / unwbefore.Integral())
                
#                print 'MC Eff for', i, ':[', mceff, ',', mcerr, ']' 
            self.setEff(type, dir, - 1, mceff, mcerr)
#                for effs in result[1]:          
#                    print 'Eff for %s disc < %1.2f : %1.3f +- %1.3f' % (i, effs[0], effs[1], effs[2])
#                    self.setEff(type, i,  effs[0], effs[1], effs[2])
            leg = Helper.makePlainLegend(25 , 95, 70, 25)
            title1 = 'overall eff. %1.4f \pm %1.4f' % (mceff, mcerr)   
            leg.SetHeader(title1)
            leg = Helper.setLegendStyle(leg)
            eff = result[0]
            eff.SetTitle(title1)
            eff.Write()
            after.Scale(self.scale[type])
            before.Scale(self.scale[type])
            after.Write()
            unwafter.Write()
            before.Write()
            unwbefore.Write()
            self.output.Cd('/')        
                    
    def applyToOthers(self, inputfiles, dir, outdir=''):
        files = {}
        tf = TFile
        for i in inputfiles.keys():
            files[i] = tf(inputfiles[i])
#        self.makeMixed(inputfiles, dir)
        qcdfile = files['qcd']
        topfile = files['top']
        wjetfile = files['wjets']
        self.qcdhisttight = None
        self.qcdhistloose = None
        self.qcdhisttight = qcdfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscQCDTight')
        self.qcdhistloose = qcdfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscQCDLoose')   
        qcdloose = qcdfile.Get(dir + self.before + '/mbg_nVSmet' ).Clone('nVSmetQCDLoose')
        qcdtight  = qcdfile.Get(dir + self.after + '/mbg_nVSmet' ).Clone('nVSmetQCDLoose') 
        
        self.tophisttight = topfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscSignalTight')
        self.tophistloose = topfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscSignalLoose')
        toploose = topfile.Get(dir + self.before + '/mbg_nVSmet' ).Clone('nVSmettopLoose')
        toptight  = topfile.Get(dir + self.after + '/mbg_nVSmet' ).Clone('nVSmettopLoose') 
        
        self.wjethisttight = wjetfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscWjetTight')
        self.wjethistloose = wjetfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscWjetLoose')
        wjetloose = wjetfile.Get(dir + self.before + '/mbg_nVSmet' ).Clone('nVSmetwjetLoose')
        wjettight  = wjetfile.Get(dir + self.after + '/mbg_nVSmet' ).Clone('nVSmetwjetLoose') 
        
#        unw =  qcdfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose')   
        unw = topfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose')
        unw.Add(wjetfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose'))
        
        signalEff = self.tophisttight.Clone('signalEff')
        signalEff.Add(self.wjethisttight)
        sig = self.tophistloose.Clone('signal')
        sig.Add(self.wjethistloose)
        
        sbgT = signalEff.Clone('sgbT')
        sbgL = sig.Clone('sgbL')
        sbgL.Add(self.qcdhistloose)
        sbgT.Add(self.qcdhisttight)
        backroundoverSignalB = self.qcdhistloose.Clone("bgOverbgs_loose")
        backroundoverSignalA = self.qcdhisttight.Clone("bgOverbgs_tight")
        backroundoverSignalA.Divide(sbgT)
        backroundoverSignalB.Divide(sbgL)
        
#===============================================================================
#        MET
#===============================================================================
        METsignalEff = toptight.Clone('MsignalEff')
        METsignalEff.Add(wjettight)
        METsig = toploose.Clone('Msignal')
        METsig.Add(wjetloose)
        
        msbgT = METsignalEff.Clone('sgbTM')
        msbgL = METsig.Clone('sgbLM')
        msbgL.Add(qcdloose)
        msbgT.Add(qcdtight)
        mbackroundoverSignalB = qcdloose.Clone("bgOverbgs_looseMET")
        mbackroundoverSignalA = qcdtight.Clone("bgOverbgs_tightMET")
        mbackroundoverSignalA.Divide(msbgT)
        mbackroundoverSignalB.Divide(msbgL)
        
        
        
        sigalla = signalEff.Integral()
        sigallb = sig.Integral()
        signalEff.Divide(sig)
        
        tight = self.mixa.Integral()  #+ qcdA + wA + tA
        loose = self.mixb.Integral()# +qcdB + wB + tB#+self.qcdhistloose.Integral()
        effsig = sigalla / sigallb
        effsigerr = self.getStatError(effsig, unw.Integral())
        
        
        self.output.cd('applied')
        completedir = 'applied/'
        if outdir:
            if not self.output.GetDirectory('applied').GetDirectory(outdir):
                self.output.GetDirectory('applied').mkdir(outdir)
            if not self.output.GetDirectory('applied').GetDirectory(outdir).GetDirectory(dir):
                self.output.GetDirectory('applied').GetDirectory(outdir).mkdir(dir)
            if not self.output.GetDirectory('applied').GetDirectory(outdir).GetDirectory(dir).GetDirectory('sig'):
                self.output.GetDirectory('applied').GetDirectory(outdir).GetDirectory(dir).mkdir('sig')
            if not self.output.GetDirectory('applied').GetDirectory(outdir).GetDirectory(dir).GetDirectory('bkg'):
                self.output.GetDirectory('applied').GetDirectory(outdir).GetDirectory(dir).mkdir('bkg')
            completedir += outdir + '/' + dir
            self.output.cd(completedir)
        else:
            self.output.GetDirectory('applied').mkdir(dir)
            self.output.GetDirectory('applied').GetDirectory(dir).mkdir('sig')
            self.output.GetDirectory('applied').GetDirectory(dir).mkdir('bkg')
            completedir += dir 
            self.output.cd('applied/' + dir)
        qu = self.makeQualityHist(loose, tight, effsig, effsigerr, dir, completedir)
        # 0,                     1            2          3            4         5
        #[qualityBL, qualityBT,qualitySL, qualityST, effB, effQB]
        self.output.cd(completedir + '/bkg')
        qu[0].Write()
        qu[1].Write()
        qu[4].Write()
        qu[5].Write()
        qu[6].Write()
        qu[7].Write()
        qu[8].Write()
        backroundoverSignalA.Write()
        backroundoverSignalB.Write()
        mbackroundoverSignalA.Write()
        mbackroundoverSignalB.Write()
        self.output.Cd('/')
        self.output.cd(completedir + '/sig')
        qu[2].Write()
        qu[3].Write()
        signalEff.Write()
#        for i in qu:
#            i.Write()
        self.output.Cd('/')
        return qu
        
    def makeQualityHist(self, loose, tight, effsig, effsigerr, dir, basedir=''): 
        scale = self.scale
        keys = self.effbg[dir].keys()
        keys.sort()
        
        t2 = basedir + dir + 'bkg'
        t2 = t2.replace('/', '_')
        gROOT.mkdir(t2)
        gROOT.cd(t2)
        
        qualityBL = None
        qualityBT = None
        effB = None
        effQB = None
        effQCDmcVSCUT = copy.deepcopy(self.qcdhisttight.Clone('effMC_vs_MVAcut'))
        EQMSBC = effQCDmcVSCUT.SetBinContent
        EQMSBE = effQCDmcVSCUT.SetBinError
    
        qualityBL = copy.deepcopy(self.qcdhistloose.Clone('quality_loose'))
        qualityBL.SetName('quality_loose')
        qualityBL.SetTitle('quality_loose_BG')
        
        NvsCL = copy.deepcopy(self.qcdhistloose.Clone('NvsCutloose'))
        NvsCL.SetName('NvsCutloose')
        NvsCL.SetTitle('NvsCutloose')
        
        NvsCT= copy.deepcopy(self.qcdhistloose.Clone('NvsCuttight'))
        NvsCT.SetName('NvsCuttight')
        NvsCT.SetTitle('NvsCuttight')
            
        qualityBT = copy.deepcopy(self.qcdhistloose.Clone('quality_tight'))
        qualityBT.SetName('quality_tight')
        qualityBT.SetTitle('quality_tight_BG')
            
        effB = copy.deepcopy(self.qcdhistloose.Clone('effvsDiscCut_BG'))
        effB.SetName('effvsDiscCut_BG')
        effB.SetTitle('effvsDiscCut_BG')
            
        effQB = copy.deepcopy(self.qcdhistloose.Clone('quality_eff_BG'))
        effQB.SetName('quality_eff_BG')
        effQB.SetTitle('quality_eff_BG')
#            TH1F('quality_loose', 'quality_loose_BG', int(len(keys)), 0., eval(keys[len(keys) - 1]))
#            qualityBT = TH1F('quality_tight', 'quality_tight_BG', int(len(keys)), 0., eval(keys[len(keys) - 1]))
#            effB = TH1F('effvsDiscCut_BG', 'effvsDiscCut_BG', int(len(keys)), 0., eval(keys[len(keys) - 1]))
#            effQB = TH1F('quality_eff_BG', 'quality_eff_BG', int(len(keys)), 0., eval(keys[len(keys) - 1]))

        t1 = basedir + dir + 'sig'
        t1 = t1.replace('/', '_')
        gROOT.mkdir(t1)
        gROOT.cd(t1)
        
        qualitySL = copy.deepcopy(self.tophistloose.Clone('quality_loose_SIG'))
        qualitySL.SetName('quality_loose')
        qualitySL.SetTitle('quality_loose_SIG')
        
        qualityST = copy.deepcopy(self.tophistloose.Clone('quality_tight_SIG'))
        qualityST.SetName('quality_tight')
        qualityST.SetTitle('quality_tight_SIG')
        #TH1F('quality_loose', 'quality_loose_SIG', int(len(keys)), 0., eval(keys[len(keys) - 1]))
#        qualityST = TH1F('quality_tight', 'quality_tight_SIG', int(len(keys)), 0., eval(keys[len(keys) - 1]))
        
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
        
        nLSBC = NvsCL.SetBinContent
        nTSBC = NvsCT.SetBinContent
        
        wjetsL = self.wjethistloose.Integral() * scale['wjets']
        wjetsT = self.wjethisttight.Integral() * scale['wjets']
        topL = self.tophistloose.Integral() * scale['top']
        topT = self.tophisttight.Integral() * scale['top']
        qcdL = self.qcdhistloose.Integral() * scale['qcd']
        qcdT = self.qcdhisttight.Integral() * scale['qcd']
        sigT = wjetsT + topT
        sigL = wjetsL + topL
        trueQCDEff = qcdT / qcdL
        trueQCDEffErr = self.getStatError(trueQCDEff, qcdT + qcdL)
#        eSBCS = effS.SetBinContent
#        eSBES = effS.SetBinError
        x = 1
        for i in keys:  
            
            effqcd = self.effbg[dir][i][0]
            effqcderr = self.effbg[dir][i][1]
            
            mixaSel, mixbSel, qcdTSel, qcdLSel = [1,1,1,1]
            if x ==1:
                mixaSel = self.mixa.GetBinContent(x)
                mixbSel = self.mixb.GetBinContent(x)
                qcdTSel = self.qcdhisttight.GetBinContent(x)
                qcdLSel = self.qcdhistloose.GetBinContent(x)
            else:
                mixaSel = self.mixa.Integral(1, x)
                mixbSel = self.mixb.Integral(1, x)
                qcdTSel = self.qcdhisttight.Integral(1, x)
                qcdLSel = self.qcdhistloose.Integral(1, x)
#            if qcdT == 0:
#                nTSBC(x,0)
#            else:
            nTSBC(x,qcdTSel/qcdT)
#            if qcdLSel == 0:
#                nLSBC(x,0)
#            else:
            nLSBC(x,qcdLSel/qcdL)
            
            
#            effsig = (self.effsig[dir][i][0] + self.effwjet[dir][i][0])/2
#            effsigerr = sqrt(self.effsig[dir][i][1]*self.effsig[dir][i][1] + self.effwjet[dir][i][1]*self.effwjet[dir][i][1])/2
            eSBCB(x, effqcd)
            eSBEB(x, effqcderr)
            res = MatrixMethod.getNumbers(effqcd, effqcderr, sigT / sigL, effsigerr, loose, tight)
            sigma = (res['NQT']) / qcdT
            sigmaErr = res['NQTerr'] / qcdT
            sigmaS = res['NST'] / sigT
            sigmaErrS = res['NSTerr'] / sigT
            
            if dir =='all' and i == '0.20':
                tab = 'From %s and %s \n'%(dir, basedir)
                tab += '$N_L = %d, N_T = %d$ \\\\ \n' % (loose, tight)
                tab += 'cut %s, $\epsilon_q = %1.4f \\pm %1.4f$, $\epsilon_s = %1.4f \\pm %1.4f$ \n' %(i, effqcd, effqcderr, sigT / sigL, effsigerr) 
                tab += '\\begin{table}[!ht] \n'
                tab += '\\begin{tabular}{l c c c c} \n'
                tab += '\\toprule'+'\n'
                tab += ' & $N_q$ & $N_q^{MC}$ & $N_s$ & $N_s^{MC}$\\\\ \n'
                tab += '\\midrule \n'
                tab += 'loose & $%d \\pm %d$ & %d & $%d \\pm %d$ & \\num{%d} \\\\ \n' %(res['NQ'], res['NQerr'], qcdL, res['NS'], res['NSerr'], sigL)
                tab += 'tight  & $%d \\pm %d$ & %d & $%d \\pm %d$ & \\num{%d} \\\\ \n' %(res['NQT'], res['NQTerr'], qcdT, res['NST'], res['NSTerr'], sigT)
                tab += '\\bottomrule \n'
                tab += '\\end{tabular} \n'
                tab += '\\end{table} \n'
                self.addToTables(tab)
            if self.debug  and dir == 'all':
                print 'overall'
                print 'T est: ', res['NQT'], 'pm', res['NQTerr'], 'real: ', qcdT
                print 'T est: ', res['NST'], 'pm', res['NSTerr'], 'real: ', sigT
                print effqcd, effqcderr
                print effsig, effsigerr
                print i, sigma, '+-', sigmaErr 
                print i, sigmaS, '+-', sigmaErrS
            
            EQMSBC(x, qcdTSel/qcdLSel)
            EQMSBE(x, self.getStatError(qcdTSel/qcdLSel, qcdLSel/5))
            err = sqrt(trueQCDEffErr * trueQCDEffErr + effqcderr * effqcderr)
            eSBCQ(x, effqcd/trueQCDEff)
            eSBEQ(x, err)
            
            qSBCBT(x, sigma)
            qSBEBT(x, sigmaErr)
            qSBCST(x, sigmaS)
            qSBEST(x, sigmaErrS)
            
            
            sigma = (res['NQ']) / qcdL
            sigmaErr = res['NQerr'] / qcdL
            sigmaS = res['NS'] / sigL
            sigmaErrS = res['NSerr'] / sigL
            
            if self.debug and dir == 'all':
                print 'L est: ', res['NQ'], 'pm', res['NQerr'], 'real: ', qcdL
                print 'L est: ', res['NS'], 'pm', res['NSerr'], 'real: ', sigL
                print i, sigma, '+-', sigmaErr
                print i, sigmaS, '+-', sigmaErrS 
#            
            qSBCBL(x, sigma)
            qSBEBL(x, sigmaErr)
            
            qSBCSL(x, sigmaS)
            qSBESL(x, sigmaErrS)
            x += 1
        return [qualityBL, qualityBT, qualitySL, qualityST, effB, effQB, NvsCL, NvsCT, effQCDmcVSCUT]
            
            
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
                print 'est: ', res['NQT'], 'pm', res['NQTerr'], 'real: ', self.qcdhisttight.GetBinContent(x)
                
    def getStatError(self , eff, unweighted):
        if unweighted > 0:
            err = sqrt((eff * (1 - eff)) / unweighted)
        else:
            err = 0
        return err
    
    
    def saveReferencePlots(outputdir, files):
        save = '/playground/ThesisPlots/reference/%s/' % outputdir 
        f = open('plotconfigs/LKThesis.xml')
        cfg = f.read()
        f.close()
        cfg = cfg.replace('{$QCD}', files['qcd'])
        cfg = cfg.replace('{$TOP}', files['top'])
        cfg = cfg.replace('{$WJETS}', files['wjets'])
        Macro.savePlotsFromCfg(cfg, save)
    saveReferencePlots = staticmethod(saveReferencePlots)
    
    def saveMVAPlots(self,outputdir):
        Helper.set_palette("simple", 127)
        save = '/playground/ThesisPlots/MVATraining/%s/' % outputdir
        cfg = 'plotconfigs/MVAHists.xml'
        f = open(cfg)
        cfg = f.read()
        f.close()
        Macro.savePlotsFromCfg(cfg, save)
        
    def saveMacroPlots(self,outputdir, file='/playground/rootfiles/FINAL/complete_hists_MM.root'):
        save = ''
        if self.var == 'MVA':
            save = '/playground/ThesisPlots/macro/%s/' % outputdir
        elif self.var == 'met':
            save = '/playground/ThesisPlots/macro_MET/%s/' % outputdir
        cfg = ''
        if self.var == 'MVA':
            cfg = 'plotconfigs/LKMacro.xml'
        elif self.var == 'met':
            cfg = 'plotconfigs/LKMacro_MET.xml'
        f = open(cfg)
        cfg = f.read()
        f.close()
        cfg = cfg.replace('{$macro}', file)
        Macro.savePlotsFromCfg(cfg, save)
        
    def saveShapePlots(self, outputdir, file='/playground/rootfiles/FINAL/complete_hists_MM.root'):
        save = ''
        if self.var == 'MVA':
            save = '/playground/ThesisPlots/macro/%s/' % outputdir
        elif self.var == 'met':
            save = '/playground/ThesisPlots/macro_MET/%s/' % outputdir
        cfg = "plotconfigs/LKMacro2.xml"
        f = open(cfg)
        cfg = f.read()
        f.close()
        Macro.savePlotsFromCfg(cfg, save)
#    saveMacroPlots = staticmethod(saveMacroPlots)
        
        
    def savePlotsFromCfg(cfg, savedir):
        f = open('tmp.xml', 'w')
        f.write(cfg)
        f.close()
        Plotter.savePlotsFromCfg('tmp.xml', savedir)
#        cp = ConfigParser('tmp.xml') 
        os.remove('tmp.xml')
    savePlotsFromCfg = staticmethod(savePlotsFromCfg)
        
    def makeCutDependencyPlots(self, inputdirs, rcut):
#        shapes = ["ptUp", "circularityUp", "dPhiMJ12Up", "dPhiMETMuUp","ptDown", "circularityDown", "dPhiMJ12Down", "dPhiMETMuDown"]
        self.makeJCutPlots(inputdirs)
        self.makeMCutPlots(inputdirs)
        if self.var == "MVA":
            self.makeShapeDependencyPlots(inputdirs)
        
    def makeJCutPlots(self,inputdirs):
        keys = qcdfilesJ.keys()
        keys.sort()
        cuteffsJ = {}
        effs = {}
        tab = self.startTable('starting for jcuts', "jet-\Et")
        for i in keys:
            filesC = {'qcd' : qcdfilesJ[i], 'top' : topfilesJ[i], 'wjets' : wjetsfilesJ[i]}
#            filesV = {'qcd' : qcdVfilesJ[i], 'top' : topVfilesJ[i], 'wjets' : wjetsVfilesJ[i]}
        
            outputdir = 'jetcut_%s' % i
            for dir in inputdirs:
                self.makeMixed(filesC, dir)
                self.setN(dir, self.mixb.Integral(), self.mixa.Integral())
                for type in ['qcd', 'top', 'wjets', 'mixed']:
                    self.calcEff(type, filesC, dir, outputdir)
                
                effs[dir] = self.applyToOthers(filesC, dir, outputdir)
#            if not omg.has_key(i):
#                    omg[i] = {}
            omg[i+'J'] = self.effqcd
            sum = 'summary for %s \n' % (i+'J')
            cut = '\\SI{%s}{\\GeV}' % i
            tab = self.addToTable(tab, cut)
#            self.addToTables(sum)
#            self.makeTable()
                # 0,                     1            2          3            4         5
        #[qualityBL, qualityBT,qualitySL, qualityST, effB, effQB]
            eff = [effs['all'][0], effs['all'][1]]
            cuteffsJ[i] = self.test1(eff, cuteffsJ, i)
        self.test(cuteffsJ, 'Jcuts')
        tab = self.endTable(tab)
        self.addToTables(tab)
                
    def makeMCutPlots(self, inputdirs):
        cuteffsJ = {}
        effs = {}
        tab = self.startTable('starting for Mcuts', '$\mu$-\pt')
        keys = qcdfilesM.keys()
        keys.sort()
        for i in keys:
            filesC = {'qcd' : qcdfilesM[i], 'top' : topfilesM[i], 'wjets' : wjetsfilesM[i]}
            outputdir = 'muoncut_%s' % i
            for dir in inputdirs:
                self.makeMixed(filesC, dir)
                self.setN(dir, self.mixb.Integral(), self.mixa.Integral())
                for type in ['qcd', 'top', 'wjets', 'mixed']:
                    self.calcEff(type, filesC, dir, outputdir)
                effs[dir] = self.applyToOthers(filesC, dir, outputdir)
                # 0,                     1            2          3            4         5
        #[qualityBL, qualityBT,qualitySL, qualityST, effB, effQB]
            omg[i+'M'] = self.effqcd
            sum = 'summary for %s \n' % (i+'M')
#            self.addToTables(sum)
            cut = '\\SI{%s}{\\GeV}' % i
            tab = self.addToTable(tab, cut)
#            self.makeTable()
            eff = [effs['all'][0], effs['all'][1]]
            cuteffsJ[i] = self.test1(eff, cuteffsJ, i)
        self.test(cuteffsJ, 'Mcuts')
        tab = self.endTable(tab)
        self.addToTables(tab)
    
    def makeShapeDependencyPlots(self, inputdirs):
        self.output.cd('shapes')
        
        ptBG = TH1F("ptBG", "ptBG", 260, 20, 150)
        ptBG.Add(TF1("tmpPtBg", "1/(0.1*x -1.8)", 20, 150))   
        ptSIG = TH1F("ptSIG", "ptSIG", 260, 20, 150)
        ptSIG.Add(TF1("tmpPtSIG","0.1*x -1.8", 20, 150)) 
#        ptSIG = TF1("ptSIG", "0.1*x -1.8", 20, 150)
        
        circBG = TH1F("circBG", "ptBG", 100, 0, 1)
        circBG.Add(TF1("tmpcBg", "x*x -2*x +1", 0, 1))   
        circSIG = TH1F("circSIG", "ptSIG", 100, 0, 1)
        circSIG.Add(TF1("tmpcSIG","-x*x +2*x", 0,1)) 
        
#        circSIG = TF1("circSIG","-x*x +2*x" , 0, 1)
#        circBG = TF1("circBG", "x*x -2*x +1", 0, 1)
        METMuBG = TH1F("METMuBG", "METMuBG", 200, -4, 4)
        METMuSIG = TH1F("METMuSIG", "METMuSIG", 200, -4, 4)
        METMuBG.Add(TF1("TMETMuBG","1/(1.5/pi *x + 0.5)" ,0, 4))
        METMuBG.Add(TF1("TMETMuBG","1/(-1.5/pi *x + 0.5)" ,-4, 0))
        METMuSIG.Add(TF1("TMETMuSIG", "1.5/pi *x + 0.5", 0, 4))
        METMuSIG.Add(TF1("TMETMuSIG", "-1.5/pi *x + 0.5", -4, 0))
        
        testBG = TF1("TtMETMuBG","1/(1.5/pi *x + 0.5)" ,0, 4)
        testSIG = TF1("TtMETMuSIG", "1.5/pi *x + 0.5", 0, 4)
        
        MJ12BG = TH1F("MJ12BG", "MJ12BG", 200,-6.3, 6.3)
        MJ12SIG = TH1F("MJ12SIG", "MJ12SIG", 200, -6.3, 6.3)
        MJ12SIG.Add(TF1("tMJ12SIG", "(1/pi/pi)*x*x - 2/pi*x + 1", 0, 6.3))
        MJ12SIG.Add(TF1("tMJ12SIG", "(1/pi/pi)*x*x + 2/pi*x + 1", -6.3, 0))
        MJ12BG.Add(TF1("tMJ12BG", "1-((1/pi/pi)*x*x - 2/pi*x + 1)", 0, 6.3))
        MJ12BG.Add(TF1("tMJ12BG", "1-((1/pi/pi)*x*x + 2/pi*x + 1)", -6.3, 0))
        
        ptBG.Write()
        ptSIG.Write()
        circSIG.Write()
        circBG.Write()
        METMuBG.Write()
        METMuSIG.Write()
        MJ12SIG.Write()
        MJ12BG.Write()
        cuteffsJ = {}
        effs = {}
#        tab = self.startTable('starting for Shapes', '$\mu$-\pt')
        keys = shapeFiles.keys()
        keys.sort()
        qcd = "/playground/rootfiles/FINAL/qcdmu_20j20m.root"
        top = "/playground/rootfiles/FINAL/top_20j20m.root"
        wjets = "/playground/rootfiles/FINAL/wjets_20j20m.root"
        Qfile = TFile(qcd)
        topfile = TFile(top)
        wjetfile = TFile(wjets)
        for dir in inputdirs:
            self.tophisttight = topfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscSignalTight')
            self.tophistloose = topfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscSignalLoose')
            self.wjethisttight = wjetfile.Get(dir + self.after + '/' + self.hists['weighted']).Clone('nVSdiscWjetTight')
            self.wjethistloose = wjetfile.Get(dir + self.before + '/' + self.hists['weighted']).Clone('nVSdiscWjetLoose')
            effsig = (self.tophisttight.Integral() + self.wjethisttight.Integral())/(self.tophistloose.Integral() + self.wjethistloose.Integral())
        
            self.mixunwb = topfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose')
            self.mixunwb.Add(wjetfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose'))
            self.mixunwb.Add(Qfile.Get(dir + self.before + '/' + self.hists['simple']).Clone('nVSdiscQCDLoose'))
            
            self.mixunwa = topfile.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscQCDtight')
            self.mixunwa.Add(wjetfile.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscQCDtight'))
            self.mixunwa.Add(Qfile.Get(dir + self.after + '/' + self.hists['simple']).Clone('nVSdiscQCDtight'))
            unw = self.mixunwb.Integral()
            effsigerr = self.getStatError(effsig, unw)
            qt = Qfile.Get(dir + self.after + '/' + self.hists['weighted'])
            qcdtight = qt.Integral()
            ql = Qfile.Get(dir + self.before + '/' + self.hists['weighted'])
            qcdloose = ql.Integral()   
            qcdUpCirc = "/playground/rootfiles/FINAL/qcdMu_circUp.root"
            qcdDownCirc ="/playground/rootfiles/FINAL/qcdMu_circDown.root"
        
        #do mixed
            for k in keys:
                qcdfile= TFile(shapeFiles[k])
                
                self.qcdhisttight = qcdfile.Get(dir + self.after + '/' + "var_MVAdisc").Clone('nVSdiscQCDTight')                
                self.qcdhistloose = qcdfile.Get(dir + self.before + '/' + "var_MVAdisc").Clone('nVSdiscQCDLoose')
#                if k == "dPhiMETMuSIG":
#                    ql.Multiply(testSIG)
#                    qt.Multiply(testSIG)
#                    self.qcdhistloose = ql
#                    self.qcdhisttight = qt
#                elif k == "dPhiMETMuBG":
#                    ql.Multiply(testBG)
#                    qt.Multiply(testBG)
#                    self.qcdhistloose = ql
#                    self.qcdhisttight = qt
                #------------------------------------------------------------------------------ rescaling to right amount
                self.qcdhistloose.Scale(1/self.qcdhistloose.Integral())
                self.qcdhisttight.Scale(1/self.qcdhisttight.Integral())   
                self.qcdhistloose.Scale(qcdloose)
                self.qcdhisttight.Scale(qcdtight)   
                effqcd = copy.deepcopy(self.qcdhisttight.Clone("efficiency"))
                effqcd.Sumw2()
                effqcd.Divide(self.qcdhistloose)
                allT = self.tophisttight.Clone("tmp1")
                allT.Add(self.wjethisttight)
                allT.Add(self.qcdhisttight)
                
                allL = self.tophistloose.Clone("tmp2")
                allL.Add(self.wjethistloose)
                allL.Add(self.qcdhistloose)
                backroundoverSignalB = self.qcdhistloose.Clone("bgOverbgs_loose")
                backroundoverSignalA = self.qcdhisttight.Clone("bgOverbgs_tight")
                backroundoverSignalA.Divide(allT)
                backroundoverSignalB.Divide(allL)
#                effqcd.Sumw2()
#===============================================================================
#            mix all
#===============================================================================
                self.mixa = self.qcdhisttight.Clone("mixedAfter")
                self.mixa.Add(self.tophisttight)
                self.mixa.Add(self.wjethisttight)
                
                self.mixb = self.qcdhistloose.Clone("mixedbefore")
                self.mixb.Add(self.tophistloose)
                self.mixb.Add(self.wjethistloose)
                completedir = "shapes/"
                if not self.output.GetDirectory('shapes').GetDirectory(k):
                    self.output.GetDirectory('shapes').mkdir(k)
                if not self.output.GetDirectory('shapes').GetDirectory(k).GetDirectory(dir):
                    self.output.GetDirectory('shapes').GetDirectory(k).mkdir(dir)
                    
                if not self.output.GetDirectory('shapes').GetDirectory(k).GetDirectory(dir).GetDirectory("sig"):
                    self.output.GetDirectory('shapes').GetDirectory(k).GetDirectory(dir).mkdir('sig')
                if not self.output.GetDirectory('shapes').GetDirectory(k).GetDirectory(dir).GetDirectory("bkg"):
                    self.output.GetDirectory('shapes').GetDirectory(k).GetDirectory(dir).mkdir('bkg')
                
                completedir += k + "/" +dir
                
                self.doEffMixed([], dir, k)
                loose = self.mixb.Integral()
                tight = self.mixa.Integral()
                self.output.Cd("/")
                qu = self.makeQualityHist(loose, tight, effsig, effsigerr, dir, "shapes/"+k)
                self.output.cd(completedir + '/bkg')
                qu[0].Write()
                qu[1].Write()
                qu[4].Write()
                qu[5].Write()
                qu[6].Write()
                qu[7].Write()
                qu[8].Write()
                backroundoverSignalB.Write()
                backroundoverSignalA.Write()
                effqcd.Write()
                self.output.Cd('/')
                self.output.cd(completedir + '/sig')
                qu[2].Write()
                qu[3].Write()
                
        
    def startTable(self, note, cut):
        table = note + '\n'
        header = '\\begin{table}[!ht] \n' 
        header += '\\begin{tabular}{lcccc} \n' 
        header += '\\toprule \n' 
        k = ['all','jetiso', 'track', 'calo']
        k.sort()
        header += '%s ' %cut
        for i in k:
            header += '& %s ' %i 
        header += '\\\\ \n \\midrule \n' 
        table += header
        return table
    
    def addToTable(self, table, cut):
        body = '%s ' %cut
        k = self.effwjet.keys()
        k.sort()
        for i in k:
            qcd = Timer.formatErrorPair(self.effqcd[i][0], self.effqcd[i][1])
            body += '& %s ' % (qcd)
        table += body + "\\\\ \n"
        return table
        
    def endTable(self, table):
        footer = '\\bottomrule \n' 
        footer += '\\end{tabular} \n' 
        footer += '\\caption{Isolations for different cuts} \n' 
        footer += '\\label{tab:iso} \n' 
        footer += '\\end{table} \n' 
        table += footer
        return table
        
    def makeTable(self):
        header = '\\begin{table}[!ht] \n' 
        header += '\\begin{tabular}{@{} lccc} \n' 
        header += '\\toprule \n' 
        header += 'isolation & top-signal& W+Jets & QCD \\\\ \n' 
        header += '\\midrule \n' 
        body = ''
        k = self.effwjet.keys()
        k.sort()
        for i in k:
            top = Timer.formatErrorPair(self.effsig[i][0], self.effsig[i][1])
            wjets = Timer.formatErrorPair(self.effwjet[i][0], self.effwjet[i][1])
            qcd = Timer.formatErrorPair(self.effqcd[i][0], self.effqcd[i][1])
            body += '%s & %s & %s & %s\\\\ \n ' % (i, top, wjets, qcd)
#            body += '%s & %1.3f $\\pm$ %1.3f & %1.3f $\\pm$ %1.3f & %1.3f $\\pm$ %1.3f \\\\ \n ' % (i, self.effsig[i][0], self.effsig[i][1], self.effwjet[i][0], self.effwjet[i][1], self.effqcd[i][0], self.effqcd[i][1])
        footer = '\\bottomrule \n' 
        footer += '\\end{tabular} \n' 
        footer += '\\caption{Isolation efficiencies for top-signal, W+jets and QCD} \n' 
        footer += '\\label{tab:iso} \n' 
        footer += '\\end{table} \n' 
        
        table = header + '\n' + body + '\n' + footer + '\n' 
        table += '\n\n' 
        self.addToTables(table)
    
    def writeTables(self):
        f = open('MacroTables.tex', 'a')
        f.write(self.tables)
        f.close()
        
    def test1(self, eff, cuteffsJ, i):
        eGBC = eff[0].GetBinContent
        eGBE = eff[0].GetBinError
        eFB = eff[0].GetXaxis().FindBin
        eGBC2 = eff[1].GetBinContent
        eGBE2 = eff[1].GetBinError
        eFB2 = eff[1].GetXaxis().FindBin
        cuteffsJ[i] = {}
        for varcut in self.cuts:
            bin = eFB(varcut) -1
            bin2 = eFB2(varcut) -1
            cuteffsJ[i][varcut] = [[eGBC(bin), eGBE(bin)], [eGBC2(bin2), eGBE2(bin2)]]
        return cuteffsJ[i]
    
    def test(self, cuteffsJ, cutdir):        
        k = cuteffsJ.keys()
        k.sort()
        if not self.output.GetDirectory('applied').GetDirectory(cutdir):
            self.output.GetDirectory('applied').mkdir(cutdir)
        self.output.cd('applied/' + cutdir)
#        neu = TH1F('neu', 'neu', 7, 15, 45)
        
        for cut in self.cuts:
            name1 = 'quality_cut_%s_loose' % cut
            name2 = 'quality_cut_%s_tight' % cut
            neu = None
            if cutdir == 'Mcuts':
                neu = TH1F(name1, name1, 7, 17.5, 32.5)
                neu2 = TH1F(name2, name2, 7, 17.5, 32.5)
            elif cutdir == 'Jcuts':
                neu = TH1F(name1, name1, 7, 15, 45)
                neu2 = TH1F(name2, name2, 7, 15, 45)
                
            nSBC = neu.SetBinContent
            nSBE = neu.SetBinError
            n2SBC = neu2.SetBinContent
            n2SBE = neu2.SetBinError
            for i in k:
                b = neu.FindBin(float(i))
                nSBC(b, cuteffsJ[i][cut][0][0])
                nSBE(b, cuteffsJ[i][cut][0][1])
                n2SBC(b, cuteffsJ[i][cut][1][0])
                n2SBE(b, cuteffsJ[i][cut][1][1])
            neu.Write()
            neu2.Write()
        self.output.Cd('/')
        
    def addToTables(self, string):
        self.tables += string
        
    def saveCutPlots(self):
        keys = qcdfilesJ.keys()
        keys.sort()
        for i in keys:
            files = {'qcd' : qcdfilesJ[i], 'top' : topfilesJ[i], 'wjets' : wjetsfilesJ[i]}
            outputdir = 'jetcut_%s' % i
            Macro.saveReferencePlots(outputdir, files)
            
        
        keys = qcdfilesM.keys()
        keys.sort()
        for i in keys:
            files = {'qcd' : qcdfilesM[i], 'top' : topfilesM[i], 'wjets' : wjetsfilesM[i]}
            outputdir = 'muoncut_%s' % i
            Macro.saveReferencePlots(outputdir, files)
            
    def saveAllPlots(self, what=["cuts", "macro", "MVA"]):
        if "cuts" in what:
            self.saveCutPlots()
        if "macro" in what:        
            self.saveMacroPlots("")
        if "MVA" in what:
            self.saveMVAPlots('')
        if "shapes" in what:
            self.saveShapePlots('')
#    saveAllPlots = staticmethod(saveAllPlots)

if __name__ == '__main__':
#===============================================================================
#    delete the Latex file if it exists
#===============================================================================
    if os.path.exists('MacroTables.tex'):
        os.remove('MacroTables.tex')
    gROOT.Reset()
    Helper.set_palette('', 999)
    TGaxis.SetMaxDigits(3)
#===============================================================================
#    for white background
#===============================================================================
    gROOT.SetStyle("Plain")
    gROOT.SetBatch(True)
    #set color palette for TH2F
#    inputfiles = {}
#    folder = '/playground/rootfiles/FINAL/'
#    inputfiles['qcd'] = folder + "qcdmu_20j20m.root"
#    inputfiles['top'] = folder + "top_20j20m.root"
#    inputfiles['wjets'] = folder + "wjets_20j20m.root"
    inputdirs = ["all", "jetIso", "calo", "track"]
#    inputs = {}
#    inputs['qcd'] = folder + "qcdmu_20j20m.root"
#    inputs['top'] = folder +  "top_20j20m.root"
#    inputs['wjets'] = folder + "wjets_20j20m.root"
    mac = Macro(folder + 'complete_hists_MM.root', inputdirs, 'MVA')
    mac.debug = False
    mac.makeCutDependencyPlots(inputdirs, '0.2')
    mac.output.Close()
    mac.writeTables()
    mac.saveAllPlots(["cuts"])
