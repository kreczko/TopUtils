#ifndef ParticleEnergyScale_h
#define ParticleEnergyScale_h

/** \class ParticleEnergyScale
 *
 * Vary energy of electrons/muons/tau-jets by +/- 1 standard deviation, 
 * in order to estimate resulting uncertainty. Variation is propagated to MET
 *
 * \author Original by Christian Veelken, LLR
 *
 *
 */

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include <string>
#include <vector>

template <typename T>
class ParticleEnergyScale : public edm::EDProducer  
{
  typedef std::vector<T> ParticleCollection;

 public:

  explicit ParticleEnergyScale(const edm::ParameterSet& cfg)
    : moduleLabel_(cfg.getParameter<std::string>("@module_label"))
  {
    src_ = cfg.getParameter<edm::InputTag>("src");
    mets_ = cfg.getParameter<edm::InputTag>("mets");

    shiftBy_ = cfg.getParameter<double>("shiftBy");

    if ( cfg.exists("binning") ) {
      typedef std::vector<edm::ParameterSet> vParameterSet;
      vParameterSet cfgBinning = cfg.getParameter<vParameterSet>("binning");
      for ( vParameterSet::const_iterator cfgBinningEntry = cfgBinning.begin();
        cfgBinningEntry != cfgBinning.end(); ++cfgBinningEntry ) {
        binning_.push_back(new binningEntryType(*cfgBinningEntry));
      }
    }
    else {
      double uncertainty = cfg.getParameter<double>("uncertainty");
      binning_.push_back(new binningEntryType(uncertainty));
    }
    
    // use label of input to create label for output
    outputParticles_  = src_.label();
    outputMETs_       = "METs";

    // register products
    produces<ParticleCollection>(outputParticles_);
    produces<std::vector<pat::MET> >(outputMETs_);
  }
  ~ParticleEnergyScale()
  {
    for ( typename std::vector<binningEntryType*>::const_iterator it = binning_.begin();
    it != binning_.end(); ++it ) {
      delete (*it);
    }
  }
    
 private:

  void produce(edm::Event& evt, const edm::EventSetup& es)
  {
    edm::Handle<ParticleCollection> originalParticles;
    evt.getByLabel(src_, originalParticles);
    
    edm::Handle<std::vector<pat::MET> > originalMETs;
    evt.getByLabel(mets_, originalMETs);

    std::auto_ptr<ParticleCollection> shiftedParticles(new ParticleCollection);
    std::auto_ptr<std::vector<pat::MET> > shiftedMETs(new std::vector<pat::MET>);

    double dPx = 0., dPy = 0., dSumEt = 0.;

    for ( typename ParticleCollection::const_iterator originalParticle = originalParticles->begin();
    originalParticle != originalParticles->end(); ++originalParticle ) {

      double uncertainty = 0.;
      for ( typename std::vector<binningEntryType*>::iterator binningEntry = binning_.begin();
        binningEntry != binning_.end(); ++binningEntry ) {
        if ( (!(*binningEntry)->binSelection_) || (*(*binningEntry)->binSelection_)(*originalParticle) ) {
          uncertainty = (*binningEntry)->binUncertainty_;
          break;
        }
      }
      
      double shift = shiftBy_*uncertainty;

      reco::Candidate::LorentzVector shiftedParticleP4 = originalParticle->p4();
      shiftedParticleP4 *= (1. + shift);

      T shiftedParticle(*originalParticle);      
      shiftedParticle.setP4(shiftedParticleP4);

      shiftedParticles->push_back(shiftedParticle);
      
      dPx    += shiftedParticle.px() - originalParticle->px();
      dPy    += shiftedParticle.py() - originalParticle->py();
      dSumEt += shiftedParticle.et() - originalParticle->et();
    }

    
    // scale MET accordingly
    pat::MET met = *(originalMETs->begin());
    double scaledMETPx = met.px() - dPx;
    double scaledMETPy = met.py() - dPy;
    pat::MET scaledMET(reco::MET(met.sumEt()+dSumEt, reco::MET::LorentzVector(scaledMETPx, scaledMETPy, 0, sqrt(scaledMETPx*scaledMETPx+scaledMETPy*scaledMETPy)), reco::MET::Point(0,0,0)));
    shiftedMETs->push_back( scaledMET );
    
    evt.put(shiftedParticles, outputParticles_);
    evt.put(shiftedMETs, outputMETs_);
    
    //evt.put(shiftedParticles);
  }

  std::string moduleLabel_;

  edm::InputTag src_;
  edm::InputTag mets_;
  
  std::string outputParticles_;
  std::string outputMETs_;

  struct binningEntryType
  {
    binningEntryType(double uncertainty)
      : binSelection_(0),
        binUncertainty_(uncertainty)
    {}
    binningEntryType(const edm::ParameterSet& cfg)
    : binSelection_(new StringCutObjectSelector<T>(cfg.getParameter<std::string>("binSelection"))),
      binUncertainty_(cfg.getParameter<double>("binUncertainty"))
    {}
    ~binningEntryType() 
    {
      delete binSelection_;
    }
    StringCutObjectSelector<T>* binSelection_;
    double binUncertainty_;
  };
  std::vector<binningEntryType*> binning_;

  double shiftBy_; // set to +1.0/-1.0 for up/down variation of energy scale
};

#endif
