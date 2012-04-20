import FWCore.ParameterSet.Config as cms

effSFElectronEventWeight = cms.EDProducer("EffSFElectronEventWeight",
  electrons  = cms.InputTag(""), ## electron collection
  jets       = cms.InputTag(""), ## jet collection
  sysVar     = cms.string(""),                  ## "noSys", "combinedEffSFNormUp/Down Stat/Sys", "combinedEffSFShapeUpPt(Eta)/DownPt(Eta)", 
                                              ## "flatTriggerSF"
  shapeVarPtEleThreshold  = cms.double(55.),     ## pt threshold which divides up/down variations during SFShapeUp/DownPt
  shapeVarEtaEleThreshold = cms.double(0.7),     ## eta threshold which divides up/down variations during SFShapeUp/DownEta
  verbose  = cms.int32(  0),                  ## set to 1 if terminal text output is desired
  filenameJetLeg = cms.FileInPath(""),        ## filename for the jet leg eff. SF
                                              ## provided in that file
  additionalFactor = cms.double(1.),          ## result is multiplied with this additional factor
                                              ## (can be used for other flat SF)
  additionalFactorErr = cms.double(0.),       ## error
  meanTriggerEffSF    = cms.double(1.) ,      ## in case of flat SF
  meanTriggerEffSFErr = cms.double(0.) ,      ## in case of flat SF
  shapeDistortionErr  = cms.double(0.),      ## for shape uncertainty calculation
  jetTriggerEffsSFNormSysErr  = cms.double(0.),     ## syst. SF error for the jet leg (normalisation error)
  jetTriggerEffsSFShapeSysErr = cms.double(0.)     ## syst. SF error for the jet leg (shape error)
)
