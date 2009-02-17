from ROOT import TLegend, TBox, TColor, gStyle, TPaletteAxis, TPaveText, TPaveStats, TF1, TAxis
import PadService as ps
import os
from math import log
from array import array

class Helper:
    """
    Tool for the layout of histograms
    """
    drawOption = ''
    varOption = ""
    allowedFormats = ['eps', 'ps', 'pdf', 'png', 'jpg']
    def makePlainLegend(x, y, sizeX, sizeY):
        x = float(x) / 100
        y = float(y) / 100
        sizeX = float(sizeX) / 100
        sizeY = float(sizeY) / 100
        leg = TLegend(x, y, x + sizeX, y+sizeY) 
        leg = Helper.setLegendStyle(leg)
        return leg
        
    makePlainLegend = staticmethod(makePlainLegend)
        
    def setLegendStyle(leg):
        leg.SetFillStyle(0)
        leg.SetFillColor(0)
        leg.SetBorderSize(0)
        leg.SetLineWidth(8)
        return leg
    setLegendStyle = staticmethod(setLegendStyle)
    
    def setHistLabels(hist, titleX, titleY, rotate=False):
        hist.SetTitle("")
#        if not  ["TF1", "TGraph"] in hist.__str__() :
        if not 1 in [c in hist.__str__() for c in ["TF1", "TGraph"] ]:
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
        hist.GetYaxis().RotateTitle(rotate)
        
        if Helper.drawOption and "TH2F" in hist.__str__():
            hist.GetZaxis().SetTitleSize(0.07)
            hist.GetZaxis().SetTitleColor(1)
            hist.GetZaxis().SetTitleOffset(1.0)
            hist.GetZaxis().SetTitleFont(62)
            hist.GetZaxis().SetLabelSize(0.05)
            hist.GetZaxis().SetLabelFont(62)
            if "Matrix" in hist.GetName():
                hist.GetZaxis().SetTitle('correlation factor')
                hist.GetYaxis().SetTitleOffset(-0.5)
            else:
                hist.GetZaxis().SetTitle('# of events')
            hist.SetContour(99)
            if Helper.drawOption == 'LEGO':                
                hist.GetXaxis().SetTitleSize(0.06)
                hist.GetYaxis().SetTitleSize(0.06)
                hist.GetYaxis().SetTitleOffset(1.2)
                
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
        elif name =="simple":
            stops = [0.00, 0.25, 0.50, 0.75, 1.00 ]
            red   = [0.00, 0.00, 1.00, 1.00, 1.00 ]
            green = [0.00, 1.00, 1.00, 1.00, 0.00 ]
            blue  = [1.00, 1.00, 1.00, 0.00, 0.00 ]
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
        pt = None
        if not "norm_ProcMatrix" in filename:
            pt = TPaveText(0.5,0.85,0.72,0.9, "NDC") 
            pt.SetFillColor(0)
            pt.SetTextSize(0.04) 
            pt.SetTextAlign(12)
            pt.AddText(corr.__str__())
        else:
            
            hist.GetZaxis().SetTitle('correlation factor')
            hist.GetXaxis().SetBinLabel(1, "|#DeltaPhi(MET, #mu)|")
            hist.GetXaxis().SetBinLabel(2, "|#Sigma#DeltaPhi(#mu, jet1,2)|")
            hist.GetXaxis().SetBinLabel(3, "p_{T}(#mu)")
            hist.GetXaxis().SetBinLabel(4, "circularity")
            
            hist.GetYaxis().SetBinLabel(1, "|#DeltaPhi(MET, #mu)|")
            hist.GetYaxis().SetBinLabel(2, "|#Sigma#DeltaPhi(#mu, jet1,2)|")
            hist.GetYaxis().SetBinLabel(3, "p_{T}(#mu)")
            hist.GetYaxis().SetBinLabel(4, "circularity")
#            hist.GetYaxis().SetBit(TAxis.kLabelsDown)
            hist.GetYaxis().SetLabelSize(0.04)
            hist.GetYaxis().CenterTitle(True)
           
            
            
                    
        if not os.path.exists(folder):
            os.mkdir(folder)
        pads = ps.PadService('testTH2', 'testingTH2', 1)
        pad = pads.Next()
        pad = Helper.setPadLayout(pad)
        if Helper.drawOption == 'COLZ':
            pad.SetRightMargin(0.13)
        if "norm_ProcMatrix" in filename:
            pad.SetLeftMargin(0.2)
