import FWCore.ParameterSet.Config as cms

bTagSFEventWeight = cms.EDProducer("BTagSFEventWeight",
  jets  = cms.InputTag("tightLeadingPFJets"), ## jet collection (after jet selection, before b-tagging)
  bTagAlgo = cms.string("SSVHEM"),            ## name of b tag algorithm
  sysVar   = cms.string(""),                  ## bTagSFUp, bTagSFDown, misTagSFUp, misTagSFDown possible;
                                              ## bTagSFShapeUpPt, bTagSFShapeDownPt, bTagSFShapeUpEta, bTagSFShapeDownEta,
                                              ## everything else: no systematic variation is made
  shapeVarPtThreshold  = cms.double(65.),     ## pt threshold which divides up/down variations during bTagSFShapeUp/DownPt
  shapeVarEtaThreshold = cms.double(0.7),     ## eta threshold which divides up/down variations during bTagSFShapeUp/DownEta
  uncertaintySFb = cms.double(-1),            ## uncertainty of SFb (0.05 means 5%); if set to <0, the values from the b-tag DB are taken
  shapeDistortionFactor = cms.double(0.5),    ## for shape uncertainty calculation (fraction of normalisation uncertainty)
  verbose  = cms.int32(  0),                  ## set to 1 if terminal text output is desired
  filename = cms.FileInPath("")               ## if filename != "", the efficiencies are read from histos
                                              ## provided in that file
)
