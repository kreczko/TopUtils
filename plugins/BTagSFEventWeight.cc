#include "FWCore/Utilities/interface/EDMException.h"
#include "TopAnalysis/TopUtils/plugins/BTagSFEventWeight.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/View.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


BTagSFEventWeight::BTagSFEventWeight(const edm::ParameterSet& cfg):
  jets_                   ( cfg.getParameter<edm::InputTag>    ( "jets"   ) ),
  bTagAlgo_               ( cfg.getParameter<std::string>      ("bTagAlgo") ),
  version_                ( cfg.getParameter<std::string>      ("version"  ) ),
  sysVar_                 ( cfg.getParameter<std::string>      ("sysVar"  ) ),
  shapeVarPtThreshold_    ( cfg.getParameter<double>           ("shapeVarPtThreshold"  ) ),
  shapeVarEtaThreshold_   ( cfg.getParameter<double>           ("shapeVarEtaThreshold"  ) ),
  uncertaintySFb_         ( cfg.getParameter<double>           ("uncertaintySFb"  ) ),
  shapeDistortionFactor_  ( cfg.getParameter<double>           ("shapeDistortionFactor"  ) ),
  verbose_                ( cfg.getParameter<int>              ("verbose" ) ),
  filename_               ( cfg.getParameter<edm::FileInPath>  ("filename") )
{
  produces<double>();
  
  // set the edges of the last histo bin
  maxPtDB_     = 240.;
  maxPt11004_  = 670.;
  maxPtMisTag_ = 520.;
  maxEta_      = 2.4;
  
  // laod TFile Service
  edm::Service<TFileService> fs;
  if( !fs ){
    throw edm::Exception( edm::errors::Configuration,
			  "TFile Service is not registered in cfg file" );
  }
  /// booking of histogram for b tag eff SF
  hists_["effBTagEventSF"]     = fs->make<TH1F>( "effBTagEventSF", "effBTagEventSF", 100, 0.5, 1.5 );
  hists_["effBTagEventSFMean"] = fs->make<TH1F>( "effBTagEventSFMean", "effBTagEventSFMean", 1, 0, 1 );
  
  /// getting efficiency histos from input files
  if(filename_.location()){
    file_ = new TFile((TString)filename_.fullPath());
    if(!(file_->IsZombie())){
      if(verbose_>=1) std::cout<<filename_.fullPath()<<" opened"<<std::endl;
      
	effHists_["NumBJetsPtEta"]       = (TH2F*) file_->Get("bTagEff/NumBJetsPtEta")->Clone();
	effHists_["NumBJetsTaggedPtEta"] = (TH2F*) file_->Get("bTagEff/NumBJetsTaggedPtEta")->Clone();
	effHists_["EffBJetsTaggedPtEta"] = (TH2F*) file_->Get("bTagEff/EffBJetsTaggedPtEta")->Clone();
	effHists_["NumCJetsPtEta"]       = (TH2F*) file_->Get("bTagEff/NumCJetsPtEta")->Clone();
	effHists_["NumCJetsTaggedPtEta"] = (TH2F*) file_->Get("bTagEff/NumCJetsTaggedPtEta")->Clone();
	effHists_["EffCJetsTaggedPtEta"] = (TH2F*) file_->Get("bTagEff/EffCJetsTaggedPtEta")->Clone();
	effHists_["NumLJetsPtEta"]       = (TH2F*) file_->Get("bTagEff/NumLJetsPtEta")->Clone();
	effHists_["NumLJetsTaggedPtEta"] = (TH2F*) file_->Get("bTagEff/NumLJetsTaggedPtEta")->Clone();
	effHists_["EffLJetsTaggedPtEta"] = (TH2F*) file_->Get("bTagEff/EffLJetsTaggedPtEta")->Clone();
	
	/// re-calculation of b tag efficiencies as input might be corrupted due to hadd
	if(effHists_.count("NumBJetsPtEta") && effHists_.count("NumBJetsTaggedPtEta") && effHists_.count("EffBJetsTaggedPtEta") &&
	  effHists_.count("NumCJetsPtEta") && effHists_.count("NumCJetsTaggedPtEta") && effHists_.count("EffCJetsTaggedPtEta") &&
	  effHists_.count("NumBJetsPtEta") && effHists_.count("NumBJetsTaggedPtEta") && effHists_.count("EffBJetsTaggedPtEta")) {
	  
	  effHists_.find("EffBJetsTaggedPtEta")->second->Reset();
	  effHists_.find("EffCJetsTaggedPtEta")->second->Reset();
	  effHists_.find("EffLJetsTaggedPtEta")->second->Reset();
	
	  effHists_.find("EffBJetsTaggedPtEta")->second->Divide(effHists_.find("NumBJetsTaggedPtEta")->second, 
	  effHists_.find("NumBJetsPtEta")->second,1,1,"B");
	  effHists_.find("EffCJetsTaggedPtEta")->second->Divide(effHists_.find("NumCJetsTaggedPtEta")->second, 
	  effHists_.find("NumCJetsPtEta")->second,1,1,"B");
	  effHists_.find("EffLJetsTaggedPtEta")->second->Divide(effHists_.find("NumLJetsTaggedPtEta")->second, 
	  effHists_.find("NumLJetsPtEta")->second,1,1,"B");
	 }
	 else{
	   std::cout<<"Eff.Histos not found!!!!! Efficiencies cannot be taken from this file!!! Default taken!"<<std::endl;
	   filename_ = edm::FileInPath();
	 }
    }
    else{
      std::cout<<filename_.fullPath()<<" not found!!!!! Efficiencies cannot be taken from this file!!! Default taken!"<<std::endl;
      filename_ = edm::FileInPath();
    }
  }
  
  /// load map from database
  measureMap_["BTAGBEFFCORR"]=PerformanceResult::BTAGBEFFCORR;
  measureMap_["BTAGBERRCORR"]=PerformanceResult::BTAGBERRCORR;
  measureMap_["BTAGLEFFCORR"]=PerformanceResult::BTAGLEFFCORR;
  measureMap_["BTAGLERRCORR"]=PerformanceResult::BTAGLERRCORR;
}

