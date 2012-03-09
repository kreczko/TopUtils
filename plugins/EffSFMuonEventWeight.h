#ifndef EffSFMuonEventWeight_h
#define EffSFMuonEventWeight_h

#include <memory>
#include <string>
#include <iostream>

#include "TH1.h"
#include "TGraph.h"
#include "TFile.h"
#include "TString.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/// This module returns an efficiency scale factor (SF)
/// which is put into the CMSSW event
/// as a double, which can be used as an event weight in the analyzers of interest.
/// cfg parameters:
/// particles_             particle collection
/// sysVar_                name of the systematic variation 
///                        "noSys"
///                        "combinedEffSFNormUpStat/Down": normalisation uncertainty using the statistical errors
///                        "combinedEffSFShapeUpEta/Down" : uncertainty to distort the shape (softens or tightens the dependence)
///                        "flatTriggerSF": uses meanTriggerEffSF_ as flat SF
///                        "combinedEffSFNormUpSys/Down": uses additionalFactorErr_ as flat global uncertainty
/// verbose_               set to 0 if no output on terminal is desired, 1 for moderate and 2 for detailed output
/// filename_              if not set to "", efficiencies are loaded from histos in filename_
/// additionalFactor_      multiplies with factor (can be used for flat SF)
/// additionalFactorErr_   flat normalisation error for additional factor (e.g. 0.03 = 3%)
/// meanTriggerEffSF_      flat mean trigger eff.
/// shapeDistortionFactor_ for eff SF shape uncertainty
/// shapeVarPtThreshold_   eta value which divides up and down variation for shape variations
/// shapeVarPtThreshold_   pT value which divides up and down variation for shape variations

class EffSFMuonEventWeight : public edm::EDProducer {

 public:
  explicit EffSFMuonEventWeight(const edm::ParameterSet&);
  ~EffSFMuonEventWeight();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob();

 private:
  edm::InputTag particles_;
  std::string sysVar_;
  int verbose_;
  edm::FileInPath filename_;
  double additionalFactor_;
  double additionalFactorErr_;
  double meanTriggerEffSF_;
  double shapeDistortionFactor_;
  double shapeVarEtaThreshold_;
  double shapeVarPtThreshold_;
  
  /// histogram container
  /// efficiency histos as input
  std::map<std::string, TGraph*> effHists_;
  /// hists as output for control
  std::map<std::string, TH1F*> hists_;
  
  /// file with histos
  TFile * file_;
  
  /// values of graph
  int N          ;
  double *xVec   ;
  double *yVec   ;
  double *xErrHi ;
  double *xErrLo ;
  double *yErrHi ;
  double *yErrLo ;
  
};

#endif
