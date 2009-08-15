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

   \brief   Plugin to select for the muon, which is part of the top hypothesis or best matching to the decay on generator level  

   The plugin selects for the muon that is part of the top hypothesis or for the muon with the best match 
   to the muon from the leptonic top decay on generator level. It has the TtSemiLeptonicEvent as input &
   produces two std::vector<pat::Muon>'s, which can be further processed with standard analyzers. The output 
   labels are: 

   * selectedLayer1TopMuon:rec   :   for the muon from the ttbar hypothesis 
   * selectedLayer1TopMuon:gen   :   for the muon that matches best to the muon from the leptonic decay 
                                     on generator level (in deltaR)

   For the kGenMatch hypothesis both muons should be the same.
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
