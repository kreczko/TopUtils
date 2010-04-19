#include <string>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

/**
   \class   GenFinalStateSelector GenFinalStateSelector.h "TopAnalysis/TopUtils/plugins/GenFinalStateSelector.h"

   \brief   Plugin to select top antitop events according to expected final state particles

   The module filters top antitop events on generator level according to expected final state 
   particles in contrary to the decay channel. Considered are electrons and muons in the final 
   state. Configuration parameters are: 

    * src    : input source (as edm::InputTag)
    * elecs  : number of electrons in the final state
    * muons  : number of muons in the final state
    * invert : invert the selection

   the parameters elecs, muons or invert may be omitted from the configuration file. If 
   omitted or set to -1 the corresponding lepton flavor will not be considered in the final 
   state classification. If omitted of set to false the classification will not be inverted.
*/


class GenFinalStateSelector : public edm::EDFilter {

 public:
  /// constructor
  explicit GenFinalStateSelector(const edm::ParameterSet& cfg);
  /// destructor
  ~GenFinalStateSelector();
     
 private:
  /// all that needs to done during the event loop
  virtual bool filter(edm::Event& event, const edm::EventSetup& setup);

 private:
  /// generated particle collection src
  edm::InputTag src_;
  /// number of expected electrons or muons in the final state
  int elecs_, muons_;
  /// switch to invert the selevtion
  bool invert_;
};
