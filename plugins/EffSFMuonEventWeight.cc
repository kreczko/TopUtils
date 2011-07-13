#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EffSFMuonEventWeight.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


EffSFMuonEventWeight::EffSFMuonEventWeight(const edm::ParameterSet& cfg):
  particles_    ( cfg.getParameter<edm::InputTag>    ("particles"   ) ),
  sysVar_  ( cfg.getParameter<std::string>      ("sysVar"  ) ),
  verbose_ ( cfg.getParameter<int>              ("verbose" ) ),
  filename_( cfg.getParameter<std::string>      ("filename"  ) ),
  additionalFactor_( cfg.getParameter<double>   ("additionalFactor"  ) )
{
  produces<double>();
  
  // laod TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  
  /// getting efficiency histos from input files
  if(filename_!=""){
    file_ = new TFile((TString)filename_);
    if(!(file_->IsZombie())){
      if(verbose_>=1) std::cout<<filename_<<" opened"<<std::endl;
      // use ->FindObject because ComparisonSFtapTrigger_3 is a TPad object
      effHists_["effSFEta"]       = (TH1F*) file_->Get("ComparisonSFtapTrigger_3")->FindObject("effSFEta_1")->Clone();
      if(verbose_>=1) std::cout<<"histo found"<<std::endl;
     
    }
    else{
      std::cout<<filename_<<" not found!!!!! Efficiencies cannot be taken from this file!!! Default taken!"<<std::endl;
      filename_ = "";
    }
  }
}

EffSFMuonEventWeight::~EffSFMuonEventWeight()
{
  if(filename_!="") {if(!(file_->IsZombie())) file_->Close();}
}

void
EffSFMuonEventWeight::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // get probe collection
  edm::Handle<edm::View<reco::Candidate> > particles;
  evt.getByLabel(particles_, particles);

  double pt, eta;
  double result=-1;
  double error=-1;
  int numPart=0;
  
  for(edm::View<reco::Candidate>::const_iterator part=particles->begin(); part!=particles->end(); ++part){

    pt  = part->pt();
    eta = part->eta();
    numPart++;
    if(filename_!="") {
      TH1F* his = effHists_.find("effSFEta")->second;
      if(eta >= his->GetBinLowEdge(his->GetNbinsX()+1)) {
	result= his->GetBinContent(his->GetNbinsX());
	error = his->GetBinError(his->GetNbinsX());
      }
      else if(eta < his->GetBinLowEdge(1)){
	result= his->GetBinContent(1);
	error = his->GetBinError(1);
      }
      else{
	result = his->GetBinContent( his->FindBin(eta) );
	error  = his->GetBinError( his->FindBin(eta) );
      }
    }
    else{
    }
    
    if(sysVar_ == "EffSFUp") result += error;
    if(sysVar_ == "EffSFDown") result -= error;
    result *= additionalFactor_;
    
    if(verbose_>=1) std::cout<<numPart<< ": pt=" <<pt<< "; eta=" <<eta<< "; SF= "<<result<<"; error="<< error <<std::endl;

  std::auto_ptr<double> SFEventWeight(new double);
  *SFEventWeight = result;    
  evt.put(SFEventWeight);
  // break in order to have only one event weight (the one of the leading part.) in case of more part. in the event
  break;
  }
}

// executed at the end after looping over all events
void 
    EffSFMuonEventWeight::endJob() 
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( EffSFMuonEventWeight );