BTagSFEventWeight::~BTagSFEventWeight()
{
  if(filename_.location()) {if(!(file_->IsZombie())) file_->Close();}
}

void
BTagSFEventWeight::produce(edm::Event& evt, const edm::EventSetup& setup)
{
    //Setup measurement from database
  setup.get<BTagPerformanceRecord>().get( "BTAG"+bTagAlgo_, perfHBTag);
  setup.get<BTagPerformanceRecord>().get( "MISTAG"+bTagAlgo_, perfHMisTag);
  
  edm::Handle<edm::View< pat::Jet > > jets;
  evt.getByLabel(jets_, jets);

  double pt, eta;
  std::vector<double> oneMinusBEffies(0) , oneMinusBEffies_scaled(0);
  std::vector<double> oneMinusBMistags(0), oneMinusBMistags_scaled(0);
  
    for(edm::View<pat::Jet>::const_iterator jet = jets->begin();jet != jets->end(); ++jet) {
      pt  = jet->pt();
      eta = std::abs(jet->eta());
      if(jet->partonFlavour() == 5 || jet->partonFlavour() == -5){
	oneMinusBEffies               .push_back(1.- effBTag(pt, eta));
	oneMinusBEffies_scaled        .push_back(1.- (effBTag(pt, eta) * effBTagSF(pt, eta)));
      }
  
      else if(jet->partonFlavour() == 4 || jet->partonFlavour() == -4){
	oneMinusBMistags               .push_back(1.- effBTagCjet(pt, eta));
	oneMinusBMistags_scaled        .push_back(1.-(effBTagCjet(pt, eta) * effBTagSF(pt, eta)));
      }
  
      else{
	oneMinusBMistags               .push_back(1.- effMisTag(pt, eta));
	oneMinusBMistags_scaled        .push_back(1.-(effMisTag(pt, eta) * effMisTagSF(pt, eta)));
      }
   }
     
   double effBTagEvent_unscaled = effBTagEvent( oneMinusBEffies, oneMinusBMistags );
   double effBTagEvent_scaled   = effBTagEvent( oneMinusBEffies_scaled, oneMinusBMistags_scaled );
   double effBTagEventSF = effBTagEvent_scaled / effBTagEvent_unscaled;
  
   if(verbose_>=1) std::cout<<"effBTagEvent_unscaled= "<<effBTagEvent_unscaled
	                    <<" effBTagEvent_scaled = " <<effBTagEvent_scaled
	                    <<" effBTagEventSF ="       <<effBTagEventSF << std::endl;
   
   hists_.find("effBTagEventSF" )->second->Fill( effBTagEventSF );

  std::auto_ptr<double> bTagSFEventWeight(new double);
  *bTagSFEventWeight = effBTagEventSF;    
  evt.put(bTagSFEventWeight);  
}

