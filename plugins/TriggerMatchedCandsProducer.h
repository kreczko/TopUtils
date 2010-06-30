#ifndef TriggerMatchedCandProducer_h
#define TriggerMatchedCandProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class TriggerMatchedCandsProducer : public edm::EDProducer {

 public:
  explicit TriggerMatchedCandsProducer(const edm::ParameterSet&);
  ~TriggerMatchedCandsProducer(){};
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  edm::InputTag src_;
  std::string matches_;
  edm::InputTag trigger_;
  edm::InputTag triggerEvent_;
};

#endif
