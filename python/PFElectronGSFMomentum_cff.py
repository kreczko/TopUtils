import FWCore.ParameterSet.Config as cms

from TopAnalysis.TopUtils.PFElectronGSFMomentum_cfi import *

def pFElectronGSFMomentum(process,
                       factor):

    print "---------------------------------------------------------------------"
    print "Replacing the Momentum of the PF Electrons by the GSF values"
    print "If you are using MET in your analysis, please change your InputTag to"
    print "'pFEnergyGSFMomElectron:patMETs' in your modules."

    print "---------------------------------------------------------------------"
