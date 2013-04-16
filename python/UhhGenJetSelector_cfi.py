import FWCore.ParameterSet.Config as cms

uhhSelectedGenJets = cms.EDProducer("UhhGenJetSelector",
  ## input collection (should be of type reco::GenJet)
  genJet = cms.InputTag("reco::GenJet"),
  ## indices of relevant entries within src
  ## from input tags to get the (anti)-b-jet indices as created by
  ## TopAnalysis/TopUtils/GenLevelBJetProducer
  ## please adapt if cloning and renaming produceGenLevelBJets!
  BHadJetIndex     = cms.InputTag("produceGenLevelBJets", "BHadJetIndex"    ),
  AntiBHadJetIndex = cms.InputTag("produceGenLevelBJets", "AntiBHadJetIndex"),
  ## min pt requirement
  pt=cms.double(0.),
  ## max eta requirement
  eta=cms.double(999.)
)
