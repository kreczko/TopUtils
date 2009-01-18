from ROOT import TLegend, TBox
import PadService as ps
import os

class Helper:
    def makePlainLegend(legXLeft, legYLower, legXRight, legYUpper):
        leg = TLegend(legXLeft, legYLower, legXRight, legYUpper)
        leg = Helper.setLegendStyle(leg)
        return leg
        
    makePlainLegend = staticmethod(makePlainLegend)
        
    def setLegendStyle(leg):
        leg.SetFillStyle(0)
        leg.SetFillColor(0)
        leg.SetBorderSize(0)
        return leg
    setLegendStyle = staticmethod(setLegendStyle)
    
    def setHistLabels(hist, titleX, titleY):
        hist.SetTitle("")
        hist.SetStats(0);
        hist.SetFillColor(0)
        hist.GetXaxis().SetTitle(titleX)
        hist.GetXaxis().SetTitleSize(0.06)
        hist.GetXaxis().SetTitleColor(1)
        hist.GetXaxis().SetTitleOffset(1.0)
        hist.GetXaxis().SetTitleFont(62)
        hist.GetXaxis().SetLabelSize(0.05)
        hist.GetXaxis().SetLabelFont(62)
        #hist.GetXaxis().CenterTitle()
        hist.GetXaxis().SetNdivisions(505)

        hist.GetYaxis().SetTitle(titleY)
        hist.GetYaxis().SetTitleSize(0.07)
        hist.GetYaxis().SetTitleColor(1)
        hist.GetYaxis().SetTitleOffset(1.3)
        hist.GetYaxis().SetTitleFont(62)
        hist.GetYaxis().SetLabelSize(0.05)
        hist.GetYaxis().SetLabelFont(62)
        return hist
    setHistLabels = staticmethod(setHistLabels)
    
    def saveHist(hist, filename, folder, legend = None):
        if not os.path.exists(folder):
            os.mkdir(folder)
        pads = ps.PadService('test', 'testing', 1)
        pad = pads.Next()
        pad.GetFrame().SetFillColor(0)
        pad.SetFillStyle(4000)
        pad.SetLeftMargin( 0.20);
        pad.SetRightMargin( 0.05);
        pad.SetBottomMargin( 0.15);
        pad.SetTopMargin( 0.05);
        for i in pad.GetListOfPrimitives():
            i.SetFillColor(0)
        hist.Draw()
        if legend:
            legend.Draw("same")
        pad.RedrawAxis();
        pad.Print(folder + '/' + filename + '.pdf')
        pad.Print(folder + '/' + filename + '.eps')
        pad.Print(folder + '/' + filename + '.png')
    saveHist = staticmethod(saveHist)
        
    def setMarker(hist, color, size, style):
        hist.SetMarkerColor(color)
        hist.SetMarkerSize(size)
        hist.SetMarkerStyle(style)
        hist.SetLineWidth(size);
        hist.SetLineColor(color);
        return hist
    setMarker = staticmethod(setMarker)
    
        