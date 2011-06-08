import FWCore.ParameterSet.Config as cms

bTagSFEventWeight = cms.EDProducer("BTagSFEventWeight",
  jets  = cms.InputTag("tightLeadingPFJets"), ## jet collection (after jet selection, before b-tagging)
  bTagAlgo = cms.string("SSVHEM"),            ## name of b tag algorithm
  sysVar   = cms.string(""),                  ## bTagSFUp, bTagSFDown, misTagSFUp, misTagSFDown possible;
                                              ## everything else: no systematic variation is made
  verbose  = cms.int32(  0),                  ## set to 1 if terminal text output is desired
  filename = cms.string("")                   ## if filename != "", the efficiencies are read from histos
                                              ## provided in that file
)
