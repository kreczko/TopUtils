#ifndef TriggerMatchedCandProducer_h
#define TriggerMatchedCandProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/**
   \class   TriggerMatchedCandsProducer TriggerMatchedCandsProducer.h "TopAnalysis/TopUtils/plugins/TriggerMatchedCandsProducer.h"

   \brief   Module to produce a collection of muons that have a (pre-configured) match to a corresponding trigger object

   This module produces a collection of muons that have a pre-configured match to a corresponding 
   trigger object. This module only works with PAT objects. The parameters of the module are:

    _src_          : input collection of the reco objects, that will be checked to have a match to 
                     the corresponding trigger object (edm::InputTag).

    _matches_      : name of the PAT trigger match association used for the configuration of the 
                     pat::TriggerEvent (std::string).

    _triggerEvent_ : module label of the pat::TriggerEvent (edm::InputTag).

   This module is specific for pat::Muons. It can easily be generalized to any pat::Candidate by
   templating.
*/

class TriggerMatchedCandsProducer : public edm::EDProducer {

 public:
  /// default constructor
  explicit TriggerMatchedCandsProducer(const edm::ParameterSet& cfg);
  /// default destructor
  ~TriggerMatchedCandsProducer(){};
  
 private:
  /// produce the trigger matched muon collection
  virtual void produce(edm::Event& evt, const edm::EventSetup& setup);

 private:
  /// reco input collection
  edm::InputTag src_;
  /// label of muon matches
  std::string matches_;
  /// label of the used trigger event
  edm::InputTag triggerEvent_;
};

#endif
