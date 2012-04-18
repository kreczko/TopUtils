#ifndef GenCandSelector_h
#define GenCandSelector_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

/**
   \class   GenCandSelector GenCandSelector.h "TopAnalysis/TopUtils/interface/GenCandSelector.h"

   \brief   Module to select a single generator particle for cross section measurements

   This is a module to select generator particles as daughters of a gauge boson which again might 
   be a daughter of a top quark for cross section measurements. The output is a std::vector of 
   reco::GenParticles of arbitary length. The parameters of the module are:

    _src_          : input collection of generator particles. This can be the genParticle 
                     collection or any derived (or pruned) collection of type reco::GenParticle.

    _target_       : parameter set for the configuration of the target particle(s). This para-
                     meter set is expected to hold the parameters, _pdgId_ and _status_, for the 
		     configuration of the target particle(s) as described below.

    _pdgId_        : vector of pdgIds of the target particle(s). The parameter is expected to be 
                     of type std::vector<std::string>. The elements of the std::vector can be the 
		     bare numbers (as strings) or of type 'part:daug', where part indicates the 
		     pdgId of the target particle and daug the pdgId of a potential daughter par-
		     ticle. If of type 'part:daug' only those particles will be chosen as target 
		     particles, which have a daughter particle with pdgId daug. Both particle & 
		     anti-particle are selected. Example: to choose both muons and muons from 
		     tau decays at the same time the configuration has to be of type: pdgId = 
		     ["13", "15:13"]. 

    _status_       : status of the target particle(s). Note that status acts on the particle 
                     that is located most to the left in a given pdgId string.

    _ancestor_     : parameter set for the configuration of the ancestor particle(s). This para-
                     meter set is expected to hold the parameter _pdgId_ for the configuration 
		     of the ancestor particle(s) as described below.

    _ancestorIgnore_  : parameter set for the configuration of particle(s) that should not be ancestor(s). 
                        This parameter set is expected to hold the parameter _pdgId_ for the configuration 
		        of the ancestor particle(s) as described below.

    _pdgId_        : vector of pdgIds of the ancestor particle(s). E.g. this might be W's or Z's. 
                     The elements of the vector can be bare numbers (as strings) or of type 
		     'ance:part', where part indicates the pdgId of the ancestor particle and 
		     ance the pdgId of a potential further ancester particle. If of type 
		     'ance:part' only those particles will be allowed, which have an additional 
		     ancestor particle with pdgId ance. Both particle and anti-particle are 
		     selected. Example: to choose inclusive W's and Z's the configuration has to 
		     be of type: pdgId = ["23", "24"]. To choose exclusively W's, which originate 
		     from top quarks the configuration has to be of type pdgId = ["6:23"]. 

   The module was tested on generator particle listings of pythia. Extensions might be needed for 
   the module to be also applicable on MC at NLO like particle listings. Further testing will be 
   necessary here.
*/

class GenCandSelector : public edm::EDProducer {

 public:
  /// default constructor
  explicit GenCandSelector(const edm::ParameterSet&);
  /// default destructor
  ~GenCandSelector(){};
  /// print properties from all ancestors
  void MotherPrinter(edm::LogVerbatim& log, const reco::Candidate* p) const;
  
 private:
  /// produce function to select the 
  virtual void produce(edm::Event&, const edm::EventSetup&);

  /// returns substring up to first appearance of ':' or full string if no ':' is found
  std::string firstElement (const std::string& label, const char* delim=":") const { return label.substr(0, label.find(delim)); };  
  /// returns substring from first appearance of ':' up to end of emtpy string if no ':' is found
  std::string secondElement(const std::string& label, const char* delim=":") const { return (label.find(delim)!=std::string::npos)?label.substr(label.find(':')+1):std::string(""); };  
  /// find p in list of allowed particles
  int find(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p, bool firstList=true) const;
  /// check whether p is contained in list of allowed particles or not
  bool contained(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p, bool firstList=true) const;
  /// find descendant which is in list of allowed particles downstream the decay chain
  bool descendant(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p) const;
  /// find ancestor which is in list of allowed particles upstream the decay chain
  bool ancestor(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p) const;
  /// object logging for debugging purposes
  void print(const reco::Candidate* p) const;

 private:
  /// input collection
  edm::InputTag src_;
  /// pdgId of the target particle(s); the first element corresponds 
  /// to the pdgId of the target particle, the second element to the 
  /// pdgId of the daughter. If no daughter is given the second ele-
  /// ment will be set to 0
  std::vector<std::pair<int, int> > daughterIds_;
  /// status of the target particle(s)
  int status_;
  /// pgdId of ancestor particle(s); the second element corresponds 
  /// to the pdgId of the target particle, the first element to the 
  /// pdgId of a potentail further ancestor particle. If no ancestor 
  /// is given the first element will be set to 0 
  std::vector<std::pair<int, int> > ancestorIds_;
  /// pgdId of excluded ancestor particle(s); the second element corresponds 
  /// to the pdgId of the target particle, the first element to the 
  /// pdgId of a potentail further ancestor particle. If no ancestor 
  /// is given the first element will be set to 0 
  //std::vector<std::pair<int, int> > ancestorIgnoreIds_;

};

/// check whether the pdgId of the given candidates is element if the std::vector of std::pair, '0' is used as wildcart
inline int 
GenCandSelector::find(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p, bool firstList) const {
  for(std::vector<std::pair<int, int> >::const_iterator elem=first; elem!=last; ++elem){ 
    if( firstList?elem->first==abs(p->pdgId()):elem->second==abs(p->pdgId()) ){
      return (elem-first);
    }
  } 
  return (last-first);
}

/// check whether p is contained in list of allowed particles or not
inline bool 
GenCandSelector::contained(const std::vector<std::pair<int, int> >::const_iterator& first, const std::vector<std::pair<int, int> >::const_iterator& last, const reco::Candidate* p, bool firstList) const {
  return (find(first, last, p, firstList)<(last-first));
} 

#endif
