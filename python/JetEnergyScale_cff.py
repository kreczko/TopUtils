import FWCore.ParameterSet.Config as cms

from TopAnalysis.TopUtils.JetEnergyScale_cfi import *

def scalePatJetsEnergy(process,
                       factor):

    print "---------------------------------------------------------------------"
    print "Rescaling the energy of the patJets by a factor of " + str(factor)
    print "before using them to produce the selectedPatJets."
    print "If you are using MET in your analysis, please change your InputTag to"
    print "'scaledJetEnergy:patMETs' in your modules."

    process.scaledJetEnergy.scaleFactor = factor

    process.selectedPatCandidates.replace(process.selectedPatJets,
                                          process.scaledJetEnergy * process.selectedPatJets)

    process.selectedPatJets.src = "scaledJetEnergy:patJets"

    print "---------------------------------------------------------------------"
