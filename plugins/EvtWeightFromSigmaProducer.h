#ifndef EvtWeightFromSigmaProducer_h
#define EvtWeightFromSigmaProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class EvtWeightFromSigmaProducer : public edm::EDProducer {

 public:
  explicit EvtWeightFromSigmaProducer(const edm::ParameterSet&);
  ~EvtWeightFromSigmaProducer();
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  double eventWeight_;
  double crossSection_;
  int nEvents_;
  double targetLumi_;  
  double prodLumi;
};

#endif
