#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EffSFElectronEventWeight.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


EffSFElectronEventWeight::EffSFElectronEventWeight(const edm::ParameterSet& cfg):
  particles_            ( cfg.getParameter<edm::InputTag>    ("particles"   ) ),
  sysVar_               ( cfg.getParameter<std::string>      ("sysVar"  ) ),
  verbose_              ( cfg.getParameter<int>              ("verbose" ) ),
  //filename_             ( cfg.getParameter<std::string>      ("filename"  ) ),
  additionalFactor_     ( cfg.getParameter<double>   ("additionalFactor"  ) ),
  additionalFactorErr_  ( cfg.getParameter<double>   ("additionalFactorErr"  ) ),
  meanTriggerEffSF_     ( cfg.getParameter<double>   ("meanTriggerEffSF"  ) ),
  meanTriggerEffSFErr_  ( cfg.getParameter<double>   ("meanTriggerEffSFErr"  ) ),
  shapeDistortionFactor_( cfg.getParameter<double>   ("shapeDistortionFactor"  ) )
{
  produces<double>();
  
  factorNonIso_ = 0.31; // fracton of luminosity of Ele25TriJet range = lumiNonIso/lumiTotal
  factorIso_ = 0.69; // fraction of luminosity of IsoEle25TriJet range = lumiIso/lumiTotal
  
  // laod TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  /// booking of histogram for b tag eff SF
  hists_["electronEffSF"]     = fs->make<TH1F>( "electronEffSF", "electronEffSF", 200, 0.5, 1.5 );
  
  /// getting efficiency histos from input files
//   if(filename_!=""){
//     file_ = new TFile((TString)filename_);
//     if(!(file_->IsZombie())){
//       if(verbose_>=1) std::cout<<filename_<<" opened"<<std::endl;
//       // use ->FindObject because ComparisonSFtapTrigger_3 is a TPad object
//       effHists_["effSFEta"]       = (TH1F*) file_->Get("ComparisonSFtapTrigger_3")->FindObject("effSFEta_1")->Clone();
//       if(verbose_>=1) std::cout<<"histo found"<<std::endl;
//      
//     }
//     else{
//       std::cout<<filename_<<" not found!!!!! Efficiencies cannot be taken from this file!!! Default taken!"<<std::endl;
//       filename_ = "";
//     }
//   }
}

EffSFElectronEventWeight::~EffSFElectronEventWeight()
{
  //if(filename_!="") {if(!(file_->IsZombie())) file_->Close();}
}

void
EffSFElectronEventWeight::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // get probe collection
  edm::Handle<edm::View<pat::Electron> > particles;
  evt.getByLabel(particles_, particles);

  double pt, eta, relIso;
  double result=-1;
  double error=-1;
  int numPart=0;
  

  
  for(edm::View<pat::Electron>::const_iterator part=particles->begin(); part!=particles->end(); ++part){

    pt  = part->pt();
    eta = part->eta();
    relIso = (part->chargedHadronIso() + part->neutralHadronIso() + part->photonIso()) / part->et();
    numPart++;
    /// eta dependent trigger eff. SF
//     if(filename_!="" && sysVar_!="flatTriggerSF") {
//       TH1F* his = effHists_.find("effSFEta")->second;
//       if(eta >= his->GetBinLowEdge(his->GetNbinsX()+1)) {
// 	result= his->GetBinContent(his->GetNbinsX());
// 	error = his->GetBinError(his->GetNbinsX());
//       }
//       else if(eta < his->GetBinLowEdge(1)){
// 	result= his->GetBinContent(1);
// 	error = his->GetBinError(1);
//       }
//       else{
// 	result = his->GetBinContent( his->FindBin(eta) );
// 	error  = his->GetBinError( his->FindBin(eta) );
//       }
//     }
//     else{
//         result = meanTriggerEffSF_;
//     }
    
    /// scale trigger efficiency to take difference between non-isolated and
    /// isolated part for later run ranges (IsoEle25TriJet) into account;
    /// parameterisation obtained by comparing Ele25TriJet (control trigger) and IsoEle25TriJet
    result = meanTriggerEffSF_ * (factorNonIso_ + factorIso_ * (0.997+0.05*relIso-3.*relIso*relIso));
    
    /// systematic variations for trigger eff. SF (normalisation and shape uncertainties)
    if     (sysVar_ == "triggerEffSFNormUp")   result += meanTriggerEffSFErr_;
    else if(sysVar_ == "triggerEffSFNormDown") result -= meanTriggerEffSFErr_;
    else if(sysVar_ == "triggerEffSFShapeUpPt")  { 
        if(pt<40) result += shapeDistortionFactor_ * meanTriggerEffSFErr_; 
	else      result -= shapeDistortionFactor_ * meanTriggerEffSFErr_;
    }
    else if(sysVar_ == "triggerEffSFShapeDownPt"){ 
        if(pt<40) result -= shapeDistortionFactor_ * meanTriggerEffSFErr_; 
        else      result += shapeDistortionFactor_ * meanTriggerEffSFErr_;
    }
    else if(sysVar_ == "triggerEffSFShapeUpEta")  { 
      if(std::abs(eta)<1.479) result += shapeDistortionFactor_ * meanTriggerEffSFErr_; 
      else                    result -= shapeDistortionFactor_ * meanTriggerEffSFErr_;
    }
    else if(sysVar_ == "triggerEffSFShapeDownEta"){ 
      if(std::abs(eta)<1.479) result -= shapeDistortionFactor_ * meanTriggerEffSFErr_; 
      else                    result += shapeDistortionFactor_ * meanTriggerEffSFErr_;
    }
    
    /// additional factor as lepton selection eff. SF
    result *= additionalFactor_;
    /// systematic variations for lepton selection eff. SF
    if     (sysVar_ == "selectionEffSFNormUp")   result *= (1+additionalFactorErr_);
    else if(sysVar_ == "selectionEffSFNormDown") result *= (1-additionalFactorErr_);
    
    if(verbose_>=1) std::cout<<numPart<< ": pt=" <<pt<< "; eta=" <<eta<< "; SF= "<<result<<"; error="<< error <<std::endl;
    hists_.find("electronEffSF" )->second->Fill( result );

  std::auto_ptr<double> SFEventWeight(new double);
  *SFEventWeight = result;    
  evt.put(SFEventWeight);
  // break in order to have only one event weight (the one of the leading part.) in case of more part. in the event
  break;
  }
}

// executed at the end after looping over all events
void 
    EffSFElectronEventWeight::endJob() 
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( EffSFElectronEventWeight );

