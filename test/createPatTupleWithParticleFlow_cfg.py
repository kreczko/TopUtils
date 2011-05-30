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

## let it run
process.p = cms.Path(
    process.particleFlow *
    process.patDefaultSequence
)

process.out.outputCommands+= ['keep *_genParticles_*_*']
process.out.outputCommands+= ['keep recoTracks_generalTracks__*']
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
process.out.outputCommands+= ['keep recoGenJets_ak5GenJets_*_*']
