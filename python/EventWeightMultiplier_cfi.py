import FWCore.ParameterSet.Config as cms

eventWeightMultiplier = cms.EDProducer("EventWeightMultiplier",
  eventWeightTags = cms.VInputTag(),         ## enter event weights as a vector of input tags
  verbose  = cms.int32(  0)                  ## set to 1 if terminal text output is desired
)