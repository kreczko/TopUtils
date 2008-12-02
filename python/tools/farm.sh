#!/bin/sh

## 
# to run farmscripts do the following:
#    * voms-proxy-init --voms cms -rfc
#    * gsissh cms.naf.desy.de 
#    * cd /scratch/current/cms/user/***/
#    * ini cmssw
#    * setup environment etc
#    * kinit -4 daisy@CERN.CH to co UserCode from cern Respository
#    * compile
#    * configure your cmsRun job
#    * qsub -l h_vmem=1G -cwd -V -o output.txt -e error.txt TopAnalysis/TopUtils/python/tools/farm.sh
##

echo "hallo welt"

python TopAnalysis/TopUtils/python/tools/farm.py

echo "hallo welt"
python TopAnalysis/TopUtils/python/tools/ConfigRunner.py -t 'top' -e 100 -a analysis

echo "und tschuesssssssss"

# os.system(cd)
## 1) change to data directory on worker node
##    * qsub -V -cwd
## 2) set cmsenv --> ask Benedikt Hegner with problem description / crab experts
## 3) cut job in pieces (configurable)
## 4) add untracked uint32 skipEvents = 0 thatfore
#( 5) monitor jobs qsub -status )
## 6  return job and add output:
##     -- edm none
##     -- histogram files hadd
##     -- mva mit TreeSaver --> combine root trees
