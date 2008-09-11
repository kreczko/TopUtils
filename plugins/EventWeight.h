#ifndef EventWeight_h
#define EventWeight_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class EventWeight : public edm::EDProducer {

 public:
  explicit EventWeight(const edm::ParameterSet&);
  ~EventWeight(){};
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  double gridEff_;
  edm::InputTag weight_;
};

#endif
