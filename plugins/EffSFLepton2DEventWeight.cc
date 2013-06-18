#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/EffSFLepton2DEventWeight.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


EffSFLepton2DEventWeight::EffSFLepton2DEventWeight(const edm::ParameterSet& cfg):
  particles_            ( cfg.getParameter<edm::InputTag>    ("particles") ),
  sysVar_               ( cfg.getParameter<std::string>      ("sysVar"   ) ),
  verbose_              ( cfg.getParameter<int>              ("verbose"  ) ),
  filename_             ( cfg.getParameter<edm::FileInPath>  ("filename" ) ),
  additionalFactor_     ( cfg.getParameter<double>      ("additionalFactor"     ) ),
  additionalFactorErr_  ( cfg.getParameter<double>      ("additionalFactorErr"  ) ),
  shapeDistortionFactor_( cfg.getParameter<double>      ("shapeDistortionFactor") ),
  shapeVarEtaThreshold_ ( cfg.getParameter<double>      ("shapeVarEtaThreshold" ) ),
  shapeVarPtThreshold_  ( cfg.getParameter<double>      ("shapeVarPtThreshold"  ) )
{
  produces<double>();
  
  // debugging control outpu
  if(verbose_>=2){
     std::cout << "executing EffSFLepton2DEventWeight with the following options"<< std::endl;   
    std::cout << "particles_            : " << particles_             << std::endl;
    std::cout << "sysVar_               : " << sysVar_                << std::endl;
    std::cout << "verbose_              : " << verbose_               << std::endl;
    std::cout << "filename_             : " << filename_              << std::endl;
    std::cout << "additionalFactor_     : " << additionalFactor_      << std::endl;
    std::cout << "additionalFactorErr_  : " << additionalFactorErr_   << std::endl;
    std::cout << "shapeDistortionFactor_: " << shapeDistortionFactor_ << std::endl;
    std::cout << "shapeVarEtaThreshold_ : " << shapeVarEtaThreshold_  << std::endl;
    std::cout << "shapeVarPtThreshold_  : " << shapeVarPtThreshold_   << std::endl;
  }

  // load TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  /// booking of histogram for muon ID+trigger eff SF
  hists_["lepEffSF"] = fs->make<TH1F>( "lepEffSF", "lepEffSF", 40, 0., 2.);
  
  /// get SF 2D histo from specified input file
  if(filename_.location()){
    file_ = new TFile((TString)filename_.fullPath());
    if(!(file_->IsZombie())){
      if(verbose_>=1) std::cout<<filename_.fullPath()<<" opened"<<std::endl;
      /// get pt and eta dependend 2D histo
      if     (sysVar_=="PUup")   hists2D_["effSFPtEta"]       = (TH2F*) file_->Get("tapAllSFetaPUup"  )->Clone();
      else if(sysVar_=="PUdown") hists2D_["effSFPtEta"]       = (TH2F*) file_->Get("tapAllSFetaPUdown")->Clone();
      else                       hists2D_["effSFPtEta"]       = (TH2F*) file_->Get("tapAllSFeta"      )->Clone();
      if(verbose_>=1) std::cout<<"histo found"<<std::endl;
      /// considered range for 2D SFhisto
      ptmin =hists2D_["effSFPtEta"]->GetXaxis()->GetXmin();
      ptmax =hists2D_["effSFPtEta"]->GetXaxis()->GetXmax();
      etamin=hists2D_["effSFPtEta"]->GetYaxis()->GetXmin();
      etamax=hists2D_["effSFPtEta"]->GetYaxis()->GetXmax();
      // set range per hand
      ptmin =  33.1;
      ptmax =1199.9;
      etamin= -2.09;
      etamax=  2.09;
      if(verbose_>=1){
	std::cout << "x range: " << ptmin  << ".." << ptmax  << std::endl;
	std::cout << "y range: " << etamin << ".." << etamax << std::endl;
      }
    }
    else{
      std::cout<<filename_.fullPath()<<" not found!!!!! Efficiencies cannot be taken from this file!!! Default taken"<<std::endl;
      filename_ = edm::FileInPath();
    }
  }
}

EffSFLepton2DEventWeight::~EffSFLepton2DEventWeight()
{
  if(filename_.location()) {if(!(file_->IsZombie())) file_->Close();}
}

