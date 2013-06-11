#ifndef UhhGenJetSelector_h
#define UhhGenJetSelector_h

#include <memory>
#include <string>
#include <iostream>
#include <utility>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
//#include "DataFormats/Candidate/interface/Candidate.h"

/**
   \class   UhhGenJetSelector UhhGenJetSelector.h "TopAnalysis/TopUtils/interface/UhhGenJetSelector.h"

   \brief   Module to select bjets from a genJet collection

   This is a module to select the bjets from a genjet collection. This is done by the indices _bHadJetIdx_
   and _antibHadJetIdx_ which have been identified beforehand. Furthermore, also pt and eta cuts can be applied
   
   _genJet_         : input gen jet collection
   
   _bHadJetIdx_     : index of identified b jet
   
   _antibHadJetIdx_ : index of identified anti b jet
   
   _pt_           : minimum pt requirement the relevant objects have to fulfill
   
   _eta_          : maximum eta requirement the relevant objects have to fulfill
   
*/

class UhhGenJetSelector : public edm::EDProducer {

 public:
  /// default constructor
  explicit UhhGenJetSelector(const edm::ParameterSet&);
  /// default destructor
  ~UhhGenJetSelector(){};
  
 private:
  /// produce function to select
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  /// input collection
  edm::InputTag genJet_;
  // from gen level b-jet identification  
  edm::InputTag bHadJetIdx_, antibHadJetIdx_;
  /// min pt requirement
  double pt_;
  /// max eta requirement
  double eta_;
  /// b jet indices
  int bIX;
  int bbarIX;
  
}; 

#endif
