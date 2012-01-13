import FWCore.ParameterSet.Config as cms

eventWeightMC = cms.EDProducer("EventWeightMC")

makeWeightsMC = cms.Sequence(eventWeightMC)
