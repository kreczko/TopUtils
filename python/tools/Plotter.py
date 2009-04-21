from XMLConfigParser import *
import os
from ROOT import gROOT
from Drawer import Drawer


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
            for var in hist.subobjects["var"]:
                source = var.getOption("source")
                if var.getOption('operation') == 'none':
                    file = source[0]
                    h1 = source[1]
                    f = TFile(file)
                    gROOT.cd()
                    h = f.Get(h1).Clone()
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
                        f = TFile(varroots[xxx])
                        h1 = varhists[xxx]
                        gROOT.cd()
                        if xxx == 0:#load first variable
                            h = f.Get(h1).Clone()
                        else:#add or divide by the 2nd variable
                            #TODO: in order to make real math, all variables need to be saved temporaly
                            if var.getOption('operation') == 'add':
                                h.Add(f.Get(h1))
                            elif var.getOption('operation') == 'div':
#                                print h1
#                                print var.hist
#                                print h.GetName()
                                if hist.getOption("showErrors").upper() == "TRUE":
                                    h.Sumw2()
                                h.Divide(f.Get(h1))
#                                if hist.opt["showErrors"]:
#                                    h.Sumw2()
                            else:
                                print 'unknown operation'

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
            Drawer.saveHistsInOne(list, hist, savedir, writeAs) 
    def savePlots(self,savedir, cfg = "Plotter"):
        cfg = Configuration(cfg)
        cfg.loadConfiguration(self.config)
        plots = cfg.getPlots()
        writeAs = plots.getOption("create").split(",")
        histlist = plots.subobjects["hist"]
        self.doHistList(histlist, savedir, writeAs)
        histlists = plots.subobjects["histlist"]
        for i in histlists:
            self.doHistList(i.subobjects["hist"], savedir, writeAs)
        
if __name__ == "__main__":
    Drawer.setDefaultLayout()
    pl = Plotter("test/NewConfig.xml")
    pl.savePlots("plots")
