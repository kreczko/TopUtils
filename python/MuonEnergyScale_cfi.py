import FWCore.ParameterSet.Config as cms

scaledMuonEnergy = cms.EDProducer("MuonEnergyScale",
      src         = cms.InputTag("patMuons"),
      mets        = cms.InputTag("patMETs"),
      uncertainty = cms.double(0.01),
      shiftBy     = cms.double(0.)
)
