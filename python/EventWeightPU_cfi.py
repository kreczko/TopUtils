import FWCore.ParameterSet.Config as cms

eventWeightPU = cms.EDProducer("EventWeightPU",
                               
    PUSource            = cms.InputTag("addPileupInfo"),
    WeightName          = cms.string("eventWeightPU"),                           

    MCSampleTag         = cms.string("Fall11"),  # valid identifier: Fall11, Summer11                               
                               
    MCSampleFile        = cms.FileInPath("TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"),
    MCSampleHistoName   = cms.string("histo_Fall11_true"),
    DataFile            = cms.FileInPath("TopAnalysis/TopUtils/data/Data_PUDist_2011Full.root"),
    DataHistoName       = cms.string("histoData_true"),

    CreateWeight3DHisto = cms.bool(False),
    Weight3DHistoFile   = cms.FileInPath("TopAnalysis/TopUtils/data/DefaultWeight3D.root")
)
