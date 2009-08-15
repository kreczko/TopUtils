#ifndef TopJetsSelector_h
#define TopJetsSelector_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

namespace pat{ class Jet; }

/*
   \class   JetsJetSelector JetsJetSelector.h "TopAnalysis/TopUtils/interface/JetsJetSelector.h"

   \brief   Plugin to select for the jets, which are part of the top hypothesis or best matching to the decay on generator level  

   The plugin selects for the jets that are part of the top hypothesis or for the jets with the best match 
   to the jets from the ttbar decay on generator level. It has the TtSemiLeptonicEvent as input & produces 
   a std::vector<pat::Jets>'s, which can be further processed with standard analyzers. It can be further 
   specified to select for several types of jets ('BJet', 'LighQuark', 'All') and to choose the jets, which 
   are part of the hypothesis/decay or those which are not. The output labels are: 

   * selectedLayer1TopJets:rec   :   for the jets from the ttbar hypothesis 
   * selectedLayer1TopJets:gen   :   for the jets that match best to the jets from the ttbar decay on
                                     generator level (in deltaR)

   There is no distinction of the users choice for the additional selection criteria in the labeling of 
   the output collections. For the kGenMatch hypothesis both jet collections should be the same.
**/

class TopJetsSelector : public edm::EDProducer {

 public:
  /// default constructor
  explicit TopJetsSelector(const edm::ParameterSet& configFile);
  /// default destructor
  ~TopJetsSelector();
  
 private:
  /// produce function
  virtual void produce(edm::Event& event, const edm::EventSetup& setup);
  /// helper function to decide whether the element should be pushed or not
  bool pushElement(const int& idx, const std::vector<int>& match);

 private:
  /// input collection label
  edm::InputTag inputCollection_;
  /// generatorMatch label
  edm::InputTag generatorMatch_;
  /// ttSemiLeptonicEvent label
  edm::InputTag ttSemiLeptonicEvent_;
  /// hypothesis key
  std::string hypoKey_;
  // specify for 'BJets', 'LightQuark' or 'All'
  std::string jetType_;
  // select for top jets or not
  bool partOfTheHypothesis_;
};

#endif
