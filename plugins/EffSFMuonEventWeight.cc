#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EffSFMuonEventWeight.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


EffSFMuonEventWeight::EffSFMuonEventWeight(const edm::ParameterSet& cfg):
  particles_            ( cfg.getParameter<edm::InputTag>    ("particles"   ) ),
  sysVar_               ( cfg.getParameter<std::string>      ("sysVar"  ) ),
  verbose_              ( cfg.getParameter<int>              ("verbose" ) ),
  filename_             ( cfg.getParameter<std::string>      ("filename"  ) ),
  additionalFactor_     ( cfg.getParameter<double>   ("additionalFactor"  ) ),
  additionalFactorErr_  ( cfg.getParameter<double>   ("additionalFactorErr"  ) ),
  meanTriggerEffSF_     ( cfg.getParameter<double>   ("meanTriggerEffSF"  ) ),
  shapeDistortionFactor_( cfg.getParameter<double>   ("shapeDistortionFactor"  ) )
{
  produces<double>();
  
  // laod TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  /// booking of histogram for b tag eff SF
  hists_["muonEffSF"]     = fs->make<TH1F>( "muonEffSF", "muonEffSF", 200, 0.5, 1.5 );
  
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
    /// eta dependent trigger eff. SF
    if(filename_!="" && sysVar_!="flatTriggerSF") {
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
        result = meanTriggerEffSF_;
    }
    /// systematic variations for trigger eff. SF (normalisation and shape uncertainties)
    if     (sysVar_ == "triggerEffSFNormUp")   result += error;
    else if(sysVar_ == "triggerEffSFNormDown") result -= error;
    else if(sysVar_ == "triggerEffSFShapeUpEta")  result += shapeDistortionFactor_ * (result - meanTriggerEffSF_);
    else if(sysVar_ == "triggerEffSFShapeDownEta")result -= shapeDistortionFactor_ * (result - meanTriggerEffSF_);
    else if(sysVar_ == "triggerEffSFShapeUpPt")  { 
      if(pt<40) result += 0.01; 
      else      result -= 0.01;
    }
    else if(sysVar_ == "triggerEffSFShapeDownPt"){ 
      if(pt<40) result -= 0.01; 
      else      result += 0.01;
    }
    
    /// additional factor as lepton selection eff. SF
    result *= additionalFactor_;
    /// systematic variations for lepton selection eff. SF
    if     (sysVar_ == "selectionEffSFNormUp")   result *= (1+additionalFactorErr_);
    else if(sysVar_ == "selectionEffSFNormDown") result *= (1-additionalFactorErr_);
    
    if(verbose_>=1) std::cout<<numPart<< ": pt=" <<pt<< "; eta=" <<eta<< "; SF= "<<result<<"; error="<< error <<std::endl;
    hists_.find("muonEffSF" )->second->Fill( result );

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

