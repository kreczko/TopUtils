import FWCore.ParameterSet.Config as cms

eventWeight = cms.EDFilter("EventWeightPlain",
                           nevts = cms.uint32(1),
                           xsec  = cms.double(1.0),
                           eff   = cms.double(1.0),
                           lumi  = cms.double(1.0)
                           )

makeWeights = cms.Sequence(eventWeight)

