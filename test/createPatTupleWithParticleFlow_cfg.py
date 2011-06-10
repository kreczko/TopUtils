## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

## run the particle flow classification as far as appropriate
process.load("TopAnalysis.TopUtils.particleFlowSetup_cff")

## adapt PAT to it
from PhysicsTools.PatAlgos.tools.pfTools import adaptPFMuons
adaptPFMuons(process, process.patMuons, '')
process.patMuons.pfMuonSource = "pfSelectedMuons"

## create a good vertex collection
pvSelection = cms.PSet(
  minNdof = cms.double( 7.)
, maxZ    = cms.double(24.)
, maxRho  = cms.double( 2.)
)

process.goodOfflinePrimaryVertices = cms.EDFilter(
      "PrimaryVertexObjectFilter" # checks for fake PVs automatically
    , filterParams = pvSelection
    , filter       = cms.bool(False) # use only as producer
    , src          = cms.InputTag('offlinePrimaryVertices')
    )

process.goodOfflinePrimaryVerticesWithBS = cms.EDFilter(
      "PrimaryVertexObjectFilter" # checks for fake PVs automatically
    , filterParams = pvSelection
    , filter       = cms.bool(False) # use only as producer
    , src          = cms.InputTag('offlinePrimaryVerticesWithBS')
    )

## use only good vertices for pfNoPileUp and muons
process.pfPileUp.Vertices          = cms.InputTag('goodOfflinePrimaryVertices')
process.pfMuonsFromVertex.vertices = cms.InputTag('goodOfflinePrimaryVertices')

## adapt isolation cone for muons and electrons
process.isoValMuonWithNeutral.deposits[0].deltaR = cms.double(0.3)
process.isoValMuonWithCharged.deposits[0].deltaR = cms.double(0.3)
process.isoValMuonWithPhotons.deposits[0].deltaR = cms.double(0.3)

## use beamspot instead of primary vertex for the muon creation
process.patMuons.usePV = False

process.patSequence = cms.Sequence(process.muonMatch *
                                   process.patMuons  *
                                   process.selectedPatMuons
                                   )

# reduce size
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.genParticles = cms.EDProducer(
    #see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideGenParticlePruner
    "GenParticlePruner",
    src = cms.InputTag("genParticles"),
    select = cms.vstring(
        "drop  *  ",
        "++keep++ pdgId = {Z0}",
        "++keep++ pdgId = {W+}",   "++keep++ pdgId = {W-}",
        "++keep++ pdgId = {t}",    "++keep++ pdgId = {tbar}",
        "++keep++ pdgId = {b}",    "++keep++ pdgId = {bbar}",
        "++keep++ pdgId = {c}",    "++keep++ pdgId = {cbar}",
        # "drop pdgId = {Z0} & status = 2"
        "++keep++ pdgId = {e+}",   "++keep++ pdgId = {e-}",
        "++keep++ pdgId = {mu+}",  "++keep++ pdgId = {mu-}",
        "++keep++ pdgId = {tau+}", "++keep++ pdgId = {tau-}",
    )
)

## needed for redoing the ak5GenJets
process.load("TopAnalysis.TopUtils.GenJetParticles_cff")
process.load("RecoJets.Configuration.RecoGenJets_cff")

## make sure the genJets are clustered from the full genParticle collection and not the pruned one
process.genParticlesForJets.src = cms.InputTag("genParticles","","HLT")

## std sequence to produce the ttGenEvt
process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")

## let it run
process.p = cms.Path(
    process.makeGenEvt      *
    process.goodOfflinePrimaryVertices *
    process.goodOfflinePrimaryVerticesWithBS *
    process.particleFlow    *
    process.patSequence     *
    process.genParticles    *
    process.genJetParticles *
    process.ak5GenJets
)

process.out.outputCommands+= ['keep recoGenParticles_genParticles_*_PAT']
#process.out.outputCommands+= ['keep *_genParticles_*_*']
process.out.outputCommands+= ['keep *DcsStatus*_*_*_*']
process.out.outputCommands+= ['keep recoTracks_generalTracks_*_*']
process.out.outputCommands+= ['keep *_*Gsf*_*_*']
process.out.outputCommands+= ['keep *_*gsf*_*_*']
process.out.outputCommands+= ['keep *SuperCluster*_*_*_*']
process.out.outputCommands+= ['keep *_offlineBeamSpot_*_*']
process.out.outputCommands+= ['keep *_offlinePrimaryVertices*_*_*']
process.out.outputCommands+= ['keep *_goodOfflinePrimaryVertices*_*_*']
process.out.outputCommands+= ['keep *_pfPileUp_*_*']
process.out.outputCommands+= ['keep *_pfNoPileUp_*_*']
#process.out.outputCommands+= ['keep *_particleFlow__*']
process.out.outputCommands+= ["keep *_genMetTrue_*_*"]
process.out.outputCommands+= ['keep recoGenJets_ak5GenJets_*_PAT']
process.out.outputCommands+= ['keep *_genEvt_*_*']
process.out.outputCommands+= ['keep *_initSubset_*_*']
process.out.outputCommands+= ['keep *_decaySubset_*_*']
process.out.outputCommands+= ['keep *_addPileupInfo_*_*']
process.out.outputCommands+= ['keep edmTriggerResults_*_*_*']
process.out.outputCommands+= ['keep *_hltTriggerSummaryAOD_*_*']
process.out.outputCommands+= ['keep GenEventInfoProduct_*_*_*']
process.out.outputCommands+= ['keep *_selectedPatMuons_*_*']
