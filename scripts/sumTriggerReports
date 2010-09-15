#! /usr/bin/env python

# based on script by Salvatore Rappoccio
# sums summary tables of splitted jobs
# run eg with 
# ' triggerReport crab_0_xxxxxx_yyyyyy/res/CMSSW_*.stdout
 
import re
import sys
import os.path

p = re.compile(r'\W+')


# modules in TrigReport summary tables
tokens = ['eventWeight',
	  'analyzePrimaryVertex',
	  'filterTrigger',
	  'cleanPatMuons',
	  'cleanPatElectrons',
	  'cleanPatPhotons',
	  'cleanPatTaus',
	  'cleanPatJets',
	  'cleanPatCandidateSummary',
	  'tightMuons',
	  'tightMuonSelection',
	  'globalMuons',
	  'globalMuonSelection',
	  'hardMuons',
	  'hardMuonSelection',
	  'hardJets',
	  'analyzeHardMuons',
	  'analyzeHardMuonPair',
	  'analyzeHardJets1',
	  'analyzeMET1',
	  'goodTrackMuons',
	  'goodTrackMuonSelection',
	  'goodD0Muons',
	  'goodD0MuonSelection',
	  'goodFitMuons',
	  'goodFitMuonSelection',
	  'ecalMipMuons',
	  'ecalMipMuonSelection',
	  'hcalMipMuons',
	  'hcalMipMuonSelection',
	  'isolatedMuons',
	  'isolatedMuonSelection',
	  'analyzeIsolatedMuons',
	  'analyzeIsolatedMuonPair1',
	  'analyzeHardJets2',
	  'analyzeMET2',
	  'hardJetSelection',
	  'tightJets',
	  'analyzeTightJets1',
	  'tightJetSelection',
	  'analyzeIsolatedMuonPair2',
	  'analyzeMET3',
	  'highMETs',
	  'metSelection',
	  'analyzeTightJets2',
	  'kinSolutionTtFullLepEventHypothesis',
	  'ttFullLepHypKinSolution',
	  'ttFullLepEvent',
	  'filterDiMuonMassQCDveto',
	  'filterDiMuonMassZveto',
	  'filterHypoValidity',
	  'analyzeTightJets3',
	  'analyzeKinSolution1',
	  'filterBtag',
	  'analyzeTightJets4',
	  'analyzeKinSolution2'
         ]

print 'TrigReport ---------- Modules in Path: p ------------'
print 'TrigReport  Trig Bit#    Visited     Passed     Failed      Error Name'

for s in tokens:
#    print 'Looking at string ' + s

    nvisit = 0
    npass = 0
    nfail = 0
    nerror = 0
    for i in range(1,len(sys.argv) ) :
#        print 'Opening file ' + sys.argv[i]
        infile = open(sys.argv[i], 'r')
    
        for line in infile :
            if ( line.find(s) >= 0 ) :
#                print 'found! line = ',line
#                print p.split(line)
                nvisit += int(p.split(line)[3])
                npass  += int(p.split(line)[4])
                nfail  += int(p.split(line)[5])
                nerror += int(p.split(line)[6])
                break
        infile.close()
    print 'TrigReport     1    0  %9d' % nvisit,'%10d' % npass,'%10d' % nfail, '%10d' % nerror, s
