import FWCore.ParameterSet.Config as cms

eventWeight = cms.EDProducer("EventWeight",
                             gridEff = cms.double(1.0),
                             weight = cms.InputTag("myPredefinedWeight")
                             )

makeWeights = cms.Sequence(cms.SequencePlaceholder("myPredefinedWeight") *
                           eventWeight
                           )

