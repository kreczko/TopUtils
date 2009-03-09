from DrawHelper import Helper
from ConfigParser import *

class Plotter:
    """
    A small example of the new Plotting Tool
    """
#===============================================================================
# Draws all plots defined in the confgfile
#===============================================================================
    def savePlotsFromCfg(configfile, savedir):
#===============================================================================
# create ConfigParser instance
#===============================================================================
        cp = ConfigParser(configfile)
#===============================================================================
# get the Plotlist
#===============================================================================
        plots = cp.getPlots()
#===============================================================================
# get the file-extensions the hists should be saved as
#===============================================================================
        writeAs = plots.fileOutputs
#===============================================================================
#  get the histogram paths
#===============================================================================
        histlist = plots.histlist
#===============================================================================
#   do something with all histograms in the list
#===============================================================================
        for hist in histlist:
            list = {}
            h = None

            for var in hist.varlist:
                file = var.rootsource
                h1 = var.hist
#===============================================================================
#                if no histogramm operation is defined, just load the histogram
#===============================================================================
                if var.opt['operation'] == 'none':
                    f = TFile(file)
                    h = f.Get(h1)
                else: #combine histograms with operations
#                    integ = 0
                    varhists = h1.split(Variable.entryDelimiter)
                    varroots = var.rootsource.split(Variable.entryDelimiter)
                    for xxx in range(0, len(varroots)):
                        f = TFile(varroots[xxx])
                        h1 = varhists[xxx]
                        if xxx == 0:#load first variable
                            h = copy.deepcopy(f.Get(h1).Clone())
                        else:#add or divide by the 2nd variable
                            #TODO: in order to make real math, all variables need to be saved temporaly
                            if var.opt['operation'] == 'add':
                                h.Add(f.Get(h1))
                            elif var.opt['operation'] == 'div':
#                                print h1
#                                print var.hist
#                                print h.GetName()
                                if hist.opt["showErrors"].upper() == "TRUE":
                                    h.Sumw2()
                                h.Divide(f.Get(h1))
#                                if hist.opt["showErrors"]:
#                                    h.Sumw2()
                            else:
                                print 'unknown operation'

#===============================================================================
#                once you have the histogram, apply the configuration to it.
#===============================================================================
                h = Helper.applyHistConfig(h, hist, var)
#===============================================================================
#                only hists which are marked to be draw are added to list
#===============================================================================
                if var.opt['draw'] == '1':
                    list[var.opt["name"]] = copy.deepcopy(h.Clone())
#                    list.append(copy.deepcopy(h.Clone()))
#===============================================================================
#                save all histograms in one file
#===============================================================================
            Helper.saveHistsInOne(list, hist, savedir, writeAs)        
    savePlotsFromCfg = staticmethod(savePlotsFromCfg)