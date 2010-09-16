import FWCore.ParameterSet.Config as cms

residualCorrectedJets = cms.EDProducer("ResidualJetCorrector",
                                       ## input source of pat::Jets to be corrected
                                       jets    = cms.InputTag("selectedPatJets"),
                                       ## type of jet to be corrected, 3 possibilities:
                                       ## PF  : Spring10DataV2_L2L3Residual_AK5PF.txt
                                       ## Calo: Spring10DataV2_L2L3Residual_AK5Calo.txt
                                       ## JPT : Spring10DataV2_L2L3Residual_AK5JPT.txt
                                       corrections = cms.string("Spring10DataV2_L2L3Residual_AK5Calo.txt")
                                       )
