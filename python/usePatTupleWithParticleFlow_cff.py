import FWCore.ParameterSet.Config as cms

def prependPF2PATSequence(process, pathnames = [''], options = dict()):

    ## output all options and set defaults
    print '==================================================='
    print '|||||||||||||||||||||||||||||||||||||||||||||||||||'
    print '==================================================='
    print 'options used by prependPF2PATSequence:'
    #print 'isMC:', options.setdefault('isMC', True)
    #print 'postfix:', options.setdefault('postfix', '')
    print 'runOnOLDcfg:', options.setdefault('runOnOLDcfg', False)
    print 'cutsMuon:', options.setdefault('cutsMuon', 'isGlobalMuon & abs(eta) < 2.5 & pt > 10.')
    print 'cutsElec:', options.setdefault('cutsElec', 'et > 20 & abs(eta) < 2.5')
    print 'pfIsoConeMuon:', options.setdefault('pfIsoConeMuon', 0.3)
    print 'pfIsoConeElec:', options.setdefault('pfIsoConeElec', 0.3)
    print 'pfIsoValMuon:', options.setdefault('pfIsoValMuon', 0.2)
    print 'pfIsoValElec:', options.setdefault('pfIsoValElec', 1.0)
    print '==================================================='
    print '|||||||||||||||||||||||||||||||||||||||||||||||||||'
    print '==================================================='

    ## postfixes are NOT supported right now
    postfix='' #options['postfix']

    ## Standard Pat Configuration File
    process.load("PhysicsTools.PatAlgos.patSequences_cff")

    ## this needs to be re-run to compute rho for the L1Fastjet corrections
    process.load("RecoJets.JetProducers.kt4PFJets_cfi")
    process.kt6PFJets = process.kt4PFJets.clone(src='pfNoElectron', doAreaFastjet=True, doRhoFastjet=True, rParam=0.6)

    ## only added as the usePF2PAT function needs it to work, is deleted immediately afterwards
    process.out = cms.OutputModule("PoolOutputModule",
                                   fileName = cms.untracked.string('dummyFile.root'),
                                   outputCommands = cms.untracked.vstring('drop *')
                                   )

    ## run the full PF2PAT sequence
    from PhysicsTools.PatAlgos.tools.pfTools import usePF2PAT
    usePF2PAT( process
             , runPF2PAT      = True
             , runOnMC        = True
             , jetAlgo        = 'AK5'
             , postfix        = postfix
             , jetCorrections = ( 'AK5PFchs'
                                , cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute'])
                                )
             )
   
    delattr(process,'out')

    ##
    ## customize PAT
    ##

    from PhysicsTools.PatAlgos.tools.coreTools import removeSpecificPATObjects, removeCleaning
    removeSpecificPATObjects(process, ['Muons', 'Taus', 'Photons'], False, postfix)
    removeCleaning(process, False, postfix)

    ## in case of postfix the pfPileUp and pfNoPileUp collections may not get one
    from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag
    massSearchReplaceAnyInputTag(getattr( process, 'patPF2PATSequence' + postfix), 'pfPileUp'   + postfix, 'pfPileUp')
    massSearchReplaceAnyInputTag(getattr( process, 'patPF2PATSequence' + postfix), 'pfNoPileUp' + postfix, 'pfNoPileUp')

    ## make selection for the pf candidates
    from TopAnalysis.TopUtils.particleFlowSetup_cff import pfSelectedChargedHadrons, pfSelectedNeutralHadrons, pfSelectedPhotons

    setattr(process, 'pfSelectedChargedHadrons'+postfix, pfSelectedChargedHadrons.clone())
    setattr(process, 'pfSelectedNeutralHadrons'+postfix, pfSelectedNeutralHadrons.clone())
    setattr(process, 'pfSelectedPhotons'       +postfix, pfSelectedPhotons.clone()       )

    getattr(process, 'patPF2PATSequence' + postfix).replace(getattr(process,'pfAllPhotons'+postfix),
                                                            getattr(process,'pfAllPhotons'+postfix)*
                                                            getattr(process,'pfSelectedChargedHadrons'+postfix)*
                                                            getattr(process,'pfSelectedNeutralHadrons'+postfix)*
                                                            getattr(process,'pfSelectedPhotons'+postfix)
                                                            )

    ##
    ## customize muons
    ##

    ## NOT WORKING SO FAR UNFORTUNATELY
    ### cutomized top projectors for pat muons and electrons
    #process.load("TopAnalysis.TopUtils.patMuonTopProjector_cfi")
    #from TopAnalysis.TopUtils.patMuonTopProjector_cfi import pfNoPatMuons
    #process.looseMuonsPF = process.selectedPatMuons.clone(
    #    src = 'selectedPatMuons',
    #    cut = 'isGlobalMuon &'
    #          'abs(eta) < 2.5 & pt > 10. &'
    #          '(chargedHadronIso+neutralHadronIso+photonIso)/pt <  0.2'
    #    )
    #
    #getattr( process, 'patPF2PATSequence' + postfix).replace(getattr(process, 'pfNoMuon'+postfix), process.looseMuonsPF*getattr(process, 'pfNoMuon'+postfix))
    #setattr(process,'pfNoMuon'+postfix, pfNoPatMuons.clone(topCollection = "looseMuonsPF", name = 'noMuon')) #, verbose = True))

    ## keep pf muon sequence for top preojections, although as it was already done at creation
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfAllMuons'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfMuonsFromVertex'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfSelectedMuons'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'isoDepMuonWithCharged'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'isoDepMuonWithNeutral'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'isoDepMuonWithPhotons'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'isoValMuonWithCharged'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'isoValMuonWithNeutral'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'isoValMuonWithPhotons'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfIsolatedMuons'+postfix))

    ## apply selection for muons ONLY VAILD FOR THE TOP PROJECTIONS
    getattr(process, 'pfSelectedMuons'+postfix).cut = options['cutsMuon']

    getattr(process, 'isoDepMuonWithCharged'+postfix).src = 'pfSelectedMuons'+postfix
    getattr(process, 'isoDepMuonWithNeutral'+postfix).src = 'pfSelectedMuons'+postfix
    getattr(process, 'isoDepMuonWithPhotons'+postfix).src = 'pfSelectedMuons'+postfix

    ## adapt isolation cone for muons
    getattr(process,"isoValMuonWithNeutral"+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,"isoValMuonWithCharged"+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    getattr(process,"isoValMuonWithPhotons"+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeMuon'])
    
    ## adapt isolation cut
    process.pfIsolatedMuons.combinedIsolationCut = options['pfIsoValMuon']

    ##
    ## customize electrons
    ##

    ## add CiC electron ID
    process.load('RecoEgamma.ElectronIdentification.cutsInCategoriesElectronIdentificationV06_cfi')
    process.eidCiCSequence = cms.Sequence(
        process.eidVeryLooseMC  *
        process.eidLooseMC      *
        process.eidMediumMC     *
        process.eidTightMC      *
        process.eidSuperTightMC *
        process.eidHyperTight1MC
        )

    getattr(process,'patElectrons'+postfix).electronIDSources = cms.PSet(
        eidVeryLooseMC   = cms.InputTag("eidVeryLooseMC"),
        eidLooseMC       = cms.InputTag("eidLooseMC"),
        eidMediumMC      = cms.InputTag("eidMediumMC"),
        eidTightMC       = cms.InputTag("eidTightMC"),
        eidSuperTightMC  = cms.InputTag("eidSuperTightMC"),
        eidHyperTight1MC = cms.InputTag("eidHyperTight1MC")
        )

    getattr(process,'pfAllElectrons'+postfix).src = 'pfNoMuon'+postfix

    ## apply selection for electrons (they are the source of the pat::electrons)
    getattr(process,'pfSelectedElectrons'+postfix).cut = options['cutsElec']

    getattr(process, 'isoDepElectronWithCharged'+postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process, 'isoDepElectronWithNeutral'+postfix).src = 'pfSelectedElectrons'+postfix
    getattr(process, 'isoDepElectronWithPhotons'+postfix).src = 'pfSelectedElectrons'+postfix

    getattr(process,"isoDepElectronWithCharged"+postfix).ExtractorPSet.inputCandView = 'pfSelectedChargedHadrons'
    getattr(process,"isoDepElectronWithNeutral"+postfix).ExtractorPSet.inputCandView = 'pfSelectedNeutralHadrons'
    getattr(process,"isoDepElectronWithPhotons"+postfix).ExtractorPSet.inputCandView = 'pfSelectedPhotons'

    ## adapt isolation cone for electrons
    getattr(process,"isoValElectronWithNeutral"+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,"isoValElectronWithCharged"+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])
    getattr(process,"isoValElectronWithPhotons"+postfix).deposits[0].deltaR = cms.double(options['pfIsoConeElec'])

    getattr(process,'pfIsolatedElectrons'+postfix).combinedIsolationCut = options['pfIsoValElec']

    ##
    ## customize taus
    ##

    ## remove the full pftau sequence as it is not needed for us
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTauPFJets08Region'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTauPileUpVertices'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTauTagInfoProducer'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfJetsPiZeros'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfJetsLegacyTaNCPiZeros'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfJetsLegacyHPSPiZeros'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTausBase'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTausBaseDiscriminationByLeadingTrackFinding'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTausBaseDiscriminationByIsolation'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTausBaseDiscriminationByLeadingPionPtCut'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfTaus'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfNoTau'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'pfMET'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'electronMatch'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'patElectrons'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'muonMatch'+postfix))
    #getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'patMuons'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByLeadingTrackFinding'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByLeadingTrackPtCut'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByLeadingPionPtCut'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByIsolation'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTrackIsolation'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByECALIsolation'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByIsolationUsingLeadingPion'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTrackIsolationUsingLeadingPion'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByECALIsolationUsingLeadingPion'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationAgainstElectron'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationAgainstMuon'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNC'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrOnePercent'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrHalfPercent'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrQuarterPercent'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'shrinkingConePFTauDiscriminationByTaNCfrTenthPercent'+postfix))

    ##
    ## customize photons
    ##

    ## remove the photons match
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'photonMatch'+postfix))

    ##
    ## customize jets
    ##

    ## switchmodules to correct sources
    massSearchReplaceAnyInputTag(getattr( process, 'patPF2PATSequence' + postfix), 'pfNoTau' + postfix, 'pfJets' + postfix)

    ## remove soft lepton taggers, which would have needed more RECO collections as input
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'softMuonTagInfosAOD'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'softMuonBJetTagsAOD'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'softMuonByPtBJetTagsAOD'+postfix))
    getattr( process, 'patPF2PATSequence' + postfix).remove(getattr(process,'softMuonByIP3dBJetTagsAOD'+postfix))
    
    getattr(process,'patJets'+postfix).tagInfoSources = []
    getattr(process,'patJets'+postfix).discriminatorSources.remove(cms.InputTag("softMuonBJetTagsAOD"+postfix))
    getattr(process,'patJets'+postfix).discriminatorSources.remove(cms.InputTag("softMuonByPtBJetTagsAOD"+postfix))
    getattr(process,'patJets'+postfix).discriminatorSources.remove(cms.InputTag("softMuonByIP3dBJetTagsAOD"+postfix))
    
    ##
    ## customize MET
    ##

    ## re-configure and create MET
    getattr(process,'pfMET'+postfix).src = 'pfNoPileUp'
    #getattr(process,'patMETs'+postfix).metSource = "pfMET"
    #getattr(process,'patMETs'+postfix).addMuonCorrections = False

    ## let it run
    process.pf2pat = cms.Sequence(
        process.eidCiCSequence *
        getattr( process, 'patPF2PATSequence' + postfix)
        )

    process.pf2pat.replace( getattr(process,'patJetCorrFactors'+postfix)
                          , process.kt6PFJets * getattr(process,'patJetCorrFactors'+postfix)
                          )

    process.pf2pat.remove(getattr(process,'pfPileUp'+postfix))
    process.pf2pat.remove(getattr(process,'pfNoPileUp'+postfix))

    ## append pf2pat sequence to all paths in pathnames
    if pathnames == ['']:
        pathnames = process.paths_().keys()

    print 'prepending PF2PAT sequence to paths:', pathnames
    for pathname in pathnames:
        if options['runOnOLDcfg']:
            massSearchReplaceAnyInputTag(getattr(process,pathname), 'selectedPatJetsAK5PF', 'selectedPatJets')
            massSearchReplaceAnyInputTag(getattr(process,pathname), 'patMETsPF', 'patMETs')
            massSearchReplaceAnyInputTag(getattr(process,pathname), cms.InputTag('scaledJetEnergy', 'selectedPatJetsAK5PF', process.name_()), cms.InputTag('scaledJetEnergy', 'selectedPatJets', process.name_()))
        getattr(process, pathname).insert(0,process.pf2pat)
