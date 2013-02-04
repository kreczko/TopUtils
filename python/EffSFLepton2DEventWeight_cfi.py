import FWCore.ParameterSet.Config as cms

effSFLepton2DEventWeight = cms.EDProducer("EffSFLepton2DEventWeight",
  particles  = cms.InputTag(""), ## jet collection (after jet selection, before b-tagging)
  sysVar   = cms.string(""),                  ## "noSys", "combinedEffSFNormUpStat/Down", "combinedEffSFShapeUpEta(Pt)/Down", 
                                              ## "combinedEffSFNormUpSys/Down"
                                              ## "PUup", "PUdown"
                                              ## "flatTriggerSF"
  verbose  = cms.int32(  0),                  ## set to 1 if terminal text output is desired
  filename = cms.FileInPath("TopAnalysis/Configuration/data/MuonEffSF2D2012.root"),
                                              ## the efficiencies are read from histos provided in that file
  additionalFactor = cms.double(1.),          ## result is multiplied with this additional factor
                                              ## (can be used for other flat SF)
  additionalFactorErr = cms.double(0.),       ## error
  shapeDistortionFactor = cms.double(0.),     ## for shape uncertainty calculation: if >0 modifies difference between mean and actual SF
                                              ## if -1: up/down variations divided by certain threshold (shapeVarEtaThreshold)
  shapeVarEtaThreshold  = cms.double(0.7),    ## eta threshold which divides up/down variations SFShapeUp/DownEta
  shapeVarPtThreshold   = cms.double(55.)     ## pt threshold which divides up/down variations SFShapeUp/DownPt
)
