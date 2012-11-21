#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EffSFElectronEventWeight.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


EffSFElectronEventWeight::EffSFElectronEventWeight(const edm::ParameterSet& cfg):
  electrons_               ( cfg.getParameter<edm::InputTag>    ("electrons"   ) ),
  jets_                    ( cfg.getParameter<edm::InputTag>    ("jets"   ) ),
  sysVar_                  ( cfg.getParameter<std::string>      ("sysVar"  ) ),
  shapeVarPtEleThreshold_  ( cfg.getParameter<double>      ("shapeVarPtEleThreshold"  ) ),
  shapeVarEtaEleThreshold_ ( cfg.getParameter<double>      ("shapeVarEtaEleThreshold"  ) ),
  verbose_                 ( cfg.getParameter<int>              ("verbose" ) ),
  filenameJetLeg_          ( cfg.getParameter<edm::FileInPath>  ("filenameJetLeg"  ) ),
  additionalFactor_        ( cfg.getParameter<double>   ("additionalFactor"  ) ),
  additionalFactorErr_     ( cfg.getParameter<double>   ("additionalFactorErr"  ) ),
  meanTriggerEffSF_        ( cfg.getParameter<double>   ("meanTriggerEffSF"  ) ),
  meanTriggerEffSFErr_     ( cfg.getParameter<double>   ("meanTriggerEffSFErr"  ) ),
  shapeDistortionErr_      ( cfg.getParameter<double>   ("shapeDistortionErr"  ) ),
  jetTriggerEffsSFNormSysErr_ ( cfg.getParameter<double>   ("jetTriggerEffsSFNormSysErr"  ) ),
  jetTriggerEffsSFShapeSysErr_( cfg.getParameter<double>   ("jetTriggerEffsSFShapeSysErr"  ) )
  
{
  produces<double>();
  
  factorNonIso_ = 0.069; // fracton of luminosity of Ele25TriJet range = lumiNonIso/lumiTotal
  factorIso_    = 0.931; // fraction of luminosity of IsoEle25TriJet range = lumiIso/lumiTotal
  
  // laod TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  /// booking of histogram for b tag eff SF
  hists_["electronEffSFEleLeg"]     = fs->make<TH1F>( "electronEffSFEleLeg", "electronEffSFEleLeg", 200, 0.5, 1.5 );
  hists_["electronEffSFJetLeg"]     = fs->make<TH1F>( "electronEffSFJetLeg", "electronEffSFJetLeg", 200, 0.5, 1.5 );
  hists_["electronEffSFTotal"]      = fs->make<TH1F>( "electronEffSFTotal", "electronEffSFTotal", 200, 0.5, 1.5 );
  
  /// getting efficiency histos from input files
  if(filenameJetLeg_.location()){
    file_ = new TFile((TString)filenameJetLeg_.fullPath());
    if(!(file_->IsZombie())){
      if(verbose_>=1) std::cout<<filenameJetLeg_.fullPath()<<" opened"<<std::endl;
      /// get histo/graph
      effHists_["effSFetaEleJetLeg"]       = (TGraph*) file_->Get("JetLegTriggerEfficiencyIsoLepTriJetJetMult4")->Clone();

//       if     (sysVar_=="PUup")   effHists_["effSFetaEle"]       = (TGraph*) file_->Get("tapAllSFetaElePUup")->Clone();
//       else if(sysVar_=="PUdown") effHists_["effSFetaEle"]       = (TGraph*) file_->Get("tapAllSFetaElePUdown")->Clone();
//       else                       effHists_["effSFetaEle"]       = (TGraph*) file_->Get("tapAllSFetaEle")->Clone();
      if(verbose_>=1) std::cout<<"histo found"<<std::endl;
      
      /// read values from TGraphAsymmerrorSFeles
      N      = effHists_["effSFetaEleJetLeg"]->GetN();
      xVec   = effHists_["effSFetaEleJetLeg"]->GetX();
      yVec   = effHists_["effSFetaEleJetLeg"]->GetY();
      xErrHi = effHists_["effSFetaEleJetLeg"]->GetEXhigh();
      xErrLo = effHists_["effSFetaEleJetLeg"]->GetEXlow();
      yErrHi = effHists_["effSFetaEleJetLeg"]->GetEYhigh();
      yErrLo = effHists_["effSFetaEleJetLeg"]->GetEYlow();
     
    }
    else{
      std::cout<<filenameJetLeg_.fullPath()<<" not found!!!!! Efficiencies cannot be taken from this file!!! Default taken!"<<std::endl;
      filenameJetLeg_ = edm::FileInPath();
    }
  }
  else{
    std::cout<<filenameJetLeg_<<" not found!!!!! Efficiencies cannot be taken from this file!!! Default taken!"<<std::endl;
  }
}