#        for i in pad.GetListOfPrimitives():
#            i.SetFillColor(0)
        hist.Draw(Helper.drawOption)
        if not "norm_ProcMatrix" in filename:
            pt.Draw()

        for i in printAs:
            if i in Helper.allowedFormats:
                pad.Print(folder + '/' + filename + '.' + i)
    saveTH2 = staticmethod(saveTH2)
    
    def setPadLayout(pad):
        pad.SetFillStyle(4000)
        pad.SetLeftMargin(0.17)
        pad.SetRightMargin(0.05)
        pad.SetBottomMargin(0.15)
        pad.SetTopMargin(0.05)
        return pad
    setPadLayout = staticmethod(setPadLayout)
        
#================================================================================
# TODO: change to accepting histconfig as a parameter and doing applyConfig by its own
#================================================================================
        
    def saveHistsInOne(histlist, histCfg, savedir, printAs=['eps', 'png']):
        filename = histCfg.opt['name']        
        folder = savedir + histCfg.opt['savefolder']
        logH = [histCfg.opt['logX'], histCfg.opt['logY']]
        err = histCfg.opt['showErrors'].upper() == 'TRUE'
        legend = histCfg.legend
        if legend:
                legend = Helper.makePlainLegend(legend.opt['posX'], legend.opt['posY'], legend.opt['sizeX'], legend.opt['sizeX'])
                
        filename = filename.replace('.','')    
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
        for histkey in histlist.keys():
            hist = histlist[histkey]
#        for hist in histlist.itervalues():
            if "TH2F" in hist.__str__():
                Helper.saveTH2(hist, filename, folder, legend, printAs)
#                print "TH2F"
                break
            
#===============================================================================
#            fit is not implemented yet, therefore a dirty hack is used
#            the code below is from the root macro created for the fit
#===============================================================================
            if filename == 'MET_reso':
                ptstats = TPaveStats(0.67,0.45,0.97,1,"brNDC")
                ptstats.SetName("stats")
                ptstats.SetBorderSize(2)
                ptstats.SetFillColor(0)
                ptstats.SetTextAlign(12)
                text = ptstats.AddText("#chi^{2} / ndf = 4.669e+07 / 197")
                text = ptstats.AddText("Constant = 4.846e+05 #pm 11 ")
                text = ptstats.AddText("Mean     = -3.121 #pm 0.001 ")
                text = ptstats.AddText("Sigma    = 20.84 #pm 0.00 ")
                ptstats.SetOptStat(0)
                ptstats.SetOptFit(1)
                ptstats.Draw()
                hist.GetListOfFunctions().Add(ptstats)
                ptstats.SetParent(hist.GetListOfFunctions())
   
                lastFitFunc =TF1("lastFitFunc","gaus",-100,100)
                lastFitFunc.SetFillColor(19)
                lastFitFunc.SetFillStyle(0)
                lastFitFunc.SetLineWidth(3)
                lastFitFunc.SetChisquare(4.668531e+07)
                lastFitFunc.SetNDF(197)
                lastFitFunc.SetParameter(0,484642)
                lastFitFunc.SetParError(0,10.74295)
                lastFitFunc.SetParLimits(0,0,0)
                lastFitFunc.SetParameter(1,-3.120982)
                lastFitFunc.SetParError(1,0.000531762)
                lastFitFunc.SetParLimits(1,0,0)
                lastFitFunc.SetParameter(2,20.83625)
                lastFitFunc.SetParError(2,0.0005538997)
                lastFitFunc.SetParLimits(2,0,234.9833)
                hist.Fit(lastFitFunc, "WW")
                hist.GetListOfFunctions().Add(lastFitFunc)
#                
            if not logH[1] == '1':
                hist.SetMaximum(max)
                hist.SetMinimum(min)
            if legend:
                legend.AddEntry(hist, hist.GetName())
            
            if x == 0:
                if not histCfg.opt["drawOption"] == "":
                    hist.Draw(histCfg.opt["drawOption"])
                elif histCfg.opt["drawOption"] == "" and not histCfg.getVarByName(histkey).opt["drawOption"] == "":
                    hist.SetFillColor(hist.GetLineColor())
                    hist.Draw(histCfg.getVarByName(histkey).opt["drawOption"])
                elif err and Helper.drawOption == "":
                    hist.Draw('e')
                else:
