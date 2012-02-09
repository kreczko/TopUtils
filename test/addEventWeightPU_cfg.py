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

process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring(
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/02719D6B-1398-E011-AA71-001A92971B94.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/02C07708-9898-E011-BC86-001A92971B0C.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/02C63308-8198-E011-8B1A-002618943896.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0412B109-8298-E011-A01C-00304867C0FC.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06167CD9-7798-E011-809D-0018F3D0967A.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06790363-7C98-E011-B101-0018F3D0968C.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0699073A-7C98-E011-848B-001A92971B94.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/069E0A93-F897-E011-9B35-0026189438AD.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06EC73E8-0E98-E011-9FA6-001A928116FA.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/06EE00FF-9F98-E011-9A0E-0018F3D0967E.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/085D13DC-F997-E011-99F1-001A92971B9C.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/089A3147-ED97-E011-968C-0026189438B3.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0C67147F-FC97-E011-8542-001A928116E0.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0E3B5039-FE97-E011-BBF5-001A92810A9E.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/0E590A33-7798-E011-894D-002354EF3BE1.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/10C8D92D-7898-E011-ABC7-0018F3D096AA.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/10F9FA3E-E397-E011-8DC1-003048678D6C.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/12015E8E-9998-E011-9793-00261894393A.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/1204EE92-F397-E011-99E8-003048679012.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/123FFA56-9298-E011-9BBE-002618943856.root",
    "/store/mc/Summer11/TTJets_TuneZ2_7TeV-madgraph-tauola/AODSIM/PU_S4_START42_V11-v1/0000/12C7067C-F297-E011-91BA-0026189438AF.root"
    ))

#### Number of Events

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
    )

#### Process Options

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

#### Definiton for Module

process.load("TopAnalysis.TopUtils.EventWeightPU_cfi")

process.eventWeightPU        = process.eventWeightPU.clone()
process.eventWeightPUsysUp   = process.eventWeightPU.clone()
process.eventWeightPUsysDown = process.eventWeightPU.clone()

#### Configuration for Nominal PU Weights

process.eventWeightPU.WeightName          = "eventWeightPU"
process.eventWeightPU.Weight3DName        = "eventWeightPU3D"
                     
process.eventWeightPU.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPU.MCSampleHistoName   = "histoSummer11_flat_observed"
process.eventWeightPU.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_EPS.root"
process.eventWeightPU.DataHistoName       = "histoData_observed"
                     
process.eventWeightPU.MCSample3DFile      = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPU.MCSample3DHistoName = "histoSummer11_flat_true"
process.eventWeightPU.Data3DFile          = "TopAnalysis/TopUtils/data/Data_PUDist_EPS.root"
process.eventWeightPU.Data3DHistoName     = "histoData_true_fineBinning"
                     
process.eventWeightPU.CreateWeight3DHisto = False
process.eventWeightPU.Weight3DHistoFile   = "./data/DefaultWeight3D.root"

#### Configuration for PU Up Variations

process.eventWeightPUsysUp.WeightName          = "eventWeightPUUp"
process.eventWeightPUsysUp.Weight3DName        = "eventWeightPU3DUp"
                     
process.eventWeightPUsysUp.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysUp.MCSampleHistoName   = "histoSummer11_flat_observed"
process.eventWeightPUsysUp.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_sysUp_EPS.root"
process.eventWeightPUsysUp.DataHistoName       = "histoData_observed"
                     
process.eventWeightPUsysUp.MCSample3DFile      = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysUp.MCSample3DHistoName = "histoSummer11_flat_true"
process.eventWeightPUsysUp.Data3DFile          = "TopAnalysis/TopUtils/data/Data_PUDist_sysUp_EPS.root"
process.eventWeightPUsysUp.Data3DHistoName     = "histoData_true_fineBinning"
                     
process.eventWeightPUsysUp.CreateWeight3DHisto = False
process.eventWeightPUsysUp.Weight3DHistoFile   = "./data/DefaultWeight3DUp.root"

#### Configuration for PU Down Variations

process.eventWeightPUsysDown.WeightName          = "eventWeightPUDown"
process.eventWeightPUsysDown.Weight3DName        = "eventWeightPU3DDown"
                     
process.eventWeightPUsysDown.MCSampleFile        = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysDown.MCSampleHistoName   = "histoSummer11_flat_observed"
process.eventWeightPUsysDown.DataFile            = "TopAnalysis/TopUtils/data/Data_PUDist_sysDown_EPS.root"
process.eventWeightPUsysDown.DataHistoName       = "histoData_observed"
                     
process.eventWeightPUsysDown.MCSample3DFile      = "TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"
process.eventWeightPUsysDown.MCSample3DHistoName = "histoSummer11_flat_true"
process.eventWeightPUsysDown.Data3DFile          = "TopAnalysis/TopUtils/data/Data_PUDist_sysDown_EPS.root"
process.eventWeightPUsysDown.Data3DHistoName     = "histoData_true_fineBinning"
                     
process.eventWeightPUsysDown.CreateWeight3DHisto = False
process.eventWeightPUsysDown.Weight3DHistoFile   = "./data/DefaultWeight3DDown.root"

#### Output Module


process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string("myOutputFile.root"),
                               outputCommands = cms.untracked.vstring('keep *')
#                                                                      'keep *_eventWeightPU*_*_*') 
                               )
#### Path

process.USER    = cms.Path(process.eventWeightPU * process.eventWeightPUsysUp * process.eventWeightPUsysDown)
process.outpath = cms.EndPath(process.out)



