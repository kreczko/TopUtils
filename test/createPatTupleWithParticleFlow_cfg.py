import FWCore.ParameterSet.VarParsing as VarParsing
import sys

# setup 'standard' options
options = VarParsing.VarParsing ('standard')
## decide whether to run on MC or data
options.register('runOnMC', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "decide to run on MC or data")
## is the MC ttbar (has no effect once runOnMC==False)
options.register('isTTBarMC', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.bool, "decide to produce ttGenEvent")
## global Tag to be used (if left empty pat default tag is used)
options.register('globalTag', '', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "which globalTag should be used")

# get and parse the command line arguments
if( hasattr(sys, "argv") ):
    for args in sys.argv :
        arg = args.split(',')
        for val in arg:
            val = val.split('=')
            if(len(val)==2):
                setattr(options,val[0], val[1])

## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

if not options.globalTag == '':
    process.GlobalTag.globaltag = options.globalTag + '::All'

### test configuration
#process.source.fileNames = cms.untracked.vstring()
#process.source.fileNames += 'file:/scratch/hh/current/cms/user/eschliec/TTBar_pythia6_Summer11/7ABCBB94-E67D-E011-9B16-00151796D80C.root',
#process.source.fileNames += 'file:/scratch/hh/current/cms/user/eschliec/TTBar_pythia6_Summer11/4A5D814A-E67D-E011-9052-0015178C0190.root',
#process.maxEvents.input = cms.untracked.int32(-1)

## run the particle flow classification as far as appropriate
process.load("TopAnalysis.TopUtils.particleFlowSetup_cff")

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

## use only good vertices for pfNoPileUp
process.pfPileUp.Vertices          = cms.InputTag('goodOfflinePrimaryVertices')

if options.runOnMC:

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
    
    if options.isTTBarMC:
        ## std sequence to produce the ttGenEvt
        process.load("TopQuarkAnalysis.TopEventProducers.sequences.ttGenEvent_cff")

        ## make sure the ttGenEvent is made from the full genParticle collection and not the pruned one
        process.initSubset.src  = cms.InputTag("genParticles","","HLT")
        process.decaySubset.src = cms.InputTag("genParticles","","HLT")

## let it run
process.p = cms.Path(
    process.goodOfflinePrimaryVertices *
    process.goodOfflinePrimaryVerticesWithBS *
    process.pfPileUp *
    process.pfNoPileUp
    )

if options.runOnMC:
    if options.isTTBarMC:
        process.p += process.makeGenEvt

    process.p += process.genParticles
    process.p += process.genJetParticles
    process.p += process.ak5GenJets


process.out.outputCommands+= ['keep *DcsStatus*_*_*_*']
## all tracks in the event
process.out.outputCommands+= ['keep recoTracks_generalTracks_*_*']
## beamspot und primary vertices
process.out.outputCommands+= ['keep *_offlineBeamSpot_*_*']
process.out.outputCommands+= ['keep *_offlinePrimaryVertices*_*_*']
process.out.outputCommands+= ['keep *_goodOfflinePrimaryVertices*_*_*']
## collections for PF2PAT
process.out.outputCommands+= ['keep *_pfPileUp_*_*']
process.out.outputCommands+= ['keep *_pfNoPileUp_*_*']
## trigger information
process.out.outputCommands+= ['keep edmTriggerResults_*_*_*']
process.out.outputCommands+= ['keep *_hltTriggerSummaryAOD_*_*']
## data needed for muons
process.out.outputCommands+= ['keep *_muons__*']
process.out.outputCommands+= ['keep *_*_muCorrData_*']
process.out.outputCommands+= ['keep *_tevMuons_*_*']
process.out.outputCommands+= ['keep *_globalMuons__*']
process.out.outputCommands+= ['keep *_standAloneMuons_*_*']
## data needed for electrons
process.out.outputCommands+= ['keep *_*Gsf*_*_*']
process.out.outputCommands+= ['keep *_*gsf*_*_*']
process.out.outputCommands+= ['keep *SuperCluster*_*_*_*']
## generator information
if options.runOnMC:
    ## skimmed genParticles
    process.out.outputCommands+= ['keep recoGenParticles_genParticles_*_PAT']
    ## ak5GenJets
    process.out.outputCommands+= ['keep recoGenJets_ak5GenJets_*_PAT']
    ## genMET
    process.out.outputCommands+= ["keep *_genMetTrue_*_*"]
    ## pile-up information
    process.out.outputCommands+= ['keep *_addPileupInfo_*_*']
    ## further info on generated event (like pt-hat)
    process.out.outputCommands+= ['keep GenEventInfoProduct_*_*_*']
    ## ttGenEvent
    if options.isTTBarMC:
        process.out.outputCommands+= ['keep *_genEvt_*_*']
        process.out.outputCommands+= ['keep *_initSubset_*_*']
        process.out.outputCommands+= ['keep *_decaySubset_*_*']
