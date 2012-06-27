import FWCore.ParameterSet.Config as cms

scaledElectronEnergy = cms.EDProducer("ElectronEnergyScale",
      src         = cms.InputTag("patElectrons"),
      mets        = cms.InputTag("patMETs"),
      binning     = cms.VPSet(
                      cms.PSet(
                          binSelection = cms.string('isEB'),
                          binUncertainty = cms.double(0.006)
                      ),
                      cms.PSet(
                          binSelection = cms.string('!isEB'),
                          binUncertainty = cms.double(0.015)
                      ),
      ),                         
      shiftBy = cms.double(0.)
)
