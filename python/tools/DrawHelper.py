from ROOT import TLegend, TBox, TColor, gStyle, TPaletteAxis, TPaveText
import PadService as ps
import os
from math import log
from array import array

class Helper:
    drawOption = ''
    allowedFormats = ['eps', 'ps', 'pdf', 'png', 'jpg']
    def makePlainLegend(x, y, sizeX, sizeY):
        x = float(x) / 100
        y = float(y) / 100
        sizeX = float(sizeX) / 100
        sizeY = float(sizeY) / 100
        leg = TLegend(x, y - sizeY, x + sizeX, y) 
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
        
        if Helper.drawOption:
            hist.GetZaxis().SetTitleSize(0.07)
            hist.GetZaxis().SetTitleColor(1)
            hist.GetZaxis().SetTitleOffset(1.0)
            hist.GetZaxis().SetTitleFont(62)
            hist.GetZaxis().SetLabelSize(0.05)
            hist.GetZaxis().SetLabelFont(62)
            hist.GetZaxis().SetTitle('# of events')
            hist.SetContour(99)
            if Helper.drawOption == 'LEGO':                
                hist.GetXaxis().SetTitleSize(0.06)
                hist.GetYaxis().SetTitleSize(0.06)
                hist.GetYaxis().SetTitleOffset(1.2)
#            if Helper.drawOption == 'COLZ':
#                palette = TPaletteAxis(0.7950905,0.06660464,0.8516713,10.0728,hist)
#                palette.SetLabelColor(1)
#                palette.SetLabelFont(62)
#                palette.SetLabelOffset(0.005)
#                palette.SetLabelSize(0.04)
#                palette.SetTitleOffset(1)
#                palette.SetTitleSize(0.04)
#                palette.SetFillColor(1)
#                palette.SetFillStyle(1001) 
#                hist.GetListOfFunctions().Add(palette,"br")
                
        return hist
    setHistLabels = staticmethod(setHistLabels)
    
    def set_palette(name='', ncontours=999):
        """Set a color palette from a given RGB list
        stops, red, green and blue should all be lists of the same length
        see set_decent_colors for an example"""
        s = None
        r = None
        g = None
        b = None
        if name == "gray" or name == "grayscale":
            stops = [0.00, 0.34, 0.61, 0.84, 1.00]
            red   = [1.00, 0.84, 0.61, 0.34, 0.00]
            green = [1.00, 0.84, 0.61, 0.34, 0.00]
            blue  = [1.00, 0.84, 0.61, 0.34, 0.00]
    # elif name == "whatever":
        # (define more palettes)
        else:
        # default palette, looks cool
            stops = [0.00, 0.34, 0.61, 0.84, 1.00]
            red   = [0.00, 0.00, 0.87, 1.00, 0.51]
            green = [0.00, 0.81, 1.00, 0.20, 0.00]
            blue  = [0.51, 1.00, 0.12, 0.00, 0.00]
            
        s = array('d', stops)
        r = array('d', red)
        g = array('d', green)
        b = array('d', blue)


        npoints = len(s)
        TColor.CreateGradientColorTable(npoints, s, r, g, b, ncontours)
        gStyle.SetNumberContours(ncontours)
    set_palette = staticmethod(set_palette)

    
    def saveTH2(hist, filename, folder, legend=None, printAs=['eps', 'png']):      
        minX = hist.GetXaxis().GetXmin()
        maxX = hist.GetXaxis().GetXmax()
        dx = abs(maxX - minX)
        midX = maxX - dx/2
        x2 = maxX
        x1 = x2 *0.05
        minY = hist.GetYaxis().GetXmin()
        maxY = hist.GetYaxis().GetXmax()
        if Helper.drawOption == 'COLZ':
                
#                print 'y',min, max
                
#                palette = TPaletteAxis(x2*1.005,minY,x1+x2*1.005,maxY,hist)
                palette = TPaletteAxis(0.88,0.149,0.90,0.95,hist)
                palette.ConvertNDCtoPad()
#                palette.GetAxis().SetTextAlign(1)
                palette.SetLabelColor(1)
                palette.SetLabelFont(62)
#                palette.SetLabelOffset(-0.1)
                palette.SetLabelSize(0.04)
#                palette.SetTitleOffset(-2)
                palette.SetTitleSize(0.04)
                palette.SetLineColor(1)
                palette.SetFillColor(1)
                palette.SetFillStyle(1001) 
                hist.GetListOfFunctions().Add(palette,"br")
        cor = hist.GetCorrelationFactor(1,2)
        corr = 'CF: %1.4f' % cor
#        pt = TPaveText(midX,0.92*maxY,maxX - abs(maxX*0.2),0.98*maxY, "correlation factor") 
        pt = TPaveText(0.5,0.85,0.72,0.9, "NDC") 
        pt.SetFillColor(0)
        pt.SetTextSize(0.04) 
        pt.SetTextAlign(12)
        pt.AddText(corr.__str__())
            
                    
        if not os.path.exists(folder):
            os.mkdir(folder)
        pads = ps.PadService('testTH2', 'testingTH2', 1)
        pad = pads.Next()
        pad = Helper.setPadLayout(pad)
        if Helper.drawOption == 'COLZ':
            pad.SetRightMargin(0.13)
