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
  filename_             ( cfg.getParameter<edm::FileInPath>  ("filename"  ) ),
  additionalFactor_     ( cfg.getParameter<double>   ("additionalFactor"  ) ),
  additionalFactorErr_  ( cfg.getParameter<double>   ("additionalFactorErr"  ) ),
  meanTriggerEffSF_     ( cfg.getParameter<double>   ("meanTriggerEffSF"  ) ),
  shapeDistortionFactor_( cfg.getParameter<double>   ("shapeDistortionFactor"  ) ),
  shapeVarEtaThreshold_ ( cfg.getParameter<double>      ("shapeVarEtaThreshold"  ) ),
  shapeVarPtThreshold_  ( cfg.getParameter<double>      ("shapeVarPtThreshold"  ) )
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
  if(filename_.location()){
    file_ = new TFile((TString)filename_.fullPath());
    if(!(file_->IsZombie())){
      if(verbose_>=1) std::cout<<filename_.fullPath()<<" opened"<<std::endl;
      /// get histo/graph
      if     (sysVar_=="PUup")   effHists_["effSFEta"]       = (TGraph*) file_->Get("tapAllSFetaPUup")->Clone();
      else if(sysVar_=="PUdown") effHists_["effSFEta"]       = (TGraph*) file_->Get("tapAllSFetaPUdown")->Clone();
      else                       effHists_["effSFEta"]       = (TGraph*) file_->Get("tapAllSFeta")->Clone();
      if(verbose_>=1) std::cout<<"histo found"<<std::endl;
      
      /// read values from TGraphAsymmErrors
      N      = effHists_["effSFEta"]->GetN();
      xVec   = effHists_["effSFEta"]->GetX();
      yVec   = effHists_["effSFEta"]->GetY();
      xErrHi = effHists_["effSFEta"]->GetEXhigh();
      xErrLo = effHists_["effSFEta"]->GetEXlow();
      yErrHi = effHists_["effSFEta"]->GetEYhigh();
      yErrLo = effHists_["effSFEta"]->GetEYlow();
     
    }
    else{
      std::cout<<filename_.fullPath()<<" not found!!!!! Efficiencies cannot be taken from this file!!! Default taken!"<<std::endl;
      filename_ = edm::FileInPath();
    }
  }
}

EffSFMuonEventWeight::~EffSFMuonEventWeight()
{
  if(filename_.location()) {if(!(file_->IsZombie())) file_->Close();}
}

void
EffSFMuonEventWeight::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // get probe collection
  edm::Handle<edm::View<reco::Candidate> > particles;
  evt.getByLabel(particles_, particles);

  double pt, eta;
  double result     =  1.0;
  double errorUp    = -1.0;
  double errorDown  = -1.0;
  int numPart       =  0;
  
  for(edm::View<reco::Candidate>::const_iterator part=particles->begin(); part!=particles->end(); ++part){

    pt  = part->pt();
    eta = part->eta();
    numPart++;
    /// eta dependent eff. SF
    if(filename_.location() && sysVar_!="flatTriggerSF") {
//       TGraph* his = effHists_.find("effSFEta")->second;
      
      /// search for the index of the point associated with eta value
      int iPoint=0;
      if(eta <xVec[0]-xErrLo[iPoint]) {
	std::cout<<"WARNING!!! Eta point out of range of eff. SF graph. Value set to next possible point" <<std::endl;
	iPoint=0;
      }
      else if(eta >=xVec[N-1]+xErrLo[iPoint]) {
	std::cout<<"WARNING!!! Eta point out of range of eff. SF graph. Value set to next possible point" <<std::endl;
	iPoint=N-1;
      }
      else {
	for(iPoint=0; iPoint <N; iPoint++) {
	  if(verbose_>=1) std::cout<<"iPoint"<<iPoint << "; xVec[iPoint]="<<xVec[iPoint] << "; xErrLo[iPoint]=" << xErrLo[iPoint] << "; yVec[iPoint]="<<yVec[iPoint] << "; yErrLo[iPoint]=" << yErrLo[iPoint] << "; eta="<<eta<< std::endl;
	  if(eta <xVec[iPoint]+xErrLo[iPoint]) break;
	}
      }
      result    = yVec[iPoint];
      errorUp   = yErrHi[iPoint];
      errorDown = yErrLo[iPoint];
      
      // in case of histo
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
    }
    else{
        result = meanTriggerEffSF_;
    }
    /// systematic variations for trigger eff. SF (normalisation and shape uncertainties)
    if     (sysVar_ == "combinedEffSFNormUpStat")   result += errorUp;
    else if(sysVar_ == "combinedEffSFNormDownStat") result -= errorDown;
    else if(sysVar_ == "combinedEffSFShapeUpEta"){
      /// different versions of shape distortion (via difference between result in respective bin or via certain threshold)
      if      (shapeDistortionFactor_>0)   result += shapeDistortionFactor_ * (result - meanTriggerEffSF_);
      else if (eta <shapeVarEtaThreshold_) result -= 0.01; 
      else                                 result += 0.01;
    }
    else if(sysVar_ == "combinedEffSFShapeDownEta"){
      /// different versions of shape distortion (via difference between result in respective bin or via certain threshold)
      if      (shapeDistortionFactor_>0)   result -= shapeDistortionFactor_ * (result - meanTriggerEffSF_);
      else if (eta <shapeVarEtaThreshold_) result += 0.01; 
      else                                 result -= 0.01;
    }
    else if(sysVar_ == "combinedEffSFShapeUpPt")  { 
      if(pt<shapeVarPtThreshold_) result += 0.01; 
      else                        result -= 0.01;
    }
    else if(sysVar_ == "combinedEffSFShapeDownPt"){ 
      if(pt<shapeVarPtThreshold_) result -= 0.01; 
      else                        result += 0.01;
    }
    /// additional factor (e.g. if lepton selection and trigger eff. SF is quoted separately)
    result *= additionalFactor_;
    /// systematic variations for eff. SF
    if     (sysVar_ == "combinedEffSFNormUpSys")   result *= (1+additionalFactorErr_);
    else if(sysVar_ == "combinedEffSFNormDownSys") result *= (1-additionalFactorErr_);
    if(verbose_>=1) std::cout<<numPart<< ": pt=" <<pt<< "; eta=" <<eta<< "; SF= "<<result<<"; errorUp="<< errorUp <<"; errorDown="<< errorDown<<std::endl;
    hists_.find("muonEffSF" )->second->Fill( result );
  // break in order to have only one event weight (the one of the leading part.) in case of more part. in the event
  break;
  } 
  // FIXME: 8TeV SF to be derived, use 1.0 for the meanwhile
  result=1.0;
  std::auto_ptr<double> SFEventWeight(new double);
  *SFEventWeight = result;    
  evt.put(SFEventWeight);
}

// executed at the end after looping over all events
void 
    EffSFMuonEventWeight::endJob() 
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( EffSFMuonEventWeight );

