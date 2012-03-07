import FWCore.ParameterSet.Config as cms

# =============================
#  Cfg File for PU Reweighting
# =============================

process = cms.Process("USER")

# ================
#  Message Logger
# ================

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = 'INFO'
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# ==========================
#  Configuration of Process
# ==========================

#### Input Files

#process.load("TopAnalysis/Configuration/ttjets_MadgraphZ2_Summer11_AOD_cff")
process.load("TopAnalysis/Configuration/samples/Fall11_TTJets_TuneZ2_7TeV_madgraph_tauola_cff")
        
#process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/02719D6B-1398-E011-AA71-001A92971B94.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/02C07708-9898-E011-BC86-001A92971B0C.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/02C63308-8198-E011-8B1A-002618943896.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0412B109-8298-E011-A01C-00304867C0FC.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06167CD9-7798-E011-809D-0018F3D0967A.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06790363-7C98-E011-B101-0018F3D0968C.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0699073A-7C98-E011-848B-001A92971B94.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/069E0A93-F897-E011-9B35-0026189438AD.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06EC73E8-0E98-E011-9FA6-001A928116FA.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06EE00FF-9F98-E011-9A0E-0018F3D0967E.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/085D13DC-F997-E011-99F1-001A92971B9C.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/089A3147-ED97-E011-968C-0026189438B3.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0C67147F-FC97-E011-8542-001A928116E0.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0E3B5039-FE97-E011-BBF5-001A92810A9E.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0E590A33-7798-E011-894D-002354EF3BE1.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/10C8D92D-7898-E011-ABC7-0018F3D096AA.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/10F9FA3E-E397-E011-8DC1-003048678D6C.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/12015E8E-9998-E011-9793-00261894393A.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/1204EE92-F397-E011-99E8-003048679012.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/123FFA56-9298-E011-9BBE-002618943856.root",
#    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/12C7067C-F297-E011-91BA-0026189438AF.root"
#    ))

#### Number of Events

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
    )

#### Process Options

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

#### Definiton for Module

process.load("TopAnalysis.TopUtils.EventWeightPU_cfi")

process.eventWeightPUFall11          = process.eventWeightPU.clone()
process.eventWeightPUSummer11        = process.eventWeightPU.clone()
process.eventWeightPUsysUpFall11     = process.eventWeightPU.clone()
process.eventWeightPUsysUpSummer11   = process.eventWeightPU.clone()
process.eventWeightPUsysDownFall11   = process.eventWeightPU.clone()
process.eventWeightPUsysDownSummer11 = process.eventWeightPU.clone()

#### =================================================
#### Configuration for Nominal PU Weights (Fall11)
#### =================================================

process.eventWeightPUFall11.WeightName          = "eventWeightPU"
process.eventWeightPUFall11.MCSampleTag         = "Fall11"
                     
process.eventWeightPUFall11.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUFall11.MCSampleHistoName   = "histo_Fall11_true"
process.eventWeightPUFall11.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_EPS.root"
process.eventWeightPUFall11.DataHistoName       = "histoData_true"
                     
process.eventWeightPUFall11.CreateWeight3DHisto = False
process.eventWeightPUFall11.Weight3DHistoFile   = "TopAnalysis/TopUtils/data/DefaultWeight3D.root"

#### =================================================
#### Configuration for Nominal PU Weights (Summer11)
#### =================================================

process.eventWeightPUSummer11.WeightName          = "eventWeightPU"
process.eventWeightPUSummer11.MCSampleTag         = "Summer11"
                  
process.eventWeightPUSummer11.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUSummer11.MCSampleHistoName   = "histoSummer11_flat_true"
process.eventWeightPUSummer11.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_EPS.root"
process.eventWeightPUSummer11.DataHistoName       = "histoData_true_fineBinning"
                  
process.eventWeightPUSummer11.CreateWeight3DHisto = False
process.eventWeightPUSummer11.Weight3DHistoFile   = "TopAnalysis/TopUtils/data/DefaultWeight3D.root"