EffSFElectronEventWeight::~EffSFElectronEventWeight()
{
  //if(filenameJetLeg_!="") {if(!(file_->IsZombie())) file_->Close();}
}

void
EffSFElectronEventWeight::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // get electron collection
  edm::Handle<edm::View<pat::Electron> > electrons;
  evt.getByLabel(electrons_, electrons);
  
  // get jet collection
  edm::Handle<std::vector<pat::Jet> > jets;
  evt.getByLabel(jets_, jets);

  double ptEle, etaEle, relIsoEle;
  double SFele       =  1.0;
  double errorSFele  = -1.0;
  int numPart        =  0;
  
  double jetMult, ptJet4, absEtaJet4;
  double SFjet           =  1.0;
  double errorUpSFjet    = 0.;
  double errorDownSFjet  = 0.;
  
  ///-----------------------------------
  /// Electron SF
  ///-----------------------------------
  for(edm::View<pat::Electron>::const_iterator part=electrons->begin(); part!=electrons->end(); ++part){

    ptEle  = part->pt();
    etaEle = part->eta();
    relIsoEle = (part->chargedHadronIso() + part->neutralHadronIso() + part->photonIso()) / part->et();
    numPart++;
    
    /// scale trigger efficiency to take difference between non-isolated for early run ranges and
    /// isolated part for later run ranges (IsoEle25TriJet) into account;
    /// parameterisation obtained by comparing Ele25TriJet (control trigger) and IsoEle25TriJet
    /// if nonIsoEle is MC trigger -> correct isoEle
    //SFele = meanTriggerEffSF_ * (factorNonIso_ + factorIso_ * (0.997+0.05*relIsoEle-3.*relIsoEle*relIsoEle));
    /// if IsoEle is MC trigger -> correct nonIsoEle
    SFele = meanTriggerEffSF_ * (factorIso_ + factorNonIso_ / (0.997+0.05*relIsoEle-3.*relIsoEle*relIsoEle));
    
    /// optEleional additional factor
    SFele *= additionalFactor_;
    
    /// systematic variations for combined trigger and selection eff. SF (normalisation and shape uncertainties)
    if     (sysVar_ == "" || sysVar_ == "noSys") ; // central value -> already set above
    else if(sysVar_ == "combinedEffSFNormUpSys")    SFele *= (1+additionalFactorErr_);
    else if(sysVar_ == "combinedEffSFNormDownSys")  SFele *= (1-additionalFactorErr_);
    else if(sysVar_ == "combinedEffSFNormUpStat")   SFele += meanTriggerEffSFErr_;
    else if(sysVar_ == "combinedEffSFNormDownStat") SFele -= meanTriggerEffSFErr_;
    else if(sysVar_ == "combinedEffSFShapeUpPt")  { 
      if(ptEle<shapeVarPtEleThreshold_) SFele += shapeDistortionErr_; 
      else                        SFele -= shapeDistortionErr_;
    }
    else if(sysVar_ == "combinedEffSFShapeDownPt"){ 
      if(ptEle<shapeVarPtEleThreshold_) SFele -= shapeDistortionErr_; 
      else                        SFele += shapeDistortionErr_;
    }
    else if(sysVar_ == "combinedEffSFShapeUpEta")  { 
      if(std::abs(etaEle)<shapeVarEtaEleThreshold_) SFele += shapeDistortionErr_; 
      else                                    SFele -= shapeDistortionErr_;
    }
    else if(sysVar_ == "combinedEffSFShapeDownEta"){ 
      if(std::abs(etaEle)<shapeVarEtaEleThreshold_) SFele -= shapeDistortionErr_; 
      else                                    SFele += shapeDistortionErr_;
    }
    else if (std::string::npos != sysVar_.find("jetEffSF")) ; // to avoid an exception throw for jetEffSF variations (see below)
    else{
      throw edm::Exception( edm::errors::Configuration,
			    sysVar_+" is no valid systematic variation name! ERROR!" );
    }
    
    if(verbose_>=1) std::cout<<numPart<< ": ptEle=" <<ptEle<< "; etaEle=" <<etaEle<< "; SF= "<<SFele<<"; errorSFele="<< errorSFele <<std::endl;
    hists_.find("electronEffSFEleLeg" )->second->Fill( SFele );

    // break in order to have only one event weight (the one of the leading part.) in case of more part. in the event
    break;
  }
  
  
  ///-----------------------------------
  /// Jet Leg SF
  ///-----------------------------------
  
  std::vector<pat::Jet> jetsVector =  *jets;
  jetMult = jetsVector.size();
  
  if(verbose_>=1)  std::cout<< "jet size: " << jetMult <<std::endl; 
 
  if(verbose_>=1){
    std::cout<< "jet size: " << jetMult <<std::endl; 
    if(jetMult>3){
      std::cout<< "jet 1 pt: "  << jetsVector[0].pt() <<std::endl;
      std::cout<< "jet 2 pt: "  << jetsVector[1].pt() <<std::endl;
      std::cout<< "jet 3 pt: "  << jetsVector[2].pt() <<std::endl;
      std::cout<< "jet 4 pt: "  << jetsVector[3].pt() <<std::endl;
      std::cout<< "jet 4 eta: " << jetsVector[3].eta() <<std::endl;
    }
  }
  
  /// eta dependent eff. SF for jetMult==4 and jetPt <40
  if(filenameJetLeg_.location() && jetMult==4) {
    
    ptJet4     = jetsVector[3].pt();
    absEtaJet4 = TMath::Abs(jetsVector[3].eta());
    
    if(ptJet4 < 40){
      /// search for the index of the point associated with eta value
      int iPoint=0;
      if(absEtaJet4 <xVec[0]-xErrLo[iPoint]) {
	std::cout<<"WARNING!!! |Eta| point out of range of eff. SF graph. Value set to next possible point" <<std::endl;
	iPoint=0;
      }
      else if(absEtaJet4 >=xVec[N-1]+xErrLo[iPoint]) {
	std::cout<<"WARNING!!! |Eta| point out of range of eff. SF graph. Value set to next possible point" <<std::endl;
	iPoint=N-1;
      }
      else {
	for(iPoint=0; iPoint <N; iPoint++) {
	  if(verbose_>=1) std::cout<<"iPoint"<<iPoint << "; xVec[iPoint]="<<xVec[iPoint] << "; xErrLo[iPoint]=" << xErrLo[iPoint] << "; yVec[iPoint]="<<yVec[iPoint] << "; yErrLo[iPoint]=" << yErrLo[iPoint] << "; absEtaJet4="<<absEtaJet4<< std::endl;
	  if(absEtaJet4 <xVec[iPoint]+xErrLo[iPoint]) break;
	}
      }
      SFjet          = yVec[iPoint];
      errorUpSFjet   = yErrHi[iPoint];
      errorDownSFjet = yErrLo[iPoint];
      
      /// systematic variations
      if     (sysVar_ == "" || sysVar_ == "noSys") ; // central value -> already set above
      else if(sysVar_ == "jetEffSFNormUpSys")    SFjet += jetTriggerEffsSFNormSysErr_;
      else if(sysVar_ == "jetEffSFNormDownSys")  SFjet -= jetTriggerEffsSFNormSysErr_;
      else if(sysVar_ == "jetEffSFShapeUpSys"){
	if(absEtaJet4 < 1.4) SFjet += jetTriggerEffsSFShapeSysErr_;
	else                 SFjet -= jetTriggerEffsSFShapeSysErr_;
      }
      else if(sysVar_ == "jetEffSFShapeDownSys"){
	if(absEtaJet4 < 1.4) SFjet -= jetTriggerEffsSFShapeSysErr_;
	else                 SFjet += jetTriggerEffsSFShapeSysErr_;
      }
      else if (std::string::npos != sysVar_.find("combinedEffSF")) ; // to avoid an exception throw for combinedEffSF variations (see above)
      else{
	throw edm::Exception( edm::errors::Configuration,
			      sysVar_+" is no valid systematic variation name! ERROR!" );
      }
    }
  }
  if(verbose_>=1){
    std::cout<< "SFjet: "          << SFjet          <<std::endl;
    std::cout<< "errorUpSFjet: "   << errorUpSFjet   <<std::endl;
    std::cout<< "errorDownSFjet: " << errorDownSFjet <<std::endl;
  }   
  hists_.find("electronEffSFJetLeg" )->second->Fill( SFjet );
  
  
  ///-----------------------------------
  /// Final Weight
  ///-----------------------------------
   
   double SFtotal = SFele * SFjet;
   // FIXME: 8TeV SF to be derived, use 1.0 for the meanwhile
   SFtotal=1.0;
   if(verbose_>=1) std::cout<< "SFtotal: " << SFtotal <<std::endl;
   hists_.find("electronEffSFTotal" )->second->Fill( SFtotal );
  
  std::auto_ptr<double> SFEventWeight(new double);
  *SFEventWeight = SFtotal;
  evt.put(SFEventWeight);
}

// executed at the end after looping over all events
void 
    EffSFElectronEventWeight::endJob() 
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( EffSFElectronEventWeight );

