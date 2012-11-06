import FWCore.ParameterSet.Config as cms

createMyPatJetCorrFactors = cms.EDProducer("MyPatJetCorrFactorsProducer",
                                           ## has to be derived from reco::Jet
                                           jetSrc = cms.InputTag("ak5GenJets")
                                           )