//--------------------------------------------------------------------------

/// Default SF values taken from database wrt. PAS BTV-11-001 (pTrel method),
/// or from PAS BTV-11-004
/// Values for eff. from user-defined histo as a function of pt and eta.

double BTagSFEventWeight::effBTagSF11004(double x)
{
  // function from PAS 11-004; x = jetPt
  if(bTagAlgo_=="SSVHEM") return 0.896462*((1.+(0.00957275*x))/(1.+(0.00837582*x)));
  if(bTagAlgo_=="CSVM")   return 0.6981*((1.+(0.414063*x))/(1.+(0.300155*x)));
  if(bTagAlgo_=="JPM")    return 0.90806*((1.+(0.000236997*x))/(1.+(5.49455e-05*x)));
  else { 
    std::cout<< "WARNING!!! b tag SF for "<< bTagAlgo_ <<" not in code!!! CHECK!!!"<<std::endl;
    return 1.; 
  }
}

double BTagSFEventWeight::effBTagSFerr11004(double x)
{
  // function from PAS 11-004; x = jetPt
  // pt binning
  double pt[] = {30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 670};
  // corresponding SFb uncertainties
  double SFb_errorSSVHEM[] = {
    0.0316234,
    0.0310149,
    0.02381,
    0.0223228,
    0.023461,
    0.0202517,
    0.0156249,
    0.0214799,
    0.0399369,
    0.0416666,
    0.0431031,
    0.0663209,
    0.0687731,
    0.0793305 };
  double SFb_errorCSVM[] = {
    0.0295675,
    0.0295095,
    0.0210867,
    0.0219349,
    0.0227033,
    0.0204062,
    0.0185857,
    0.0256242,
    0.0383341,
    0.0409675,
    0.0420284,
    0.0541299,
    0.0578761,
    0.0655432 };
  double SFb_errorJPM[] = {
    0.0352594,
    0.0353008,
    0.0299008,
    0.0276606,
    0.0292312,
    0.0336607,
    0.0284701,
    0.029544,
    0.0358872,
    0.0367869,
    0.0375048,
    0.0597367,
    0.0653152,
    0.074242 };
  /// look for index corresponding to pt
  int iBin = -1;
  // if pt<30 use 12%
  if (x<pt[0]) return 0.12;
  for(int i=0; i<14; i++) {
    if (x>pt[i] && x<pt[i+1]) {
      iBin =i;
      break;
    }
  }
  double factor = 1.;
  if(iBin<0){
    // if pt>670 use SFb(670) and twice its error
    iBin=13;
    factor=2;
  }
  if(bTagAlgo_=="SSVHEM") return factor * SFb_errorSSVHEM[iBin];
  if(bTagAlgo_=="CSVM")   return factor * SFb_errorCSVM[iBin];
  if(bTagAlgo_=="JPM")    return factor * SFb_errorJPM[iBin];
  else { 
    std::cout<< "WARNING!!! b tag SF for "<< bTagAlgo_ <<" not in code!!! CHECK!!!"<<std::endl;
    return 1.; 
  }
}

// b tag eff. from MC as a function of jet pt, eta
double BTagSFEventWeight::effBTag(double jetPt, double jetEta)
{
  double result = -1111.;
  // if histo file exists, take value from there; else return a default value
  if(filename_.location()) {
    TH2F* his = effHists_.find("EffBJetsTaggedPtEta")->second;
    // ensure that pt is in accepted range of BTV DB
    if(jetPt >= maxPt11004_) jetPt = maxPt11004_-1.;
    if(jetEta >= maxEta_) jetEta = maxEta_-0.1;
    result = his->GetBinContent( his->FindBin(jetPt, jetEta) );
  }
  else {result = 0.7; std::cout<< "WARNING!!! b tag eff. is ALWAYS 0.7!!! CHECK!!!"<<std::endl; }
  if(verbose_>=2) std::cout<< "effBTag= "<<result<<std::endl;
  return result;
}

