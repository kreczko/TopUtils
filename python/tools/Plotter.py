from XMLConfigParser import *
import XMLConfigParser
import os, sys
from ROOT import gROOT, TFile
from Drawer import Drawer
from Timer import Timer

ts = ["readConfig", "drawHists", "readhists", "total"]
perf = {}
perf[ts[0]] = Timer()
perf[ts[1]] = Timer()
perf[ts[2]] = Timer()
perf[ts[3]] = Timer()


class Plotter:
    
    def __init__(self, configfile):
        self.config = ""
        if configfile == "":
            raise ConfigError, "Configuration file is empty"
    
        elif not os.path.exists(configfile):
            raise IOError, "Configuration file '%s' does not exist" %configfile
        else:
            self.config = configfile
            
    def doHistList(self, histlist, savedir, writeAs):
        for hist in histlist:
            list = {}
            h = None
            if hist == histlist[-1]:
                Drawer.summaryBegin = False
            for var in hist.subobjects["hist"]:

                source = var.getOption("source")
                if var.getOption('operation') == 'none':
                    file = source[0]
                    h1 = source[1]
                    perf[ts[2]].start()
                    f = TFile(file)
                    gROOT.cd()
                    tmp = f.Get(h1)
                    if tmp:
                        h = tmp.Clone()
                    else:
                        sys.exit("Histogram %s in file %s not found" % (h1, file))
                    perf[ts[2]].stop()
                else: #combine histograms with operations
                    integ = 0
                    varhists = []
                    varroots = []
                    for i in source:
                        s = i.getOption("source")
                        file = s[0]
                        h1 = s[1]
                        varhists.append(h1)
                        varroots.append(file)
                    for xxx in range(0, len(varroots)):
                        perf[ts[2]].start()
                        f = TFile(varroots[xxx])
                        h1 = varhists[xxx]
                        gROOT.cd()
                        tmp = f.Get(h1)
                        if tmp:
                            h = tmp.Clone()
                        else:
                            sys.exit("Histogram %s in file %s not found" % (h1,varroots[xxx]))
                        if xxx == 0:#load first variable
                            h = tmp.Clone()
                        else:#add or divide by the 2nd variable
                            #TODO: in order to make real math, all variables need to be saved temporaly
                            if var.getOption('operation') == 'add':
                                h.Add(tmp)
                            elif var.getOption('operation') == 'div':
#                                print h1
#                                print var.hist
#                                print h.GetName()
                                if hist.getOption("showErrors").upper() == "TRUE":
                                    h.Sumw2()
                                h.Divide(tmp)
#                                if hist.opt["showErrors"]:
#                                    h.Sumw2()
                            else:
                                print 'unknown operation'
                        perf[ts[2]].stop()
#===============================================================================
#                once you have the histogram, apply the configuration to it.
#===============================================================================
                h = Drawer.applyHistConfig(h, hist, var)
#===============================================================================
#                only hists which are marked to be draw are added to list
#===============================================================================
                if var.getOption('draw') == '1':
                    list[var.getOption("name")] =h.Clone()
#                    list.append(copy.deepcopy(h.Clone()))
#===============================================================================
#                save all histograms in one file
#===============================================================================
            perf[ts[1]].start()
            Drawer.saveHistsInOne(list, hist, savedir, writeAs) 
            perf[ts[1]].stop()
            
    def savePlots(self,savedir, cfg = "Plotter"):
        perf[ts[3]].start()
        perf[ts[0]].start()
        cfg = Configuration(cfg)
        cfg.loadConfiguration(self.config)
        plots = cfg.getPlots()
        writeAs = plots.getOption("create").split(",")
        summaryFile = plots.getOption("summaryFile")
        Drawer.summaryFile = savedir + "/" + summaryFile
#        if summaryFile != "":
#            Drawer.createSummary(savedir, summaryFile)
        histlist = cfg.getAllCanvas()
        perf[ts[0]].stop()
        print "Config read in %s" % perf[ts[0]].getMeasuredTime()
        self.doHistList(histlist, savedir, writeAs)
        perf[ts[3]].stop()
        noh = len(histlist)
        print "%d istograms read in %s (%f per hist)" % (noh, perf[ts[2]].getMeasuredTime(), perf[ts[2]].getMeasuredSeconds()/noh)
        print "%d histograms printed in %s (%f per hist)" % (noh, perf[ts[1]].getMeasuredTime(), perf[ts[1]].getMeasuredSeconds()/noh)
        print "total time elapsed: %s" % perf[ts[3]].getMeasuredTime()
        #Drawer.drawSummary(savedir, "Oo.ps")#
        
if __name__ == "__main__":
    XMLConfigParser.pathToDir = ""
    Drawer.setDefaultLayout()
    pl = Plotter("test/NewConfig.xml")
    print "Saving test configuration"
    pl.savePlots("plots")
    print "Thanks and GoodBye."
