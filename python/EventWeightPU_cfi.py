import FWCore.ParameterSet.Config as cms

eventWeightPU = cms.EDProducer("EventWeightPU",
                               
    PUSource          = cms.InputTag("addPileupInfo"),
    MCSampleFile      = cms.FileInPath("TopAnalysis/TopUtils/data/MC_PUDist_TTbar.root"),
    MCSampleHistoName = cms.string("pileup"),
    DataFile          = cms.FileInPath("TopAnalysis/TopUtils/data/Data_PUDist_160404-166861_7TeV_PromptReco_Collisions11.root"),
    DataHistoName     = cms.string("pileup"),          
    PUSysShiftUp      = cms.double(0.6),
    PUSysShiftDown    = cms.double(-0.6)                           

)

makeWeightsPU = cms.Sequence(eventWeightPU)
