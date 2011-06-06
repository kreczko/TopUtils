import FWCore.ParameterSet.Config as cms

eventWeightPU = cms.EDProducer("EventWeightPU",
                               
    PUSource          = cms.InputTag("addPileupInfo"),
    MCSampleFile      = cms.string("TopAnalysis/TopUtils/test/MC_PUDist.root"),
    MCSampleHistoName = cms.string("pileup"),
    DataFile          = cms.string("TopAnalysis/TopUtils/test/Data_PUDist_110527.root"),
    DataHistoName     = cms.string("pileup")          
)

makeWeightsPU = cms.Sequence(eventWeightPU)
