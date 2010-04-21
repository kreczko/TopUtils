#ifndef GenCandSelector_h
#define GenCandSelector_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

/**
   \class   GenCandSelector GenCandSelector.h "TopAnalysis/TopUtils/interface/GenCandSelector.h"

   \brief   Module to select a single generator particle for cross section measurements

   This is a module to select generator particles as daughters of a gauge boson which again might 
   be daughters of a top quark for cross section measurements. The output is a std::vector of 
   reco::GenParticles of arbitary length. The parameters of the module are:

    _src_          : input collection of generator particles. It can be the genParticle collection 
                     or any derived (or pruned) collection of same type reco::GenParticle.

    _target_       : parameter set for the configuration of the target particle. This parameter set
                     is expected to hold the parameters, _pdgId_ and _status_, for the configura-
		     tion of the generator particle as described below.

    _pdgId_        : pdgId of the target particle(s). The parameter is expected to be unsigned. 
                     Both particle and anti-particle are selected.

    _status_       : status of the target particle(s).

    _mother_       : parameter set for the configuration of the mother particle(s). This parameter
                     set is expected to hold the parameters _pdgIds_ and _pdgIdMother_ for the con-
		     figuration of the mother particle(s) as described below.

    _pdgIds_       : vector of pdgIds of the mother particle(s). E.g. this might be W's and Z's. 
                     This parameter is expected to be unsigned, both particles and anti-particles 
		     are allowed for the search.

    _ancestor_     : pdgId of the mother particle of the mother particle. This parameter is option-
                     al. It does not have to present in the definition of the module.

   The module acts on generator particle listings of pythia. Extensions might be needed for the 
   module to be also applicable on MC at NLO like particle listings. Further testing will be 
   necessary here.
*/

class GenCandSelector : public edm::EDProducer {

 public:
  /// default constructor
  explicit GenCandSelector(const edm::ParameterSet&);
  /// default destructor
  ~GenCandSelector(){};
  
 private:
  /// produce function to select the 
  virtual void produce(edm::Event&, const edm::EventSetup&);
  /// find ancestor of given type upstream the particle chain
  bool findAncestor( const reco::Candidate* part, int& type);

 private:
  /// pdgId of mother particle of mother particles
  int ancestor_;
  /// input collection
  edm::InputTag src_;
  /// pdgId of the target particle
  unsigned int pdgId_;
  /// status of the target particle
  unsigned int status_;
  /// vector of pgdIds of the mother particle(s)
  std::vector<unsigned int> pdgIds_;
};

#endif
