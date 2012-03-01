// -*- C++ -*-
//
// Package:    GenLevelBJetProducer
// Class:      GenLevelBJetProducer
//
/**\class GenLevelBJetProducer GenLevelBJetProducer.cc blutz/GenLevelBJetProducer/src/GenLevelBJetProducer.cc

Description: identifies the b-Jet inside the generator jets by matching it with the b-hadron

Implementation:
see details in the description of the  function getGenJetWith

*/

//
// Original Author:  Benjamin Lutz,,,DESY
//         Created:  Thu Feb  2 13:30:58 CET 2012
// $Id: GenLevelBJetProducer.cc,v 1.2 2012/03/01 11:40:34 blutz Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

// added by me
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "AnalysisDataFormats/TopObjects/interface/TtGenEvent.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"


//
// class declaration
//

class GenLevelBJetProducer : public edm::EDProducer {
public:
  explicit GenLevelBJetProducer(const edm::ParameterSet&);
  ~GenLevelBJetProducer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  virtual void beginRun(edm::Run&, edm::EventSetup const&);
  virtual void endRun(edm::Run&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

  std::vector<int> getGenJetWith( const reco::Candidate* bQuark, const reco::GenJetCollection& genJets );
  int getGenJetNear( const reco::Candidate* particle, std::vector<const reco::GenJet*> &genJets );
  typedef const reco::Candidate* pCRC;
  bool searchInMothers( const reco::Candidate* bQuark, const reco::Candidate* thisParticle, std::vector<const reco::Candidate*> particleChain, pCRC *bHadron);
  bool checkForLoop(std::vector<const reco::Candidate*> &particleChain, const reco::Candidate* particle);
  std::string getParticleName(int id) const;

  inline std::string printJetInfo(const size_t iJet, const reco::GenJet* ) const;
  inline std::string printParticleChain(const std::vector<const reco::Candidate*> &particleChain, const reco::Candidate *bHadron) const;

  // ----------member data ---------------------------
  edm::InputTag ttGenEvent_;
  edm::InputTag genJets_;
  double deltaR_;
  bool noBBbarResonances_;
  bool resolveName_;

  edm::ESHandle<ParticleDataTable> pdt_;

};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
GenLevelBJetProducer::GenLevelBJetProducer(const edm::ParameterSet& cfg) {

  ttGenEvent_        = cfg.getParameter<edm::InputTag>("ttGenEvent");
  genJets_           = cfg.getParameter<edm::InputTag>("genJets");
  deltaR_            = cfg.getParameter<double>("deltaR");
  noBBbarResonances_ = cfg.getParameter<bool>("noBBbarResonances");
  resolveName_       = cfg.getParameter<bool>("resolveParticleName");

  produces< std::vector<int> >("BHadJetIndex");
  produces< std::vector<int> >("AntiBHadJetIndex");

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void GenLevelBJetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {

  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("ttGenEvent",edm::InputTag("genEvt"));
  desc.add<edm::InputTag>("genJets",edm::InputTag("ak5GenJets","","HLT"));
  desc.add<double>("deltaR",.5);
  desc.add<bool>("noBBbarResonances",true);
  desc.add<bool>("resolveParticleName",false);
  descriptions.add("produceGenLevelBJets",desc);
}

GenLevelBJetProducer::~GenLevelBJetProducer() {
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void GenLevelBJetProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {

  //  setup.getData( pdt_ );

  using namespace edm;

  std::auto_ptr<std::vector<int> > bIdx(new std::vector<int>);
  std::auto_ptr<std::vector<int> > AntibIdx(new std::vector<int>);

  edm::Handle<TtGenEvent> genEvt;
  evt.getByLabel(ttGenEvent_, genEvt );

  edm::Handle<reco::GenJetCollection> genJets;
  evt.getByLabel(genJets_, genJets);

  LogDebug("bJet") << "searching for b-jets in " << genJets_;
  *bIdx = getGenJetWith(genEvt->b(), *genJets);
  LogDebug("bJet") << "searching for anti-b-jets in " << genJets_;
  *AntibIdx = getGenJetWith(genEvt->bBar(), *genJets);

  evt.put(bIdx, "BHadJetIndex");
  evt.put(AntibIdx, "AntiBHadJetIndex");

}

// ------------ method called once each job just before starting event loop  ------------
void GenLevelBJetProducer::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void GenLevelBJetProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void GenLevelBJetProducer::beginRun(edm::Run&, edm::EventSetup const&) {
}

// ------------ method called when ending the processing of a run  ------------
void
GenLevelBJetProducer::endRun(edm::Run&, edm::EventSetup const&) {
}

// ------------ method called when starting to processes a luminosity block  ------------
void GenLevelBJetProducer::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&) {
}

// ------------ method called when ending the processing of a luminosity block  ------------
void GenLevelBJetProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&) {
}

// ------------ helper functions -------------

/**
 * identify the jets that contain b-hadrons
 *
 * All jets which have a b-hadron with the right b content (b or anti-b) are identified in the GenJetCollection.
 * b-bbar resonances can either be excluded or included. The b-hadrons are not required to originate from a b-quark.
 * Only jets within the given @f$\delta_{r}@f$ are accepted.
 * In case more than one jet originates from the same b-hadron the jet which is nearer in @f$\delta_{r}@f$ is chosen.
 *
 * @todo add option to restrict to b hadrons coming from b-quarks
 *
 * @param[in] bQuark used to set the charge of the b-Hadrons
 * @param[in] genJets the GenJetCollection to be searched
 * @returns a list with the indicies of the GenJets that fulfill the requirements
 */
std::vector<int> GenLevelBJetProducer::getGenJetWith ( const reco::Candidate* bQuark, const reco::GenJetCollection& genJets ) {

  std::vector<int> result;

  std::vector<const reco::GenJet*> bQuarkCandidates;
  std::vector<const reco::GenJet*> bHadronCandidates;
  std::vector<size_t> bQuarkCandidateIndex;
  std::vector<size_t> bHadronCandidateIndex;
  std::vector<unsigned int> nBquarkParticles;
  std::vector<unsigned int> nBhadronParticles;
  std::vector<const reco::Candidate*> motherBhadron;

  typedef std::map<double, const reco::Candidate*> bHadronMap_t;
  bHadronMap_t bHadronList;
  typedef std::map<double, std::vector<size_t> > jetCandidatesVsBhadron_t;
  jetCandidatesVsBhadron_t jetCandidatesVsBhadron;

  for (size_t iJet = 0; iJet < genJets.size(); ++iJet) {
    const reco::GenJet* thisJet = &(genJets[iJet]);
    std::vector<const reco::GenParticle*> particles = thisJet->getGenConstituents();


    LogTrace("Jets") << printJetInfo(iJet, thisJet);

    bool isBquarkCandidate = false;
    bool isBhadronCandidate = false;
    unsigned int nBquarkDaughters = 0;
    unsigned int nBhadronDaughters = 0;
    bHadronMap_t bHadrons;

    for (unsigned int iParticle = 0; iParticle < particles.size(); ++iParticle) {
      const reco::GenParticle* thisParticle = particles[iParticle];
      const reco::Candidate* bHadron = 0;
      std::vector<const reco::Candidate*> particleChain;
      checkForLoop(particleChain,thisParticle);
      if (searchInMothers(bQuark, thisParticle, particleChain, &bHadron)) {
        isBquarkCandidate = true;
        ++nBquarkDaughters;
      }
      if (bHadron) {
        isBhadronCandidate = true;
        ++nBhadronDaughters;
        bHadrons[bHadron->p()] = bHadron;
        bHadronList[bHadron->p()] = bHadron;
      }
    }
    if (isBquarkCandidate) {
      bQuarkCandidates.push_back(thisJet);
      bQuarkCandidateIndex.push_back(iJet);
      nBquarkParticles.push_back(nBquarkDaughters);
    }
    if (isBhadronCandidate) {
      bHadronCandidates.push_back(thisJet);
      bHadronCandidateIndex.push_back(iJet);
      nBhadronParticles.push_back(nBhadronDaughters);
      motherBhadron.push_back(bHadrons.begin()->second);
      for ( bHadronMap_t::iterator it = bHadrons.begin(); it != bHadrons.end(); ++it) {
        if (it->second->pt() > motherBhadron.back()->pt()) motherBhadron.back() = it->second;
        jetCandidatesVsBhadron[it->first].push_back(iJet);
      }
    }
  }

  // debug output no functionality
#ifdef EDM_ML_DEBUG
  LogTrace("bQuark")  << "found " << bQuarkCandidates.size() << " jets with b-quark particles" << std::endl;
  LogTrace("bHadron") << "found " << bHadronCandidates.size() << " jets with b-hadron particles" << std::endl
                      << "found " << bHadronList.size() << " different b-Hadrons" << std::endl;

  for (unsigned int i = 0; i < bQuarkCandidates.size(); ++i) {
    const reco::GenJet *jet = bQuarkCandidates.at(i);
    size_t index = bQuarkCandidateIndex.at(i);
    LogTrace("bQuark") << "GenJet " << index << " has " << nBquarkParticles.at(i) << "/" << jet->getGenConstituents().size() << " b-quark daughters" << std::endl
                       << "GenJet " << index << " " << jet->p4() << " p: " << jet->p() << " p_T: " << jet->pt() << std::endl
                       << "GenJet " << index << " eta: " << jet->eta() << " phi: " << jet->phi() << std::endl;
  }


  for (unsigned int i = 0; i < bHadronCandidates.size(); ++i) {
    const reco::GenJet *jet = bHadronCandidates.at(i);
    size_t index = bHadronCandidateIndex.at(i);
    const reco::Candidate *bMother = motherBhadron.at(i);
    LogTrace("bHadron") << "GenJet " << index << " has " << nBhadronParticles.at(i) << "/" << jet->getGenConstituents().size() << " b-hadron daughters" << std::endl
                        << "GenJet " << index << " " << jet->p4() << " p: " << jet->p() << " p_T: " << jet->pt() << std::endl
                        << "GenJet " << index << " eta: " << jet->eta() << " phi: " << jet->phi() << std::endl
                        << "GenJet " << index << " mother b-hadron: " << getParticleName(bMother->pdgId()) <<  std::endl
                        << "GenJet " << index << " mother b-hadron: " << bMother->p4() << " p: " << bMother->p() << " p_T: " << bMother->pt() << std::endl
                        << "GenJet " << index << " mother b-hadron: eta: " << bMother->eta() << " phi: " << bMother->phi() << std::endl;
  }
#endif // EDM_ML_DEBUG -- debug output

  for ( jetCandidatesVsBhadron_t::iterator it = jetCandidatesVsBhadron.begin(); it != jetCandidatesVsBhadron.end(); ++it ) {

    const reco::Candidate *bHadron = bHadronList[it->first];
    std::vector<size_t> candidateIdx = it->second;
    LogTrace("bHadron") << "analysing jets for " << getParticleName(bHadron->pdgId()) << std::endl;
    LogTrace("bHadron") << "   " << candidateIdx.size() <<  " candidate jets: ";

    std::vector<const reco::GenJet*> jetCandidates;
    for (unsigned int idx = 0; idx < candidateIdx.size(); ++idx) {
      LogTrace("bHadron") << "     " << candidateIdx[idx] << std::endl;
      jetCandidates.push_back(&(genJets[candidateIdx[idx]]));
    }

    int bestJet = getGenJetNear(bHadron, jetCandidates);
    if (bestJet >= 0) {
      result.push_back(candidateIdx.at(bestJet));
      LogTrace("bHadron") << "    best matching jet: " << candidateIdx.at(bestJet) << std::endl;
    }
    else {
      LogTrace("bHadron") << "    no matching jet in cone of " << deltaR_ << std::endl;
    }
  }

  return result;
}

/**
 * helper function to keep track of the decay chain and identify loops in the decay tree
 *
 * @param[out] vector of particles building up the current chain
 * @param[in] particle particle that should be added
 *
 * returns true if a particle is already in the chain
 */
bool GenLevelBJetProducer::checkForLoop(std::vector<const reco::Candidate*> &particleChain, const reco::Candidate* particle) {

  for (unsigned int i = 0; i < particleChain.size(); ++i)
    if (particleChain[i] ==  particle) return true;

  particleChain.push_back(particle);
  return false;
}

/**
 * do a recursive search for the mother particles until the b-quark is found or the absolute mother is found
 *
 * the treatment of b-bar resonances depends on the global parameter noBBbarResonances_
 *
 * @param[in] bQuark ancestor b-quark for the current search
 * @param[in] thisParticle current particle to be analysed
 * @param[in] particleChain current version of the search path
 * @param[out] bHadron the oldest b-hadron found in the search
 *
 * @returns if the b-quark was found or not
 */
bool GenLevelBJetProducer::searchInMothers(const reco::Candidate* bQuark, const reco::Candidate* thisParticle, std::vector<const reco::Candidate*> particleChain, pCRC *bHadron) {

  if ( thisParticle->pdgId() / 1000  == bQuark->pdgId()  // b-baryions
       || ( thisParticle->pdgId() / 100 % 10 == -bQuark->pdgId() // b-mesons
            && ! ( noBBbarResonances_ && thisParticle->pdgId() / 10 % 100 == 55 ) ) // not a b-bbar resonance
       ) *bHadron = thisParticle;

  for (size_t iMother = 0; iMother < thisParticle->numberOfMothers(); ++iMother) {

    const reco::Candidate* mother = thisParticle->mother(iMother);
    if ( checkForLoop(particleChain, mother) ) {
      edm::LogWarning("decayChain") << "Identified a loop in the current decay chain."
                                    << " Paricle " << mother
                                    << " PDG: " << mother->pdgId()
                                    << " " << mother->p4()
                                    << " is already in the chain."
                                    << " Will stop this branch here. B-hadron identification might be incomplete." ;
      return false;
    }
    if (bQuark->p4() == mother->p4() && bQuark->pdgId() == mother->pdgId() && bQuark->status() == mother->status()) {
      LogTrace("decayChain") << printParticleChain(particleChain,*bHadron);
      return true;
    }
    else if (searchInMothers(bQuark, mother, particleChain, bHadron)) return true;
  }

  return false;
}

std::string GenLevelBJetProducer::getParticleName(int id) const
{

  const ParticleData * pd = 0;
  if (resolveName_) pdt_->particle( id );
  if (!pd) {
    std::ostringstream ss;
    ss << "P" << id;
    return ss.str();
  }
  else
    return pd->name();

}


/**
 * helper function to print jet properies
 *
 * this function is inlined to allow efficient optimisation when compiled without debug option
 *
 * @param[in] iJet the index of the jet
 * @param[in] thisJet pointer to the jet
 *
 * @returns string to be printed
 */
inline std::string GenLevelBJetProducer::printJetInfo(const size_t iJet, const reco::GenJet* thisJet) const {
  std::ostringstream out;
  out <<  "Jet: " << iJet
      << " particles: " << thisJet->getGenConstituents().size()
      << " p4: " << thisJet->p4()
      << " p_t: " << thisJet->pt()
      << std::endl;
  return out.str();
}

/**
 * helper function to generate a human readable representation of the decay chain
 *
 * this function is inlined to allow efficient optimisation when compiled without debug option
 *
 * @param[in] particleChain the decay chain
 * @param[in] bHadron the identified b-hadron
 *
 * @returns string to be printed
 */
inline std::string GenLevelBJetProducer::printParticleChain(const std::vector<const reco::Candidate*> &particleChain, const reco::Candidate* bHadron) const {
  std::ostringstream out;

  for (int i = particleChain.size()-1; i >= 0; --i) {
    if ( particleChain[i] == bHadron )
      out << "*" << getParticleName( particleChain[i]->pdgId() ) <<"*";
    else
      out << getParticleName( particleChain[i]->pdgId() );
    if (i>0) out  << "-->";
  }
  return out.str();
}


/**
 * identify which of the jets is the nearest to the particle
 *
 * @param particle reference particel
 * @param genJets candidate jets
 * @returns index of the best jet in the vector or -1 if no jet within the maximum @f$\delta_{r}@f
 */
int GenLevelBJetProducer::getGenJetNear( const reco::Candidate* particle, std::vector<const reco::GenJet*> &genJets ) {
  int result = -1;
  double bestDr = deltaR_;
  for (unsigned int i = 0; i < genJets.size(); ++i) {
    double dr = deltaR(genJets[i]->eta(), genJets[i]->phi(), particle->eta(), particle->phi());
    if (dr < bestDr) {
      bestDr = dr;
      result = i;
    }
  }
  return result;
}


//define this as a plug-in
DEFINE_FWK_MODULE(GenLevelBJetProducer);
