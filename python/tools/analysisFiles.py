#only for personal use
folder =  '/playground/rootfiles/FINAL/'

def setFolder(dict):
    for i in dict.keys():
        dict[i] = folder + dict[i]
        
qcdfilesJ = {'20': 'qcdmu_20j20m.root',
'25':'qcdmu_25j20m.root',
#MM_qcdmu_valid_final_30.30.60.75j30m.root
'30':'qcdmu_30j20m.root',
'35':'qcdmu_35j20m.root',
'40':'qcdmu_40j20m.root'}


qcdfilesM = {'20': 'qcdmu_20j20m.root',
'22.5':'qcdmu_20j22.5m.root',
'25':'qcdmu_20j25m.root',
'27.5':'qcdmu_20j27.5m.root',
'30':'qcdmu_20j30m.root'}

topfilesJ = {'20': 'top_20j20m.root',
'25':'top_25j20m.root',
#MM_top_valid_final_30.30.60.75j30m.root
'30':'top_30j20m.root',
'35':'top_35j20m.root',
'40':'top_40j20m.root'}

topfilesM = {'20': 'top_20j20m.root',
'22.5':'top_20j22.5m.root',
'25':'top_20j25m.root',
'27.5':'top_20j27.5m.root',
'30':'top_20j30m.root'}

wjetsfilesJ = {'20': 'wjets_20j20m.root',
'25':'wjets_25j20m.root',
#MM_wjets_valid_final_30.30.60.75j30m.root
'30':'wjets_30j20m.root',
'35':'wjets_35j20m.root',
'40':'wjets_40j20m.root'}

wjetsfilesM = {'20': 'wjets_20j20m.root',
'22.5':'wjets_20j22.5m.root',
'25':'wjets_20j25m.root',
'27.5':'wjets_20j27.5m.root',
'30':'wjets_20j30m.root'}
#
#shapesUp = {"pt":"qcdMu_PtUp", "circularity":"qcdMu_circUp", "dPhiMJ12":"qcdMu_MJ12Up", "dPhiMETMu":"qcdMu_METMuUp"}
#shapesDown =  {"pt":"qcdMu_PtDown", "circularity":"qcdMu_PtDown", "dPhiMJ12":"qcdMu_MJ12Down", "dPhiMETMu":"qcdMu_METMuDown"}
shapeFiles = {"MuPtSIG":"shapes/qcdMu_PtSIG.root", 
              "MuPtBG":"shapes/qcdMu_PtBG.root", 
              "circularitySIG":"shapes/qcdMu_CircSIG.root", 
              "circularityBG":"shapes/qcdMu_CircBG.root", 
              "dPhiMJ12SIG":"shapes/qcdMu_MJ12SIG.root",
              "dPhiMJ12BG":"shapes/qcdMu_MJ12BG.root", 
              "dPhiMETMuSIG":"shapes/qcdMu_METMuSIG.root",   
              "dPhiMETMuBG":"shapes/qcdMu_METMuBG.root"
              }
#qcdCfilesJ = {'20': 'MM_qcdmu_270109.root',
#'25':'MM_qcdmu_calib_final_25j20m.root',
##MM_qcdmu_calib_final_30.30.60.75j30m.root
#'30':'MM_qcdmu_calib_final_30j20m.root',
#'35':'MM_qcdmu_calib_final_35j20m.root',
#'40':'MM_qcdmu_calib_final_40j20m.root'}
#
#qcdCfilesM = {'20': 'MM_qcdmu_270109.root',
#'22.5':'MM_qcdmu_calib_final_20j22.5m.root',
#'25':'MM_qcdmu_calib_final_20j25m.root',
#'27.5':'MM_qcdmu_calib_final_20j27.5m.root',
#'30':'MM_qcdmu_calib_final_20j30m.root'}
#
#topCfilesJ = {'20': 'MM_top_270109.root',
#'25':'MM_top_calib_final_25j20m.root',
##MM_top_calib_final_30.30.60.75j30m.root
#'30':'MM_top_calib_final_30j20m.root',
#'35':'MM_top_calib_final_35j20m.root',
#'40':'MM_top_calib_final_40j20m.root'}
#
#topCfilesM = {'20': 'MM_top_270109.root',
#'22.5':'MM_top_calib_final_20j22.5m.root',
#'25':'MM_top_calib_final_20j25m.root',
#'27.5':'MM_top_calib_final_20j27.5m.root',
#'30':'MM_top_calib_final_20j30m.root'}
#
#wjetsCfilesJ = {'20': 'MM_wjets_270109.root',
#'25':'MM_wjets_calib_final_25j20m.root',
##MM_wjets_calib_final_30.30.60.75j30m.root
#'30':'MM_wjets_calib_final_30j20m.root',
#'35':'MM_wjets_calib_final_35j20m.root',
#'40':'MM_wjets_calib_final_40j20m.root'}
#
#wjetsCfilesM = {'20': 'MM_wjets_270109.root',
#'22.5':'MM_wjets_calib_final_20j22.5m.root',
#'25':'MM_wjets_calib_final_20j25m.root',
#'27.5':'MM_wjets_calib_final_20j27.5m.root',
#'30':'MM_wjets_calib_final_20j30m.root'}

setFolder(qcdfilesJ)
setFolder(topfilesJ)
setFolder(wjetsfilesJ)
setFolder(qcdfilesM)
setFolder(topfilesM)
setFolder(wjetsfilesM)
setFolder(shapeFiles)
#setFolder(shapesDown)

#setFolder(qcdCfilesM)
#setFolder(topCfilesM)
#setFolder(wjetsCfilesM)
#setFolder(qcdCfilesJ)
#setFolder(topCfilesJ)
#setFolder(wjetsCfilesJ)
