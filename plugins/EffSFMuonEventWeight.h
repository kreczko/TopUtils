#ifndef EffSFMuonEventWeight_h
#define EffSFMuonEventWeight_h

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
/// particles_        particle collection
/// sysVar_           name of the systematic variation (noSys, EffSFUp, EffSFDown)
/// verbose_          set to 0 if no output on terminal is desired, 1 for moderate and 2 for detailed output
/// filename_         if not set to "", efficiencies are loaded from histos in filename_

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
  std::string filename_;
  double additionalFactor_;
  
  /// histogram container
  /// efficiency histos as input
  std::map<std::string, TH1F*> effHists_;
  
  /// file with histos
  TFile * file_;
  
};

#endif
