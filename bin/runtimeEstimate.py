import os
import sys
import time

t0 = os.times()
# uncomment for cmsRun
os.system('cmsRun TopAnalysis/TopAnalyzer/test/analyzeMuon.cfg')
# uncomment for FWLite
# os.system('FWMuonAnalyzer TopAnalysis/TopAnalyzer/bin/FWMuonAnalyzer.cfg')
t1 = os.times()
cpu = t1[3]+t1[4] - (t0[3]+t0[4])
print 'CPU time measured in Python:', cpu 
