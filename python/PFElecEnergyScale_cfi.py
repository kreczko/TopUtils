import FWCore.ParameterSet.Config as cms

correctLES = cms.EDProducer('PfElecLesCorr',
                            src=cms.InputTag("pfElectronsFromVertex"),
                            SFBarrel=cms.double(1.0),
                            SFEC    =cms.double(1.0),
                            eta     =cms.double(1.479)                            
)
