#ifndef EventWeightPlain_h
#define EventWeightPlain_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class EventWeightPlain : public edm::EDProducer {

 public:
  explicit EventWeightPlain(const edm::ParameterSet&);
  ~EventWeightPlain();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  double eff_;  
  double xsec_;
  double lumi_;  
  unsigned nevts_;

  double wght_;
};

#endif
