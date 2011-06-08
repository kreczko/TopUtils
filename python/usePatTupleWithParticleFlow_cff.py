import FWCore.ParameterSet.Config as cms

def prependPF2PATSequence(process, pathnames, postfix = ''):

    ## Standard Pat Configuration File
    process.load("PhysicsTools.PatAlgos.patSequences_cff")

    ## this is just an example selection for muons
    process.trackMuons = process.selectedPatMuons.clone(
        src = 'selectedPatMuons',
        cut = 'isGlobalMuon & isTrackerMuon &'
        'pt > 20. &'
        'abs(eta) < 2.1 &'
        'innerTrack.numberOfValidHits >= 11 &'
        'globalTrack.normalizedChi2 < 10.0  &'
        'globalTrack.hitPattern.numberOfValidMuonHits>0 &'
        'abs(dB)<0.02 &'
        'innerTrack.hitPattern.pixelLayersWithMeasurement>=1 &'
        'numberOfMatches>1'
        )

    ## load configs for pf jet clustering and MET
    process.load("CommonTools.ParticleFlow.pfMET_cfi")
    process.load("CommonTools.ParticleFlow.pfJets_cff")

    ## cutomized top projectors for pat muons and electrons
    process.load("TopAnalysis.TopUtils.patMuonTopProjector_cfi")
    process.load("TopAnalysis.TopUtils.patElectronTopProjector_cfi")

    ## this needs to be re-run to compute rho for the L1Fastjet corrections
    process.load("RecoJets.JetProducers.kt4PFJets_cfi")
    process.kt6PFJets = process.kt4PFJets.clone(src='pfNoTrackMuons', doAreaFastjet=True, doRhoFastjet=True, rParam=0.6)

    ## cutomize the jet clustering (the default for pfJets is AK5 already,
    ## the input should be the new top-projected pfNoPileUp collection)
    process.pfNoTrackMuons = process.pfNoPatMuons.clone(topCollection = "trackMuons")
    process.pfJets.src = 'pfNoTrackMuons'


    ## customize the pat jets
    from PhysicsTools.PatAlgos.tools.jetTools import switchJetCollection
    switchJetCollection(
        process,cms.InputTag('pfJets'),
        doJTA        = True,
        doBTagging   = True,
        jetCorrLabel = ('AK5PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute'])),
        doType1MET   = False,
        genJetCollection=cms.InputTag("ak5GenJets"),
        doJetID      = False,
        outputModule = ''
        )

    ## no calo towers for pfJets
    process.patJets.embedCaloTowers     = False
    ## not needed, we keep all pfCandidates in the tuple
    process.patJets.embedPFCandidates   = False

    ## remove soft lepton taggers, which would have needed more RECO collections as input
    process.patDefaultSequence.remove(process.softMuonBJetTagsAOD)
    process.patDefaultSequence.remove(process.softMuonByPtBJetTagsAOD)
    process.patDefaultSequence.remove(process.softMuonByIP3dBJetTagsAOD)
    process.patDefaultSequence.remove(process.softMuonTagInfosAOD)
    
    process.patJets.tagInfoSources = []
    process.patJets.discriminatorSources.remove(cms.InputTag("softMuonBJetTagsAOD"))
    process.patJets.discriminatorSources.remove(cms.InputTag("softMuonByPtBJetTagsAOD"))
    process.patJets.discriminatorSources.remove(cms.InputTag("softMuonByIP3dBJetTagsAOD"))
    
    ## remove all but jets from the sequence
    from PhysicsTools.PatAlgos.tools.coreTools import removeAllPATObjectsBut, removeCleaning
    removeAllPATObjectsBut(process, ['Jets', 'METs'], False)
    removeCleaning(process, False)
    process.patDefaultSequence.remove(process.countPatLeptons)
    process.patDefaultSequence.remove(process.countPatJets)
    
    ## re-configure and create MET
    process.pfMET.src = 'pfNoPileUp'
    process.patMETs.metSource = "pfMET"
    process.patMETs.addMuonCorrections = False

    process.patDefaultSequence.remove(process.metJESCorAK5CaloJet)
    process.patDefaultSequence.remove(process.metJESCorAK5CaloJetMuons)
    
    if not postfix == '':
        from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
        cloneProcessingSnippet(process, process.patDefaultSequence, postfix)
    
        for module in process.patDefaultSequence.moduleNames():
            process.patDefaultSequence.remove(getattr(process,module))
            delattr(process,module)
    
        delattr(process,'patDefaultSequence')
        
    ## let it run
    process.pf2pat = cms.Sequence(
        # this one is only for testing
        process.trackMuons *
        process.pfNoTrackMuons *
        # jet clustering
        process.pfJetSequence *
        process.kt6PFJets *
        process.pfMET *
        getattr(process, 'patDefaultSequence' + postfix)
        )

    ## append pf2pat sequence to all paths in pathnames
    for pathname in pathnames:
        getattr(process, pathname).insert(0,process.pf2pat)
