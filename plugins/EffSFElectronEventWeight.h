#ifndef EffSFElectronEventWeight_h
#define EffSFElectronEventWeight_h

#include <memory>
#include <string>
#include <iostream>

#include "TH1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TString.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/// This module returns a scale factor (SF) for electron selection and trigger efficiency, including the jet leg of the cross trigger
/// which is put into the CMSSW event
/// as a double, which can be used as an event weight in the analyzers of interest.
/// cfg parameters:
/// particles_             particle collection
/// sysVar_                name of the systematic variation 
///                        "noSys"
///                        "combinedEffSFNormUp/Down": normalisation uncertainty using the statistical errors
///                        "combinedEffSFShapeUpPt(Eta)/DownPt(Eta)" : uncertainty to distort the shape (softens or tightens the dependence)
///                        "flatTriggerSF": uses meanTriggerEffSF_ as flat SF
///                        "selectionEffSFNormUp/Down": uses additionalFactorErr_ as flat global uncertainty
/// verbose_               set to 0 if no output on terminal is desired, 1 for moderate and 2 for detailed output
/// filenameJetLeg         if not set to "", efficiencies are loaded from histos in filename_
/// additionalFactor_      multiplies with factor (can be used for flat SF)
/// additionalFactorErr_   flat normalisation error for additional factor (e.g. 0.03 = 3%)
/// meanTriggerEffSF_      flat mean trigger eff.
/// meanTriggerEffSFErr_    error on flat mean trigger eff.
/// shapeDistortionErr_ for eff SF shape uncertainty
/// shapeVarPt/EtaThreshold_  pT/eta value which divides up and down variation for shape variations
/// jetTriggerEffsSFNormSysErr   syst. SF error for the jet leg

class EffSFElectronEventWeight : public edm::EDProducer {

 public:
  explicit EffSFElectronEventWeight(const edm::ParameterSet&);
  ~EffSFElectronEventWeight();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob();

 private:
  edm::InputTag electrons_;
  edm::InputTag jets_;
  std::string sysVar_;
  double shapeVarPtEleThreshold_;
  double shapeVarEtaEleThreshold_;
  int verbose_;
  edm::FileInPath filenameJetLeg_;
  double additionalFactor_;
  double additionalFactorErr_;
  double meanTriggerEffSF_;
  double meanTriggerEffSFErr_;
  double shapeDistortionErr_;
  double jetTriggerEffsSFNormSysErr_;
  double jetTriggerEffsSFShapeSysErr_;
  
  /// histogram container
  /// efficiency histos as input
  std::map<std::string, TGraph*> effHists_;
  /// hists as output for control
  std::map<std::string, TH1F*> hists_;
  
  double factorNonIso_;  // fracton of luminosity of Ele25TriJet range = lumiNonIso/lumiTotal
  double factorIso_ ; // fraction of luminosity of IsoEle25TriJet range = lumiIso/lumiTotal
  
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
