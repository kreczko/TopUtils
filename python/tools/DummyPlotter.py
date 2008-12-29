from ConfigParser import *
from Timer import Timer
class DummyPlotter:
    def __init__(self, config):
        c = ConfigParser(config)
        plots = c.getPlots()
        histlist = plots.histlist
        for i in histlist:
            file = i.getVarByName('qcd').rootsource
            hist =  i.getVarByName('qcd').hist
            f = TFile(file)
            h = f.Get(hist)
            h.Draw()
            Timer.sleep(5)
            
if __name__ == '__main__':
    d = DummyPlotter('test/Testconfig.xml')