// b tag eff. SF as a function of jet pt, eta
double BTagSFEventWeight::effBTagSF(double jetPt, double jetEta)
{
  double result = -1111., error = -1111.;
  const BtagPerformance & perf = *(perfHBTag.product());
  BinningPointByMap measurePoint;
  if(version_=="DB11-001"){
    /// either take SF from BTV database...
      // ensure that pt is in accepted range
      if(jetPt >= maxPtDB_) jetPt = maxPtDB_-1.;
      if(jetEta >= maxEta_) jetEta = maxEta_-0.1;
      measurePoint.insert(BinningVariables::JetEt, jetPt);
      measurePoint.insert(BinningVariables::JetAbsEta, jetEta);      
      if(perf.isResultOk( measureMap_[ "BTAGBEFFCORR" ], measurePoint))
	result = perf.getResult( measureMap_[ "BTAGBEFFCORR" ], measurePoint);
      else {
	std::cout << "ERROR! B-tag SF could not be taken from DB! b-tag SF is taken as 1!" << std::endl;
	result = 1.;
      }
  }
  else if(version_=="11-004"){
    /// ...or by hand from 11-004 (Moriond recommendation)
    result = effBTagSF11004(jetPt);
  }
  if(uncertaintySFb_<0.){
    if(version_=="DB11-001"){
      /// either take SF from BTV database...
      if(perf.isResultOk( measureMap_[ "BTAGBERRCORR" ], measurePoint))
	error = perf.getResult( measureMap_[ "BTAGBERRCORR" ], measurePoint);
      else {
	std::cout << "ERROR! B-tag SF err could not be taken from DB! b-tag SF err is taken as 0.1!" << std::endl;
	error = 0.1;
      }
    }
    else if(version_=="11-004"){
      /// ...or by hand from 11-004 (Moriond recommendation)
      error = effBTagSFerr11004(jetPt);
    }
  }
  else     error = uncertaintySFb_;
  
  /// different versions of sys. variations
  if(sysVar_ == "bTagSFUp")   result += error;
  else if(sysVar_ == "bTagSFDown") result -= error;
  else if(sysVar_ == "bTagSFShapeUpPt"){
    if(jetPt<shapeVarPtThreshold_) result += (shapeDistortionFactor_*error);
    else                           result -= (shapeDistortionFactor_*error);
  }
  else if(sysVar_ == "bTagSFShapeDownPt"){
    if(jetPt<shapeVarPtThreshold_) result -= (shapeDistortionFactor_*error);
    else                           result += (shapeDistortionFactor_*error);
  }
  else if(sysVar_ == "bTagSFShapeUpEta"){
    if(fabs(jetEta)<shapeVarEtaThreshold_) result += (shapeDistortionFactor_*error);
    else                                   result -= (shapeDistortionFactor_*error);
  }
  else if(sysVar_ == "bTagSFShapeDownEta"){
    if(fabs(jetEta)<shapeVarEtaThreshold_) result -= (shapeDistortionFactor_*error);
    else                                   result += (shapeDistortionFactor_*error);
  }
  if(verbose_>=2) std::cout<< "effBTagSF= "<<result<<" +/- "<<error<< "------ shapeDistortionFactor_=" << shapeDistortionFactor_ << "------ shapeDistortionFactor_*error=" << shapeDistortionFactor_*error <<std::endl;
  return result;
}

// b tag eff. from MC for c jets as a function of jet pt, eta;
// as first step: take average of b and mis eff.
double BTagSFEventWeight::effBTagCjet(double jetPt, double jetEta)
{
  double result = -1111.;
  // if histo file exists, take value from there; else return a default value
  if(filename_.location()) {
    TH2F* his = effHists_.find("EffCJetsTaggedPtEta")->second;
    // ensure that pt is in accepted range
    if(jetPt >= maxPt11004_) jetPt = maxPt11004_-1.;
    if(jetEta >= maxEta_) jetEta = maxEta_-0.1;
    result = his->GetBinContent( his->FindBin(jetPt, jetEta) );
  }
  else {result = 0.35; std::cout<< "WARNING!!! b tag eff. is ALWAYS 0.35!!! CHECK!!!"<<std::endl; }
  if(verbose_>=2) std::cout<< "effBTagCjet= "<<result<<std::endl;
  return result;
}

// mistag eff. from MC as a function of jet pt, eta
double BTagSFEventWeight::effMisTag(double jetPt, double jetEta)
{
  double result = -1111.;
  // if histo file exists, take value from there; else return a default value
  if(filename_.location()) {
    TH2F* his = effHists_.find("EffLJetsTaggedPtEta")->second;
    // ensure that pt is in accepted range
    if(jetPt >= maxPtMisTag_) jetPt = maxPtMisTag_-1.;
    if(jetEta >= maxEta_) jetEta = maxEta_-0.1;
    result = his->GetBinContent( his->FindBin(jetPt, jetEta) );
  }
  else {result = 0.01; std::cout<< "WARNING!!! b tag eff. is ALWAYS 0.01!!! CHECK!!!"<<std::endl; }
  if(verbose_>=2) std::cout<< "effMisTag= "<<result<<std::endl;
  return result;
}

