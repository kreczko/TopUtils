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
            self.nPadsX = 2
            self.nPadsY = 1
            self.width = 600
            self.height = 0.55 * self.width;
        elif nPlots == 3:
            self.nPadsX = 3
            self.nPadsY = 1
            self.width = 900
            self.height = 0.40 * self.width;
        elif nPlots == 4:
            self.nPadsX = 2
            self.nPadsY = 2
            self.width = 600
            self.height = 1.00 * self.width;
        else:
            self.nPadsX = 3
            self.nPadsY = 2
            self.width = 800
            self.height = 0.55 * self.width;
            
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
            return None
        else:
            pad = self.last.GetPad(self.index)
            pad.SetFillColor(0)
            pad.GetFrame().SetFillColor(0)
            return pad
            
        