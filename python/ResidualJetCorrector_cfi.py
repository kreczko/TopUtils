import FWCore.ParameterSet.Config as cms

residualCorrectedJets = cms.EDProducer("ResidualJetCorrector",
                                       ## input source of pat::Jets to be corrected
                                       jets    = cms.InputTag("selectedPatJets"),
                                       ## type of jet to be corrected, 3 possibilities:
                                       ## Calo, PF, JPT
                                       jetType = cms.string("Calo")
                                       )