#### =================================================
####  Configuration for PU Up Variations (Fall11)
#### =================================================

process.eventWeightPUsysUpFall11.WeightName          = "eventWeightPUUp"
process.eventWeightPUsysUpFall11.MCSampleTag         = "Fall11"
                        
process.eventWeightPUsysUpFall11.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysUpFall11.MCSampleHistoName   = "histo_Fall11_true"
process.eventWeightPUsysUpFall11.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_sysUp_EPS.root"
process.eventWeightPUsysUpFall11.DataHistoName       = "histoData_true"
                        
process.eventWeightPUsysUpFall11.CreateWeight3DHisto = False
process.eventWeightPUsysUpFall11.Weight3DHistoFile   = "TopAnalysis/TopUtils/data/DefaultWeight3DUp.root"

#### =================================================
#### Configuration for PU Up Variations (Summer11)
#### =================================================

process.eventWeightPUsysUpSummer11.WeightName          = "eventWeightPUUp"
process.eventWeightPUsysUpSummer11.MCSampleTag         = "Summer11"

process.eventWeightPUsysUpSummer11.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysUpSummer11.MCSampleHistoName   = "histoSummer11_flat_true"
process.eventWeightPUsysUpSummer11.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_sysUp_EPS.root"
process.eventWeightPUsysUpSummer11.DataHistoName       = "histoData_true_fineBinning"

process.eventWeightPUsysUpSummer11.CreateWeight3DHisto = False
process.eventWeightPUsysUpSummer11.Weight3DHistoFile   = "TopAnalysis/TopUtils/data/DefaultWeight3DUp.root"

#### =================================================
#### Configuration for PU Down Variations (Fall11)
#### =================================================

process.eventWeightPUsysDownFall11.WeightName          = "eventWeightPUDown"
process.eventWeightPUsysDownFall11.MCSampleTag         = "Fall11"
                          
process.eventWeightPUsysDownFall11.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysDownFall11.MCSampleHistoName   = "histo_Fall11_true"
process.eventWeightPUsysDownFall11.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_sysDown_EPS.root"
process.eventWeightPUsysDownFall11.DataHistoName       = "histoData_true"
                          
process.eventWeightPUsysDownFall11.CreateWeight3DHisto = False
process.eventWeightPUsysDownFall11.Weight3DHistoFile   = "TopAnalysis/TopUtils/data/DefaultWeight3DDown.root"

#### =================================================
#### Configuration for PU Down Variations (Summer11)
#### =================================================

process.eventWeightPUsysDownSummer11.WeightName          = "eventWeightPUDown"
process.eventWeightPUsysDownSummer11.MCSampleTag         = "Summer11"
 
process.eventWeightPUsysDownSummer11.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysDownSummer11.MCSampleHistoName   = "histoSummer11_flat_true"
process.eventWeightPUsysDownSummer11.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_sysDown_EPS.root"
process.eventWeightPUsysDownSummer11.DataHistoName       = "histoData_true_fineBinning"
 
process.eventWeightPUsysDownSummer11.CreateWeight3DHisto = False
process.eventWeightPUsysDownSummer11.Weight3DHistoFile   = "TopAnalysis/TopUtils/data/DefaultWeight3DDown.root"

#### Output Module


process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string("myOutputFile.root"),
                               outputCommands = cms.untracked.vstring('drop *',
                                                                      'keep *_eventWeightPU*_*_*') 
                               )
#### Path

process.USER    = cms.Path(process.eventWeightPUFall11         *
                           process.eventWeightPUSummer11       *
                           process.eventWeightPUsysUpFall11    *
                           process.eventWeightPUsysUpSummer11  *
                           process.eventWeightPUsysDownFall11  *
                           process.eventWeightPUsysDownSummer11)

process.outpath = cms.EndPath(process.out)