#        for i in pad.GetListOfPrimitives():
#            i.SetFillColor(0)
        hist.Draw(Helper.drawOption)
        pt.Draw()

        for i in printAs:
            if i in Helper.allowedFormats:
                pad.Print(folder + '/' + filename + '.' + i)
    saveTH2 = staticmethod(saveTH2)
    
    def setPadLayout(pad):
        pad.SetFillStyle(4000)
        pad.SetLeftMargin(0.15)
        pad.SetRightMargin(0.05)
        pad.SetBottomMargin(0.15)
        pad.SetTopMargin(0.05)
        return pad
    setPadLayout = staticmethod(setPadLayout)
        
    def saveHistsInOne(histlist, filename, folder, legend=None, logH=['0', '0'], err=False, printAs=['eps', 'png']):
        folder = folder.rstrip('/')
        f = ''
        for i in folder.split('/'):
            f = f + i + '/'
            if not os.path.exists(f):
                os.mkdir(f)
        pads = ps.PadService('test', 'testing', 1)
        pad = pads.Next()
        pad = Helper.setPadLayout(pad)
        if logH[0] == '1':
            pad.SetLogx(1)
        if logH[1] == '1':
            pad.SetLogy(1)
        mm = Helper.getMM(histlist)
        min = mm[0]
        max = mm[1]
        x = 0
        #do something else, if hist TH2F
        
        for hist in histlist:
            if "TH2F" in hist.__str__():
                Helper.saveTH2(hist, filename, folder, legend, printAs)
#                print "TH2F"
                break
                
            if not logH[1] == '1':
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
        if not "TH2F" in hist.__str__():
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
        if histconfig.opt['drawOption']:
            Helper.drawOption = histconfig.opt['drawOption']
        else: #reset drawOption
            Helper.drawOption = ''
        if "TH2F" in hist.__str__():
            if histconfig.opt['maxX'] and histconfig.opt['minX']:
#                min = hist.GetXaxis().GetXmin()
                min = float(histconfig.opt['minX'])
                max = float(histconfig.opt['maxX'])
                hist.SetAxisRange(min, max, 'X')
                
#            if histconfig.opt['minX']:
#                min = float(histconfig.opt['minX'])
#                max = hist.GetXaxis().GetXmax()
#                hist.SetAxisRange(min, max, 'X')
                
            if histconfig.opt['maxY'] and histconfig.opt['minY']:
#                min = hist.GetYaxis().GetXmin()
                min = float(histconfig.opt['minY'])
                max = float(histconfig.opt['maxY'])
                hist.SetAxisRange(min, max, 'Y')
                
#            if histconfig.opt['minY']:
#                min = float(histconfig.opt['minX'])
#                max = hist.GetYaxis().GetXmax()
#                hist.SetAxisRange(min, max, 'Y')
#            if histconfig.opt['minX']:
#                hist.GetXaxis().SetMinimum(float(histconfig.opt['minX']))
#            if histconfig.opt['maxY']:
#                 hist.GetYaxis().SetMaximum(float(histconfig.opt['maxY']))
#            if histconfig.opt['minY']:
#                hist.GetYaxis().SetMinimum(float(histconfig.opt['minY']))
             
#            hist.SetDrawOption(histconfig.opt['drawOption'])
        #set titles
        hist = Helper.setHistLabels(hist, histconfig.opt['titleX'], histconfig.opt['titleY'], histconfig.opt['rotate'].upper() == 'T')
        if var.opt['type'] == 'filled':
            hist = Helper.setFilled(hist, var.opt['color'], var.opt['size'], var.opt['style'])
        elif var.opt['type'] == 'marker':
            hist = Helper.setMarker(hist, var.opt['color'], var.opt['size'], var.opt['style'])
        else:
            hist = Helper.setLine(hist, var.opt['color'], var.opt['size'], var.opt['style'])
        
        hist.SetName(var.opt['name'])
        if var.opt['norm'] == 'Integral':
            hist.Scale(1 / hist.Integral())
        if histconfig.opt['max']:
            hist.SetMaximum(float(histconfig.opt['max']))
        if histconfig.opt['min']:
            hist.SetMinimum(float(histconfig.opt['min']))
            
        return hist
    applyHistConfig = staticmethod(applyHistConfig)
    
    def getMax(histlist):
        yScale = 1.0
        max = 0
        x = 0
        for i in histlist:
            mx = i.GetMaximum()
            if x == 0:
                max = mx
            else:
                if mx > max:
                    max = mx
            x += 1
        return max * yScale
        
    getMax = staticmethod(getMax)
    
    def getMin(histlist):
        yScale = 1.00
        min = 0
        x = 0
        for i in histlist:
            mn = i.GetMinimum()
            if x == 0:
                min = mn
            else:
                if mn < min:
                    min = mn
            x += 1
        if min > 0:
            min = min - min * (yScale - 1)
        else:
            min = min * yScale
        return min
    getMin = staticmethod(getMin)
        
    def getMM(histlist):
        yScale = 0.1
        min = Helper.getMin(histlist)
        max = Helper.getMax(histlist)
        dm = max - min
        max = max + dm * yScale
        nmin = min - dm * yScale
        if nmin > 0:
            min = nmin
            
        
        return [min, max]
    getMM = staticmethod(getMM)
        