void
EffSFLepton2DEventWeight::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  // get object collection
  // (designed for lepton or electron collection)
  edm::Handle<edm::View<reco::Candidate> > particles;
  evt.getByLabel(particles_, particles);

  // variables used
  double pt, eta;
  double result     =  1.0;
  double errorUp    = -1.0;
  double errorDown  = -1.0;
  int numPart       =  0;
  if(numPart>1){
    std::cout << "WARNING in EffSFLepton2DEventWeight: more than one";
    std::cout << " object in input collection " << particles_;
    std::cout << " found! the module was designed for lepton eff with";
    std::cout << " only 1 input object. SF for the last entry in the";
    std::cout << " collection will be used!" << std::endl;
      }
  // loop object collection
  for(edm::View<reco::Candidate>::const_iterator part=particles->begin(); part!=particles->end(); ++part){

    // get kinematics
    pt  = part->pt();
    eta = part->eta();
    // count number of particles
    numPart++;
    // debug output
    if(verbose_>=2){
      std::cout << "particle " << numPart << " in particles_ collection: ";
      std::cout << "pt=" << pt << ", eta=" << eta << std::endl;
    }
    /// pt and eta dependent eff. SF
    if(filename_.location() && sysVar_!="FlatEffSF") {
      /// search for corresponding pt/eta bin of this particle
      int binPt =-1;
      int binEta=-1;
      if(     pt<ptmin  ) pt=ptmin;
      else if(pt>ptmax  ) pt=ptmax;
      if(     eta<etamin) eta=etamin;
      else if(eta>etamax) eta=etamax;
      binPt = hists2D_["effSFPtEta"]->GetXaxis()->FindBin(pt );
      binEta= hists2D_["effSFPtEta"]->GetYaxis()->FindBin(eta);
      // get SF and errors for this 
      if(binPt==-1||binEta==-1){
	std::cout << "ERROR in EffSFLepton2DEventWeight: can not identify bin in 2D effSF histo for ";
	std::cout << "pt=" << pt << "& eta=" << eta << std::endl;
	std::cout << "will use SF 1.+/-0." << std::cout;
	result    =1.;
	errorUp   =0.;
	errorDown =0.;
      }
      else{
	result    = hists2D_["effSFPtEta"]->GetBinContent(binPt, binEta);
	errorUp   = hists2D_["effSFPtEta"]->GetBinError  (binPt, binEta);
	errorDown = errorUp; // asymmetric errors not possible with 2D histos
      }
      if(verbose_>1) std::cout << "bin(pt,eta)=("<< binPt << "," << binEta << ")" << std::endl;
    }
    else{// flat SF
      result = 1.0;
    }
    // debug output
    if(verbose_>=1){
      std::cout << "loaded values from 2D histo" << std::endl;
      std::cout << "result   : " << result    << std::endl;
      std::cout << "errorUp  : " << errorUp   << std::endl;
      std::cout << "errorDown: " << errorDown << std::endl;
    }
    /// systematic variations for trigger eff. SF (normalisation and shape uncertainties)
    if     (sysVar_ == "combinedEffSFNormUpStat")   result += errorUp;
    else if(sysVar_ == "combinedEffSFNormDownStat") result -= errorDown;
    else if(sysVar_ == "combinedEffSFShapeUpEta"){
      /// different versions of shape distortion (via difference between result in respective bin or via certain threshold)
      if      (shapeDistortionFactor_>0)   result += shapeDistortionFactor_ * (result - 1.);
      else if (std::abs(eta) <shapeVarEtaThreshold_) result -= errorDown; 
      else                                 result += errorUp;
    }
    else if(sysVar_ == "combinedEffSFShapeDownEta"){
      /// different versions of shape distortion (via difference between result in respective bin or via certain threshold)
      if      (shapeDistortionFactor_>0)   result -= shapeDistortionFactor_ * (result - 1.);
      else if (std::abs(eta) <shapeVarEtaThreshold_) result += errorUp; 
      else                                 result -= errorDown;
    }
    else if(sysVar_ == "combinedEffSFShapeUpPt")  { 
      if(pt<shapeVarPtThreshold_) result += errorUp; 
      else                        result -= errorDown;
    }
    else if(sysVar_ == "combinedEffSFShapeDownPt"){ 
      if(pt<shapeVarPtThreshold_) result -= errorDown; 
      else                        result += errorUp;
    }
    /// additional factor (e.g. if lepton selection and trigger eff. SF is quoted separately
    ///                         or flat SF is used                                          )
    result *= additionalFactor_;
    /// systematic variations for eff. SF
    if     (sysVar_ == "combinedEffSFNormUpSys"  ) result *= (1+additionalFactorErr_);
    else if(sysVar_ == "combinedEffSFNormDownSys") result *= (1-additionalFactorErr_);
    if(verbose_>=1) std::cout<<numPart<< ": pt=" <<pt<< "; eta=" <<eta<< "; SF= "<<result<<"; errorUp="<< errorUp <<"; errorDown="<< errorDown<<std::endl;
    hists_.find("lepEffSF" )->second->Fill(result);
  // break in order to have only one event weight (the one of the leading part.) in case of more part. in the event
  break;
  }
  std::auto_ptr<double> SFEventWeight(new double);
  *SFEventWeight = result;    
  evt.put(SFEventWeight);
}

// executed at the end after looping over all events
void 
    EffSFLepton2DEventWeight::endJob() 
{
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( EffSFLepton2DEventWeight );

