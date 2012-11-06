#ifndef MyBTagProducer_h
#define MyBTagProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Candidate/interface/Candidate.h"
//#include "DataFormats/PatCandidates/interface/Jet.h"

class MyBTagProducer : public edm::EDProducer {

 public:
  explicit MyBTagProducer(const edm::ParameterSet&);
  ~MyBTagProducer();
  bool containsBHadron(const reco::Candidate* consti);
  
 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  edm::InputTag jetSrc_;
  float bDiscValue_;

};

#endif
