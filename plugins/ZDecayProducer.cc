#include <memory>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"

/** \brief Produces an int zDecayMode corresponding to the Z decay
  *
  * This filter can select certain Z -> ll events. Also
  * it produces the zDecayMode information.
  * zDecayMode is:
  *  + 0 for decays into hadrons
  *  + 1111 for decays into ee, 12 for nu_e
  *  + 1313 for decays into mumu, 14 for nu_mu
  *  + 15111511 for decays into tautau further decaying into ee
  *  + 15111513 for decays into tautau further decaying into e-mu+ (first the particle, then anti-particle)
  *  + 15131511 for decays into tautau further decaying into e+mu-
  *  + 15131513 for decays into tautau further decaying into mumu
  *  + 150150 for decays into tautau further decaying into hadrons
  *  + 1511150 for decays into tautau further decaying e- and hadrons
  *  + 1501511 for decays into tautau further decaying hadrons and e+
  *  + 1513150 and 1501513 tautau -> mu-/+ and hadrons
  *  + 1616 for nu_tau
  */

class ZDecayProducer : public edm::EDProducer {
public:
    explicit ZDecayProducer(const edm::ParameterSet&);
    ~ZDecayProducer();

private:
    virtual void beginJob() ;
    virtual void produce(edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;
      
    int tauDecay(const reco::GenParticle*) const;
    int leptonicOrZero(int id) const;
    edm::InputTag src_;      
};


ZDecayProducer::ZDecayProducer(const edm::ParameterSet& cfg):
  src_(cfg.getParameter<edm::InputTag>("src"))
{
    produces<std::vector<int> >("zDecayMode");
}


ZDecayProducer::~ZDecayProducer()
{
}


int ZDecayProducer::tauDecay(const reco::GenParticle* part) const
{
    for(size_t i = 0; i < part->numberOfDaughters(); ++i) {
        if (abs(part->daughter(i)->pdgId())==11)
            return 11;
        if (abs(part->daughter(i)->pdgId())==13)
            return 13;
        
        // In many cases tau radiates a photon before it decays.
        // So one daughter is again a tau. Use Recursion then.
        if (abs(part->daughter(i)->pdgId())==15)    
            return tauDecay(dynamic_cast<const reco::GenParticle*>(part->daughter(i)));   
    }  
    return -1;
}

int ZDecayProducer::leptonicOrZero(int id) const {
    int absid = abs(id);
    if (absid >= 11 && absid <= 18) //leptonic decays
        return absid;
    return 0;
}

void ZDecayProducer::produce(edm::Event& evt, const edm::EventSetup& es)
{
    edm::Handle<reco::GenParticleCollection> genParticles;
    evt.getByLabel(src_, genParticles);
  
    std::auto_ptr<std::vector<int> > zDecayMode(new std::vector<int>);
    for(reco::GenParticleCollection::const_iterator iGenPart = genParticles->begin(); 
        iGenPart != genParticles->end(); ++iGenPart)
    {
        //if it is not the decaying Z boson, skip this genParticle
        if (iGenPart->pdgId()!=23 || iGenPart->status()!=3) continue;
        
        int lepton = 0;
        int antiLepton = 0;
        
        for(size_t iDaughter = 0; iDaughter < iGenPart->numberOfDaughters(); ++iDaughter){
            const reco::GenParticle* daughter(dynamic_cast<const reco::GenParticle*>(iGenPart->daughter(iDaughter)));
            int id = leptonicOrZero(daughter->pdgId());
            
            //is decaying into a tau?
            if (id == 15) {
                id = 1500 + leptonicOrZero(tauDecay(daughter));
                if (id == 1500) id = 150;
            }
            
            if (daughter->pdgId() > 0) {
                if (lepton == 0) lepton = id;
            } else {
                if (antiLepton == 0) antiLepton = id;
            }
        }
        if (antiLepton < 10) lepton *= 10;
        else if (antiLepton < 100) lepton *= 100;
        else if (antiLepton < 1000) lepton *= 1000;
        else lepton *= 10000;
        zDecayMode->push_back(lepton + antiLepton);
    }
    evt.put(zDecayMode, "zDecayMode");
}


void 
ZDecayProducer::beginJob()
{
}


void 
ZDecayProducer::endJob() 
{
}

DEFINE_FWK_MODULE(ZDecayProducer);