// mistag eff. SF as a function of jet pt, eta
double BTagSFEventWeight::effMisTagSF(double jetPt, double jetEta)
{
  /// At the moment ALWAYS taken from BTV DB as no uncertainties were available for 11004
  ///
  double result = -1111., error = -1111.;
  const BtagPerformance & perf = *(perfHMisTag.product());
  BinningPointByMap measurePoint;
  if(jetPt >= maxPtMisTag_) jetPt = maxPtMisTag_-1.;
  if(jetEta >= maxEta_) jetEta = maxEta_-0.1;
  measurePoint.insert(BinningVariables::JetEt, jetPt);
  measurePoint.insert(BinningVariables::JetAbsEta, jetEta);
  if(perf.isResultOk( measureMap_[ "BTAGLEFFCORR" ], measurePoint))
       result = perf.getResult( measureMap_[ "BTAGLEFFCORR" ], measurePoint);
  else result = 1.;
  if(perf.isResultOk( measureMap_[ "BTAGLERRCORR" ], measurePoint))
       error = perf.getResult( measureMap_[ "BTAGLERRCORR" ], measurePoint);
  else error = 0.1;
  if(sysVar_ == "misTagSFUp")   result += error;
  if(sysVar_ == "misTagSFDown") result -= error;
  if(verbose_>=2) std::cout<< "effMisTagSF= "<<result<<" +/- "<<error<<std::endl;
  return result;
}

//--------------------------------------------------------------------------

// calculate event b tag efficiency for >=2 b tags
double BTagSFEventWeight::effBTagEvent(std::vector<double> &oneMinusBEffies,
				       std::vector<double> &oneMinusBMistags)
{
  double bTaggingEfficiency = 1.;
  double tmp = 1.;

  if(verbose_) std::cout << oneMinusBEffies.size() << ": " << std::flush;

  for(std::vector<double>::const_iterator eff = oneMinusBEffies.begin();
eff != oneMinusBEffies.end(); ++eff){
    tmp *= (*eff);
    if(verbose_) std::cout << 1.-(*eff) << ", ";
  }
  if(verbose_) std::cout << oneMinusBMistags.size() << ": " << std::flush;
  for(std::vector<double>::const_iterator mis =
oneMinusBMistags.begin(); mis != oneMinusBMistags.end(); ++mis){
    tmp *= (*mis);
    if(verbose_) std::cout << 1.-(*mis) << ", ";
  }
  bTaggingEfficiency -= tmp;
  for(std::vector<double>::const_iterator eff = oneMinusBEffies.begin();
eff != oneMinusBEffies.end(); ++eff){
    tmp = 1.-(*eff);
    for(std::vector<double>::const_iterator eff2 =
oneMinusBEffies.begin(); eff2 != oneMinusBEffies.end(); ++eff2){
      if(eff != eff2) tmp *= (*eff2);
    }
    for(std::vector<double>::const_iterator mis =
oneMinusBMistags.begin(); mis != oneMinusBMistags.end(); ++mis){
      tmp *= (*mis);
    }
    bTaggingEfficiency -= tmp;
  }
  for(std::vector<double>::const_iterator mis =
oneMinusBMistags.begin(); mis != oneMinusBMistags.end(); ++mis){
    tmp = 1.-(*mis);
    for(std::vector<double>::const_iterator eff =
oneMinusBEffies.begin(); eff != oneMinusBEffies.end(); ++eff){
      tmp *= (*eff);
    }
    for(std::vector<double>::const_iterator mis2 =
oneMinusBMistags.begin(); mis2 != oneMinusBMistags.end(); ++mis2){
      if(mis != mis2) tmp *= (*mis2);
    }
    bTaggingEfficiency -= tmp;
  }
  if(verbose_) std::cout << " -> " << bTaggingEfficiency << std::endl;
  return bTaggingEfficiency;

}

// executed at the end after looping over all events
void 
    BTagSFEventWeight::endJob() 
{
  double effBTagEventSFMean = hists_.find("effBTagEventSF" )->second->GetMean();
  hists_.find("effBTagEventSFMean" )->second->Fill(0.5, effBTagEventSFMean );
  if(verbose_>=1) std::cout<<"Mean effBTagEventSF = "<<effBTagEventSFMean<<std::endl;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( BTagSFEventWeight );

