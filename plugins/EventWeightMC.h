#ifndef EventWeightMC_h
#define EventWeightMC_h

#include <memory>
#include <string>
#include <iostream>

#include "TH1.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class EventWeightMC : public edm::EDProducer {

 public:
  explicit EventWeightMC(const edm::ParameterSet&);
  ~EventWeightMC(){};
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);
  
 private:
  double weight_ ;
  /// histogram container
  /// for output
  std::map<std::string, TH1F*> hists_;
};

#endif
