import FWCore.ParameterSet.Config as cms

effSFMuonEventWeight = cms.EDProducer("EffSFMuonEventWeight",
  particles  = cms.InputTag(""), ## jet collection (after jet selection, before b-tagging)
  sysVar   = cms.string(""),                  ## EffSFUp, EffSFDown possible;
                                              ## everything else: no systematic variation is made
  verbose  = cms.int32(  0),                  ## set to 1 if terminal text output is desired
  filename = cms.string(""),                  ## if filename != "", the efficiencies are read from histos
                                              ## provided in that file
  additionalFactor = cms.double(1.)           ## result is multiplied with this additional factor
                                              ## (can be used for other flat SF)
)
