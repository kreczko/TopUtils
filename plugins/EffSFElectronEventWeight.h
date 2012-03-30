#ifndef EffSFElectronEventWeight_h
#define EffSFElectronEventWeight_h

#include <memory>
#include <string>
#include <iostream>

#include "TH1.h"
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
///                        "triggerEffSFNormUp/Down": normalisation uncertainty using the statistical errors
///                        "triggerEffSFShapeUp/Down" : uncertainty to distort the shape (softens or tightens the dependence)
///                        "flatTriggerSF": uses meanTriggerEffSF_ as flat SF
///                        "selectionEffSFNormUp/Down": uses additionalFactorErr_ as flat global uncertainty
/// verbose_               set to 0 if no output on terminal is desired, 1 for moderate and 2 for detailed output
/// filename_              if not set to "", efficiencies are loaded from histos in filename_
/// additionalFactor_      multiplies with factor (can be used for flat SF)
/// additionalFactorErr_   flat normalisation error for additional factor (e.g. 0.03 = 3%)
/// meanTriggerEffSF_      flat mean trigger eff.
/// meanTriggerEffSFErr_    error on flat mean trigger eff.
/// shapeDistortionErr_ for eff SF shape uncertainty
/// shapeVarPt/EtaThreshold_  pT/eta value which divides up and down variation for shape variations

class EffSFElectronEventWeight : public edm::EDProducer {

 public:
  explicit EffSFElectronEventWeight(const edm::ParameterSet&);
  ~EffSFElectronEventWeight();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob();

 private:
  edm::InputTag particles_;
  std::string sysVar_;
  double shapeVarPtThreshold_;
  double shapeVarEtaThreshold_;
  int verbose_;
  //std::string filename_;
  double additionalFactor_;
  double additionalFactorErr_;
  double meanTriggerEffSF_;
  double meanTriggerEffSFErr_;
  double shapeDistortionErr_;
  
  /// histogram container
  /// efficiency histos as input
  std::map<std::string, TH1F*> effHists_;
  /// hists as output for control
  std::map<std::string, TH1F*> hists_;
  
  double factorNonIso_;  // fracton of luminosity of Ele25TriJet range = lumiNonIso/lumiTotal
  double factorIso_ ; // fraction of luminosity of IsoEle25TriJet range = lumiIso/lumiTotal
  
  /// file with histos
  TFile * file_;
  
};

#endif
