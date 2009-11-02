import FWCore.ParameterSet.Config as cms

from TopAnalysis.TopUtils.JetEnergyScale_cfi import *

def scaleAllLayer1JetsEnergy(process,
                             factor):

    print "---------------------------------------------------------------------"
    print "Rescaling the energy of the allLayer1Jets be a factor of " + str(factor)
    print "before using them to produce the selectedLayer1Objects."
    print "If you are using MET in your analysis, please change your InputTag to"
    print "'scaledJetEnergy:layer1METs' in your modules."

    process.scaledJetEnergy.scaleFactor = factor

    process.selectedLayer1Objects.replace(process.selectedLayer1Jets,
                                          process.scaledJetEnergy * process.selectedLayer1Jets)

    process.selectedLayer1Jets.src = "scaledJetEnergy:allLayer1Jets"

    print "---------------------------------------------------------------------"
