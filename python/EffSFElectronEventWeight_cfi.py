import FWCore.ParameterSet.Config as cms

effSFElectronEventWeight = cms.EDProducer("EffSFElectronEventWeight",
  particles  = cms.InputTag(""), ## electron collection
  sysVar   = cms.string(""),                  ## "noSys", "triggerEffSFNormUp/Down", "triggerEffSFShapeUpPt(Eta)/DownPt(Eta)", 
                                              ## "selectionEffSFNormUp/Down"
                                              ## "flatTriggerSF"
  shapeVarPtThreshold  = cms.double(55.),     ## pt threshold which divides up/down variations during SFShapeUp/DownPt
  shapeVarEtaThreshold = cms.double(0.7),     ## eta threshold which divides up/down variations during SFShapeUp/DownEta
  verbose  = cms.int32(  0),                  ## set to 1 if terminal text output is desired
  #filename = cms.string(""),                 ## if filename != "", the efficiencies are read from histos
                                              ## provided in that file
  additionalFactor = cms.double(1.),          ## result is multiplied with this additional factor
                                              ## (can be used for other flat SF)
  additionalFactorErr = cms.double(0.),       ## error
  meanTriggerEffSF    = cms.double(1.) ,      ## in case of flat SF
  meanTriggerEffSFErr = cms.double(0.) ,      ## in case of flat SF
  shapeDistortionFactor = cms.double(0.)      ## for shape uncertainty calculation
)
