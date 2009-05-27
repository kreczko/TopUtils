from ROOT import TCanvas, TVirtualPad

"""
builds canvas up to 6 hists
"""
class PadService:
    def __init__(self, name, title, nPlots):
        if nPlots == 1:
            self.nPadsX = 1
            self.nPadsY = 1
            self.width = 600
            self.height = 1.0 * self.width
        elif nPlots == 2:
            self.nPadsX = 1
            self.nPadsY = 2
            self.height = 600
            self.width = 0.55*self.height;
        elif nPlots > 2 and nPlots < 5:
            self.nPadsX = 2
            self.nPadsY = 2
            self.width = 600
            self.height = 0.8 * self.width;
        elif nPlots > 4 and nPlots < 7:
            self.nPadsX = 2
            self.nPadsY = 3
            self.height = 800
            self.width = 0.7 *  self.height;
        elif nPlots > 6 and nPlots < 9:
            self.nPadsX = 2
            self.nPadsY = 4
            self.height = 800
            self.width = 0.6 *  self.height;
        else:
            self.nPadsX = 3
            self.nPadsY = 4
            self.height = 800
            self.width = 0.8 *  self.height;
            
            
        self.last = None
        self.index = 0
        self.count = 0
        self.name = name
        self.title = title
 
    def Next(self):
        if not self.last:
            self.count += 1
            name = '%s_%d' % (self.name, self.count)
            self.last = TCanvas(name.__str__(), self.title.__str__(), int(self.width), int(self.height))
            self.last.SetBorderSize(2);
            self.last.SetFrameFillColor(0);
            self.last.SetHighLightColor(0);
            self.last.Divide(self.nPadsX, self.nPadsY);
            self.index = 0;
            self.count+=1
        self.index += 1
        self.last.cd(self.index);
        #prevent override
        if self.index > self.nPadsX * self.nPadsY:
            print "EERER"
            self.last = None
        else:
            pad = self.last.GetPad(self.index)
            return pad
            
        