#ifndef TopMuonSelector_h
#define TopMuonSelector_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/*
   \class   MuonJetSelector MuonJetSelector.h "TopAnalysis/TopUtils/interface/MuonJetSelector.h"

   \brief   plugin to select for the muon, which is part of the top hypothesis or the decay on gernerator level  

   The plugin selects for the muon that is poart of the top hypothesis or for the muon that is 
   part of the leptonic top decay on generator level. It takes the TtSemiLeptonicEvent as input 
   and produces a std::vector<pat::Muon>, which can be further processed with standard analyzers
**/

class TopMuonSelector : public edm::EDProducer {

 public:
  /// default constructor
  explicit TopMuonSelector(const edm::ParameterSet& configFile);
  /// default destructor
  ~TopMuonSelector();
  
 private:
  /// produce function
  virtual void produce(edm::Event& event, const edm::EventSetup& setup);

 private:
  /// input collection label
  edm::InputTag inputCollection_;
  /// ttSemiLeptonicEvent label
  edm::InputTag ttSemiLeptonicEvent_;
  /// hypothesis key
  std::string hypoKey_;
};

#endif
