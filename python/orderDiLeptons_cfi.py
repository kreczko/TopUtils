import FWCore.ParameterSet.Config as cms

orderedDiLeptons = cms.EDProducer('OrderDiLeptons',
                      firstsrc  = cms.InputTag("dielectrons"),
                      secondsrc = cms.InputTag("dimuons"),
                      thirdsrc  = cms.InputTag("dimue")                     
)
