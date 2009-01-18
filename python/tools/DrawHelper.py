from ROOT import TLegend, TBox
import PadService as ps
import os

class Helper:
    allowedFormats = ['eps', 'ps', 'pdf', 'png', 'jpg']
    def makePlainLegend(x, y, sizeX, sizeY):
        x = float(x)/100
        y = float(y)/100
        sizeX = float(sizeX)/100
        sizeY = float(sizeY)/100
        leg = TLegend(x, y - sizeY, x + sizeX,y) 
        leg = Helper.setLegendStyle(leg)
        return leg
        
    makePlainLegend = staticmethod(makePlainLegend)
        
    def setLegendStyle(leg):
        leg.SetFillStyle(0)
        leg.SetFillColor(0)
        leg.SetBorderSize(0)
        return leg
    setLegendStyle = staticmethod(setLegendStyle)
    
    def setHistLabels(hist, titleX, titleY, rotate=False):
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
        hist.GetYaxis().SetTitleOffset(1.0)
        hist.GetYaxis().SetTitleFont(62)
        hist.GetYaxis().SetLabelSize(0.05)
        hist.GetYaxis().SetLabelFont(62)
        hist.GetYaxis().RotateTitle(rotate)
        return hist
    setHistLabels = staticmethod(setHistLabels)
    
    def saveHist(hist, filename, folder, legend = None, printAs  = ['eps', 'png']):
        if not os.path.exists(folder):
            os.mkdir(folder)
        pads = ps.PadService('test', 'testing', 1)
        pad = pads.Next()
        pad = Helper.setPadLayout(pad)
        pad.SetTopMargin( 0.05);
        for i in pad.GetListOfPrimitives():
            i.SetFillColor(0)
        hist.Draw()
        if legend:
            legend.Draw("same")
        
        pad.RedrawAxis();
        for i in printAs:
            if i in Helper.allowedFormats:
                pad.Print(folder + '/' + filename + '.' + i)
    saveHist = staticmethod(saveHist)
    
    def setPadLayout(pad):
        pad.SetFillStyle(4000)
        pad.SetLeftMargin( 0.15)
        pad.SetRightMargin( 0.05)
        pad.SetBottomMargin( 0.15)
        pad.SetTopMargin( 0.05)
        return pad
    setPadLayout = staticmethod(setPadLayout)
        
    def saveHistsInOne(histlist, filename, folder, legend = None,log = ['0','0'], err = False, printAs = ['eps', 'png']):
        folder =  folder.rstrip('/')
        if not os.path.exists(folder):
            os.mkdir(folder)
        pads = ps.PadService('test', 'testing', 1)
        pad = pads.Next()
        pad = Helper.setPadLayout(pad)
        if log[0] == 1:
            pad.SetLogx()
        if log[1] == 1:
            pad.SetLogy()
        min = Helper.getMin(histlist)
        max = Helper.getMax(histlist)
        x = 0
        for hist in histlist:
            hist.SetMaximum(max)
            hist.SetMinimum(min)
            if legend:
                legend.AddEntry(hist, hist.GetName())
            if x == 0:
                if err:
                    hist.Draw('e')
                else:
                    hist.Draw()
            else:
                if err:
                    hist.Draw('samee')
                else:
                    hist.Draw("same")
            x += 1
        if legend:
            legend.Draw("same")
        pad.RedrawAxis();
        for i in printAs:
            if i in Helper.allowedFormats:
                pad.Print(folder + '/' + filename + '.' + i)
    saveHistsInOne = staticmethod(saveHistsInOne)
        
    def setMarker(hist, color, size, style):
        hist.SetMarkerColor(int(color))
        hist.SetMarkerSize(float(size))
        hist.SetMarkerStyle(int(style))
        hist.SetLineWidth(int(size))
        hist.SetLineColor(int(color))
        hist.SetOption("scat");
        return hist
    setMarker = staticmethod(setMarker)
    
    def setLine(hist, color, size, style):
        hist.SetLineColor(int(color))
        hist.SetLineWidth(int(size))
        hist.SetLineStyle(int(style))
        return hist
    setLine = staticmethod(setLine)   
    
    def setFilled(hist, color, size, style):
        hist.SetLineColor(int(color))
        hist.SetLineWidth(int(size))
        
        hist.SetFillColor(int(color))
        hist.SetFillStyle(int(style))
        hist.SetOption("col")       
        return hist
    setFilled = staticmethod(setFilled)   
    
    def applyHistConfig(hist, histconfig, var):
        #set titles
        hist = Helper.setHistLabels(hist, histconfig.opt['titleX'], histconfig.opt['titleY'], histconfig.opt['rotate'].upper() =='T')
        if var.opt['type'] == 'filled':
            hist = Helper.setFilled(hist, var.opt['color'],var.opt['size'],var.opt['style'])
        elif var.opt['type'] == 'marker':
            hist = Helper.setMarker(hist, var.opt['color'],var.opt['size'],var.opt['style'])
        else:
            hist = Helper.setLine(hist, var.opt['color'],var.opt['size'],var.opt['style'])
        hist.SetName(var.opt['name'])
        if var.opt['norm'] == 'Integral':
            hist.Scale(1/hist.Integral())
        return hist
    applyHistConfig = staticmethod(applyHistConfig)
    
    def getMax(histlist):
        yScale = 1.05
        max = 0
        x = 0
        for i in histlist:
            mx =  i.GetMaximum()
            if x == 0:
                max = mx
            else:
                if mx > max:
                    max = mx
            x+= 1
        return max*yScale
        
    getMax = staticmethod(getMax)
    
    def getMin(histlist):
        yScale = 1.05
        min = 0
        x = 0
        for i in histlist:
            mn =  i.GetMinimum()
            if x == 0:
                min = mn
            else:
                if mn < min:
                    min = mn
            x+= 1
        return min*yScale
    getMin = staticmethod(getMin)
    
        