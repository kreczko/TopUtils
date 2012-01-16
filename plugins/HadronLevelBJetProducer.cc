#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "AnalysisDataFormats/TopObjects/interface/TtGenEvent.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"

class HadronLevelBJetProducer : public edm::EDProducer {

public:
    HadronLevelBJetProducer( const edm::ParameterSet& );

private:
    edm::InputTag ttGenEvent_;
    edm::InputTag genJets_;
    double deltaR_;

    virtual void produce ( edm::Event&, const edm::EventSetup& );
    virtual void beginJob();
    const int getGenJetNear ( const reco::Candidate* bQuark, const reco::GenJetCollection& genJets );

};

HadronLevelBJetProducer::HadronLevelBJetProducer(const edm::ParameterSet& cfg):
    ttGenEvent_(cfg.getParameter<edm::InputTag>("ttGenEvent")),
    genJets_(cfg.getParameter<edm::InputTag>("genJets")),
    deltaR_(cfg.getParameter<double>("deltaR"))

{
    produces<int>("BJetIndex");
    produces<int>("AntiBJetIndex");
}

void HadronLevelBJetProducer::beginJob()
{
}

void HadronLevelBJetProducer::produce(edm::Event& evt, const edm::EventSetup& setup)
{
    std::auto_ptr<int> bIdx(new int);
    std::auto_ptr<int> AntibIdx(new int);
    
    edm::Handle<TtGenEvent> genEvt;
    evt.getByLabel(ttGenEvent_, genEvt );

    edm::Handle<reco::GenJetCollection> genJets;
    evt.getByLabel(genJets_, genJets);
    
    *bIdx = getGenJetNear(genEvt->b(), *genJets);
    *AntibIdx = getGenJetNear(genEvt->bBar(), *genJets);
    
    evt.put(bIdx, "BJetIndex");
    evt.put(AntibIdx, "AntiBJetIndex");
}

const int HadronLevelBJetProducer::getGenJetNear ( const reco::Candidate* bQuark, const reco::GenJetCollection& genJets )
{
    int result = -1;
    double bestDr = deltaR_;
    for (size_t i = 0; i < genJets.size(); ++i) {
        double dr = deltaR(genJets[i].eta(), genJets[i].phi(), bQuark->eta(), bQuark->phi());
        if (dr < bestDr) {
            bestDr = dr;
            result = i;
        }
    }
    return result;
}

DEFINE_FWK_MODULE( HadronLevelBJetProducer );
