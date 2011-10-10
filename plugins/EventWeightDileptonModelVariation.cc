#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "AnalysisDataFormats/TopObjects/interface/TtGenEvent.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "TMath.h"

class EventWeightDileptonModelVariation : public edm::EDProducer {

public:
    explicit EventWeightDileptonModelVariation ( const edm::ParameterSet& );

private:
    virtual void produce ( edm::Event&, const edm::EventSetup& );
    virtual void beginJob();
    double getTopPtWeight();
    double getTopEtaWeight();
    double getTopMassWeight();
    double getTopMassWeightLandau();

    edm::InputTag ttGenEvent_;
    edm::Handle<TtGenEvent> genEvt;
    std::string weightVariable_;
    double slope_;
    double weight1x_;
    double minWeight_;
    double maxWeight_;
    double landauMPV_;
    //double landauConstant_;
    double landauSigma_;
    double landauMoveX_;
    TH1 *modelWeightSum_;
    TH1 *maxpt_, *maxptWeighted_;
    TH1 *maxeta_;
    TH1 *mass_, *massWeighted_;
};

EventWeightDileptonModelVariation::EventWeightDileptonModelVariation(const edm::ParameterSet& cfg):
    ttGenEvent_(cfg.getParameter<edm::InputTag>("ttGenEvent")),
    weightVariable_(cfg.getParameter<std::string>("weightVariable")),
    slope_(cfg.getParameter<double>("slope")),
    weight1x_(cfg.getParameter<double>("weight1x")),
    minWeight_(cfg.getParameter<double>("minWeight")),
    maxWeight_(cfg.getParameter<double>("maxWeight")),
    landauMPV_(cfg.getParameter<double>("landauMPV")),
    //landauConstant_(cfg.getParameter<double>("landauConstant")),
    landauSigma_(cfg.getParameter<double>("landauSigma")),
    landauMoveX_(cfg.getParameter<double>("landauMoveX"))
{
    produces<double>("");
}

void EventWeightDileptonModelVariation::beginJob()
{
    edm::Service<TFileService> fs;

    if( !fs ){
        throw edm::Exception( edm::errors::Configuration,
                            "TFile Service is not registered in cfg file" );
    }

    modelWeightSum_ = fs->make<TH1F>("modelWeightSum", "model weights", 2, 0., 2.);
    modelWeightSum_->GetXaxis()->SetBinLabel(1, "NEvents");
    modelWeightSum_->GetXaxis()->SetBinLabel(2, "Weighted");
    
    maxpt_ = fs->make<TH1F>("maxpt", "maxpt", 100, 0, 400);
    maxptWeighted_ = fs->make<TH1F>("maxptWeighted", "maxptWeighted", 100, 0, 400);
    maxeta_ = fs->make<TH1F>("maxeta", "maxeta", 50, -5, 5);
    mass_ = fs->make<TH1F>("ttbarmass", "ttbarmass", 100, 0, 2000);
    massWeighted_ = fs->make<TH1F>("ttbarmassWeighted", "ttbarmassWeighted", 100, 0, 2000);

}


double EventWeightDileptonModelVariation::getTopPtWeight()
{
    double maxpt = std::max(genEvt->top()->pt(), genEvt->topBar()->pt());
//     std::cout << "getTopPtWeight: " << maxpt << " " << genEvt->top()->pt() << " " << genEvt->topBar()->pt() << " " 
//         << weight1x_ << " " << slope_ << "\n";
    double weight = 1+(maxpt-weight1x_)*slope_;
    maxpt_->Fill(maxpt);
    maxptWeighted_->Fill(maxpt, weight);
    return weight;
}

double EventWeightDileptonModelVariation::getTopEtaWeight()
{
    double maxeta = std::max(std::abs(genEvt->top()->eta()), std::abs(genEvt->topBar()->eta()));
    maxeta_->Fill(maxeta);
    return (1+(maxeta-weight1x_)*slope_);
}

double EventWeightDileptonModelVariation::getTopMassWeight()
{
    double mass = (genEvt->top()->p4() + genEvt->topBar()->p4()).M();
    return (1+(mass-weight1x_)*slope_);
}

double EventWeightDileptonModelVariation::getTopMassWeightLandau()
{
    double mass = (genEvt->top()->p4() + genEvt->topBar()->p4()).M();
    mass_->Fill(mass);
    double weight =
        (TMath::Landau(mass, landauMPV_ + landauMoveX_, landauSigma_) / 
         TMath::Landau(mass, landauMPV_, landauSigma_));
    massWeighted_->Fill(mass, weight);
    return weight;
    
}

void EventWeightDileptonModelVariation::produce(edm::Event& evt, const edm::EventSetup& setup)
{
    std::auto_ptr<double> eventWeight(new double);
    evt.getByLabel(ttGenEvent_, genEvt );

    if (genEvt.failedToGet() || weightVariable_.empty()) *eventWeight = 1;
    else if (!weightVariable_.compare("toppt")) *eventWeight = getTopPtWeight();
    else if (!weightVariable_.compare("topeta")) *eventWeight = getTopEtaWeight();
    else if (!weightVariable_.compare("ttbarmass")) *eventWeight = getTopMassWeight();
    else if (!weightVariable_.compare("ttbarmasslandau")) *eventWeight = getTopMassWeightLandau();
    else edm::LogError("EventWeightDilepton") << "don't know which variation to take, " 
        << weightVariable_ << "is invalid!" << std::endl;
    
    *eventWeight = std::min(std::max(*eventWeight, minWeight_), maxWeight_);
    modelWeightSum_->Fill(0);
    modelWeightSum_->Fill(1, *eventWeight);
//     std::cout << "putting " << *eventWeight << "\n";
    evt.put(eventWeight);
}

DEFINE_FWK_MODULE( EventWeightDileptonModelVariation );
