#ifndef EventWeightMultiplier_h
#define EventWeightMultiplier_h

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

/// This module multiplies different event weights from the CMSSW event and an optional additional factor,
/// which is put again into the CMSSW event
/// as a double, which can be used as an event weight in the analyzers of interest.
/// cfg parameters:
/// eventWeightTags_    a vector of event weight InputTags
/// additionalFactor_   additional factor to be multiplied
/// verbose_            set to 0 if no output on terminal is desired

class EventWeightMultiplier : public edm::EDProducer {

 public:
  explicit EventWeightMultiplier(const edm::ParameterSet&);
  ~EventWeightMultiplier();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob();

 private:
  std::vector<edm::InputTag> eventWeightTags_;
  double additionalFactor_;
  int verbose_;
  
  /// histogram container
  /// for output
  std::map<std::string, TH1F*> hists_;  
};

#endif
