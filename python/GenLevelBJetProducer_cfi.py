import FWCore.ParameterSet.Config as cms

produceGenLevelBJets = cms.EDProducer("GenLevelBJetProducer",
    ttGenEvent = cms.InputTag('genEvt'),
#    genJets = cms.InputTag('ak5GenJets','','SIM'),
#    genJets = cms.InputTag('ak5GenJets','',''),
    genJets = cms.InputTag('ak5GenJetsPlusHadron','',''),
    deltaR = cms.double(0.5),
    flavour = cms.int32(5),
    resolveParticleName = cms.bool(False),
    requireTopBquark = cms.bool(False),
    noBBbarResonances = cms.bool(True),
    doImprovedHadronMatching = cms.bool(False),
    doValidationPlotsForImprovedHadronMatching = cms.bool(False)
)



