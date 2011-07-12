import FWCore.ParameterSet.Config as cms

eventWeightPU = cms.EDProducer("EventWeightPU",
                               
    PUSource          = cms.InputTag("addPileupInfo"),
    MCSampleFile      = cms.FileInPath("TopAnalysis/TopUtils/data/MC_PUDist_Summer11_TTJets_TuneZ2_7TeV_madgraph_tauola.root"),
    MCSampleHistoName = cms.string("MCPUDistribution/pileup"),
    DataFile          = cms.FileInPath("TopAnalysis/TopUtils/data/Data_PUDist_160404-163869_7TeV_May10ReReco_Collisions11_v2_and_165088-167913_7TeV_PromptReco_Collisions11.root"),
    DataHistoName     = cms.string("pileup"),          
    PUSysShiftUp      = cms.double(0.6),
    PUSysShiftDown    = cms.double(-0.6)                           

)

makeWeightsPU = cms.Sequence(eventWeightPU)
