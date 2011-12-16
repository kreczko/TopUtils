import FWCore.ParameterSet.Config as cms

effSFMuonEventWeight = cms.EDProducer("EffSFMuonEventWeight",
  particles  = cms.InputTag(""), ## jet collection (after jet selection, before b-tagging)
  sysVar   = cms.string(""),                  ## "noSys", "triggerEffSFNormUp/Down", "triggerEffSFShapeUp/Down", 
                                              ## "selectionEffSFNormUp/Down"
                                              ## "flatTriggerSF"
  verbose  = cms.int32(  0),                  ## set to 1 if terminal text output is desired
  filename = cms.string(""),                  ## if filename != "", the efficiencies are read from histos
                                              ## provided in that file
  additionalFactor = cms.double(1.),          ## result is multiplied with this additional factor
                                              ## (can be used for other flat SF)
  additionalFactorErr = cms.double(0.),       ## error
  meanTriggerEffSF    = cms.double(1.) ,      ## in case of flat SF and for trigger SF shape uncertainty calculation
  shapeDistortionFactor = cms.double(0.),     ## for shape uncertainty calculation (modifies difference between mean and actual SF)
  shapeVarPtThreshold  = cms.double(55.)      ## pt threshold which divides up/down variations SFShapeUp/DownPt
)