#                    hist.SetError(array('d', noerr))
                    if filename in ['quality_QCDLoose_Jcuts', 'quality_QCDLoose_Mcuts', 'quality_QCDTight_Jcuts', 'quality_QCDTight_Mcuts', 
                                    'bbs_QCDLoose_Jcuts', 'bbs_QCDTight_Jcuts', 'bbs_QCDLoose_Mcuts', 'bbs_QCDTight_Mcuts']:
                        hist.Draw('HISTP')
                    else:
                        hist.Draw()
            else:
                if not histCfg.opt["drawOption"] == "":
                        hist.Draw("same"+histCfg.opt["drawOption"] )
                elif histCfg.opt["drawOption"] == "" and not histCfg.getVarByName(histkey).opt["drawOption"] == "":
                    hist.SetFillColor(hist.GetLineColor())
                    hist.Draw("same"+histCfg.getVarByName(histkey).opt["drawOption"])
                elif err and histCfg.opt["drawOption"] == "":
                    hist.Draw('samee')
                else:
#                    hist.SetError(array('d',noerr))
                    if filename in ['quality_QCDLoose_Jcuts', 'quality_QCDLoose_Mcuts', 'quality_QCDTight_Jcuts', 'quality_QCDTight_Mcuts', 
                                    'bbs_QCDLoose_Jcuts', 'bbs_QCDTight_Jcuts', 'bbs_QCDLoose_Mcuts', 'bbs_QCDTight_Mcuts']:
                        hist.Draw('HISTPsame')
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
        hist.SetOption("P");
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
        return hist
    setFilled = staticmethod(setFilled)   
    
    def applyHistConfig(hist, histconfig, var):
        noerr = []

        if not histconfig.opt['smooth'] =="0":
            hist.Smooth(int(histconfig.opt['smooth']))
        if histconfig.opt['drawOption']:
            Helper.drawOption = histconfig.opt['drawOption']
        else: #reset drawOption
            Helper.drawOption = ''
        min = None
        max = None
#------------------------------------------------------------------------------ X axis
        if histconfig.opt['maxX']:
            max = float(histconfig.opt['maxX'])
        if histconfig.opt['minX']:
            min = float(histconfig.opt['minX'])
        if min == None:
            min = hist.GetXaxis().GetXmin()
        if max == None:
            max = hist.GetXaxis().GetXmax()
        
        #this "if" prevents the destruction of the root intern automatic scaling
        if histconfig.opt['maxX'] or histconfig.opt['minX']:
#------------------------------------------------------------------------------ change to GetXaxis().SetRangeUser(min, max)?
            hist.SetAxisRange(min, max, 'X')
#------------------------------------------------------------------------------ Y axis
        if histconfig.opt['maxY']:# and histconfig.opt['minX']:
            max = float(histconfig.opt['maxY'])
        if histconfig.opt['minY']:# and histconfig.opt['minX']:
            min = float(histconfig.opt['minY'])
        if min == None:
            min = hist.GetYaxis().GetXmin()
        if max == None:
            max = hist.GetYaxis().GetXmax()
        if histconfig.opt['maxY'] or histconfig.opt['minY']:
            hist.SetAxisRange(min, max, 'Y')
#------------------------------------------------------------ set titles
        hist = Helper.setHistLabels(hist, histconfig.opt['titleX'], histconfig.opt['titleY'], histconfig.opt['rotate'].upper() == 'TRUE')
        if var.opt['type'] == 'filled':
            hist = Helper.setFilled(hist, var.opt['color'], var.opt['size'], var.opt['style'])
        elif var.opt['type'] == 'marker':
            hist = Helper.setMarker(hist, var.opt['color'], var.opt['size'], var.opt['style'])
        else:
            hist = Helper.setLine(hist, var.opt['color'], var.opt['size'], var.opt['style'])
        
        hist.SetName(var.opt['name'])
#===============================================================================
# norm it --TODO change it to scale, so floats AND Integral can be used
#===============================================================================
        if var.opt['norm'] == 'Integral':
            if not hist.Integral()==0:
                hist.Scale(1 / hist.Integral())
            
        return hist
    applyHistConfig = staticmethod(applyHistConfig)
    
    def getMax(histlist):
        yScale = 1.0
        max = 0
        x = 0
        for i in histlist.itervalues():
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
        for i in histlist.itervalues():
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
        