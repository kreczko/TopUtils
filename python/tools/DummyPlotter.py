from ConfigParser import *
from DrawHelper import Helper
from Timer import Timer
class DummyPlotter:
    def __init__(self, config):
        c = ConfigParser(config)
        plots = c.getPlots()
        histlist = plots.histlist
        hist = histlist[1]
        file = hist.getVarByName('qcd').rootsource
        h1 =  hist.getVarByName('qcd').hist
        f = TFile(file)
        h = f.Get(h1)
        h = Helper.setMarker(h,4,3,1)
        h = Helper.setHistLabels(h, hist.opt['titleX'], hist.opt['titleY'])
        Helper.saveHist(h, "quality_loose_qcd", 'test')
#        for i in histlist:
#            file = i.getVarByName('qcd').rootsource
#            hist =  i.getVarByName('qcd').hist
#            f = TFile(file)
#            h = f.Get(hist)
#            h.Draw()
#            Timer.sleep(5)
            
if __name__ == '__main__':
    d = DummyPlotter('test/Testconfig.xml')