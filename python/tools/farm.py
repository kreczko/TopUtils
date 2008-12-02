#!/afs/naf.desy.de/group/cms/sw/slc4_ia32_gcc345/external/python/2.4.2-CMS3q/bin/python

import os
import sys
import time

print os.getcwd()

for i in range(10):
   print '.'
   time.sleep(1)


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
