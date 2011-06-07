## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

## run the particle flow classification as far as appropriate
process.load("TopAnalysis.TopUtils.particleFlowSetup_cff")

## adapt PAT to it
from PhysicsTools.PatAlgos.tools.pfTools import adaptPFMuons, adaptPFElectrons
adaptPFMuons(process, process.patMuons, '')
adaptPFElectrons(process, process.patElectrons, '')
process.patMuons.pfMuonSource = "pfSelectedMuons"
process.patElectrons.pfElectronSource = "pfSelectedElectrons"

## remove all from the default sequence but muons and electrons
from PhysicsTools.PatAlgos.tools.coreTools import *
removeAllPATObjectsBut(process, ['Muons', 'Electrons'])
process.patDefaultSequence.remove(process.patJets)
process.patDefaultSequence.remove(process.patMETs)

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

## Needed for redoing the ak5GenJets
process.load("TopAnalysis.TopUtils.GenJetParticles_cff")
process.load("RecoJets.Configuration.RecoGenJets_cff")

## std sequence to produce the ttGenEvt
process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")

## let it run
process.p = cms.Path(
    process.makeGenEvt         *
    process.particleFlow       *
    process.patDefaultSequence *
    process.genParticles       *
    process.genJetParticles    *
    process.ak5GenJets
)

process.out.outputCommands+= ['keep recoGenParticles_genParticles_*_PAT']
#process.out.outputCommands+= ['keep *_genParticles_*_*']
process.out.outputCommands+= ['keep *DcsStatus*_*_*_*']
process.out.outputCommands+= ['keep recoTracks_generalTracks_*_*']
#process.out.outputCommands+= ['keep *_generalTracks__*']
process.out.outputCommands+= ["keep *_offlineBeamSpot_*_*"]
process.out.outputCommands+= ['keep *_offlinePrimaryVertices_*_*']
process.out.outputCommands+= ['keep *_pfPileUp_*_*']
process.out.outputCommands+= ['keep *_pfNoPileUp_*_*']
process.out.outputCommands+= ['keep *_pfAllMuons_*_*']
process.out.outputCommands+= ['keep *_pfSelectedMuons_*_*']
process.out.outputCommands+= ['keep *_pfAllElectrons_*_*']
process.out.outputCommands+= ['keep *_pfSelectedElectrons_*_*']
process.out.outputCommands+= ["keep *_genMetTrue_*_*"]
process.out.outputCommands+= ['keep recoGenJets_ak5GenJets_*_PAT']
process.out.outputCommands+= ['keep *_genEvt_*_*']
process.out.outputCommands+= ['keep *_addPileupInfo_*_*']
