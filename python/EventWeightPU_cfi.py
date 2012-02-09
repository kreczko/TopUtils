import FWCore.ParameterSet.Config as cms

eventWeightPU = cms.EDProducer("EventWeightPU",
                               
    PUSource            = cms.InputTag("addPileupInfo"),
    WeightName          = cms.string("eventWeightPU"),                           
    Weight3DName        = cms.string("eventWeightPU3D"),                   
                               
    MCSampleFile        = cms.FileInPath("TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"),
    MCSampleHistoName   = cms.string("histo_Fall11_true"),
    DataFile            = cms.FileInPath("TopAnalysis/TopUtils/data/Data_PUDist_2011Full.root"),
    DataHistoName       = cms.string("histoData_true"),
 
    MCSample3DFile      = cms.FileInPath("TopAnalysis/TopUtils/data/MC_PUDist_Default2011.root"),
    MCSample3DHistoName = cms.string("histo_Fall11_true"),
    Data3DFile          = cms.FileInPath("TopAnalysis/TopUtils/data/Data_PUDist_2011Full.root"),
    Data3DHistoName     = cms.string("histoData_true_fineBinning"),                                

    CreateWeight3DHisto = cms.bool(False),
    Weight3DHistoFile   = cms.string("./data/DefaultWeight3D.root")
)
