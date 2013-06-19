#include <algorithm>

#include "TopAnalysis/TopUtils/plugins/JetEnergyScale.h"

#include "CommonTools/Utils/interface/PtComparator.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

JetEnergyScale::JetEnergyScale(const edm::ParameterSet& cfg):
  inputElectrons_      (cfg.getParameter<edm::InputTag>("inputElectrons"      )),
  inputJets_           (cfg.getParameter<edm::InputTag>("inputJets"           )),
  inputMETs_           (cfg.getParameter<edm::InputTag>("inputMETs"           )),
  payload_             (cfg.getParameter<std::string>  ("payload"             )),  
  scaleType_           (cfg.getParameter<std::string>  ("scaleType"           )),  
  sourceName_          (cfg.getParameter<std::string>  ("sourceName"          )),  
  scaleFactor_         (cfg.getParameter<double>       ("scaleFactor"         )),
  scaleFactorB_        (cfg.getParameter<double>       ("scaleFactorB"        )),
  resolutionFactor_    (cfg.getParameter<std::vector<double> > ("resolutionFactors"   )),
  resolutionRanges_    (cfg.getParameter<std::vector<double> > ("resolutionEtaRanges" )),
  JECUncSrcFile_       (cfg.getParameter<edm::FileInPath>("JECUncSrcFile") ),
  jetPTThresholdForMET_(cfg.getParameter<double>       ("jetPTThresholdForMET")),
  jetEMLimitForMET_    (cfg.getParameter<double>       ("jetEMLimitForMET"    ))
{
  // define allowed types
  allowedTypes_.push_back(std::string("abs"));
  allowedTypes_.push_back(std::string("rel"));
  allowedTypes_.push_back(std::string("jes:up"));
  allowedTypes_.push_back(std::string("jes:down"));
  allowedTypes_.push_back(std::string("top:up"));
  allowedTypes_.push_back(std::string("top:down"));
  allowedTypes_.push_back(std::string("flavor:up"));
  allowedTypes_.push_back(std::string("flavor:down"));
  allowedTypes_.push_back(std::string("source:up"));
  allowedTypes_.push_back(std::string("source:down"));

  // use label of input to create label for output
  outputElectrons_ = inputElectrons_.label();
  outputJets_ = inputJets_.label();
  outputMETs_ = inputMETs_.label(); 

  // register products
  produces<std::vector<pat::Electron> >(outputElectrons_);
  produces<std::vector<pat::Jet> >(outputJets_);
  produces<std::vector<pat::MET> >(outputMETs_); 
}

void
JetEnergyScale::beginJob()
{ 
  // check if scaleType is ok
  if(std::find(allowedTypes_.begin(), allowedTypes_.end(), scaleType_)==allowedTypes_.end()){
    edm::LogError msg("JetEnergyScale"); 
    msg << "Unknown scaleType: " << scaleType_ << " allowed types are: \n";
    for(std::vector<std::string>::const_iterator type=allowedTypes_.begin(); type!=allowedTypes_.end(); ++type){
      msg << *type << "\n";
    }
    msg << "Please modify your configuration accordingly \n";
    throw cms::Exception("Configuration Error");
  }		
}

///function to sort any auto_ptr to a vector of anything which has a pt function
template<typename T>
void sortByPt(std::auto_ptr<std::vector<T> > &collection) {
  std::sort(collection->begin(), collection->end(), GreaterByPt<T>());
}

void
JetEnergyScale::produce(edm::Event& event, const edm::EventSetup& setup)
{
  // access jets
  edm::Handle<std::vector<pat::Jet> > jets;
  event.getByLabel(inputJets_, jets);
  // access MET
  edm::Handle<std::vector<pat::MET> > mets;
  event.getByLabel(inputMETs_, mets);

  // keep differences for met rescaling
  double dPx = 0., dPy = 0., dSumEt = 0.;

  // create three new collections for jets and MET and electrons
  std::auto_ptr<std::vector<pat::Electron> > pElectrons(new std::vector<pat::Electron>);
  std::auto_ptr<std::vector<pat::Jet> > pJets(new std::vector<pat::Jet>);
  std::auto_ptr<std::vector<pat::MET> > pMETs(new std::vector<pat::MET>);

  // access electrons
  if (! inputElectrons_.label().empty()) {
  
    edm::Handle<std::vector<pat::Electron> > electrons;
    event.getByLabel(inputElectrons_, electrons);

    for(std::vector<pat::Electron>::const_iterator electron=electrons->begin(); electron!=electrons->end(); ++electron){
      pat::Electron scaledElectron = *electron;
      scaledElectron.setP4(scaledElectron.ecalDrivenMomentum());
      pElectrons->push_back( scaledElectron );
        
      dPx    += scaledElectron.px() - electron->px();
      dPy    += scaledElectron.py() - electron->py();
      dSumEt += scaledElectron.et() - electron->et();
    }
  }

  for(std::vector<pat::Jet>::const_iterator jet=jets->begin(); jet!=jets->end(); ++jet){
    pat::Jet scaledJet = *jet;

    // JER scaled for all possible methods
    double jerScaleFactor = resolutionFactor(scaledJet);
    scaleJetEnergy( scaledJet, jerScaleFactor );
    double jetPtAfterJERScaling = scaledJet.pt();
    if(scaleType_=="abs"){
      scaleJetEnergy( scaledJet, scaleFactor_ );
      if (abs(scaledJet.partonFlavour()) == 5) {
	scaleJetEnergy( scaledJet, scaleFactorB_ );
      }
    }
    else if(scaleType_=="rel"){
      scaleJetEnergy( scaledJet, 1+(fabs(scaledJet.eta())*(scaleFactor_-1. )) );
    }    
    else if(scaleType_.substr(0, scaleType_.find(':'))=="jes" || 
	    scaleType_.substr(0, scaleType_.find(':'))=="top" ){
      // get the uncertainty parameters from file, see
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECUncertaintySources
      JetCorrectorParameters* param = new JetCorrectorParameters(JECUncSrcFile_.fullPath(), "Total");
      // instantiate the jec uncertainty object
      JetCorrectionUncertainty* deltaJEC = new JetCorrectionUncertainty(*param);
      deltaJEC->setJetEta(jet->eta()); deltaJEC->setJetPt(jet->pt()); 

      // additional JES uncertainty from Top group
      // sum of squared shifts of jet energy to be applied
      float topShift2 = 0.;
      if(scaleType_.substr(0, scaleType_.find(':'))=="top"){
	// add the recommended PU correction on top  
	float pileUp = 0.352/jet->pt()/jet->pt();
	// add bjet uncertainty on top
	float bjet = 0.;
	if(jet->partonFlavour() == 5 || jet->partonFlavour() == -5)
	  bjet = ((50<jet->pt() && jet->pt()<200) && fabs(jet->eta())<2.0) ? 0.02 : 0.03;
	// add flat uncertainty for release differences and calibration changes (configurable)
	float sw = (1.-scaleFactor_);
	// add top systematics to JES uncertainty
	topShift2 += pileUp*pileUp + bjet*bjet + sw*sw;
      }

      // scale jet energy
      if(scaleType_.substr(scaleType_.find(':')+1)=="up"){
	// JetMET JES uncertainty
	float jetMet = deltaJEC->getUncertainty(true);
	scaleJetEnergy( scaledJet, 1+std::sqrt(jetMet*jetMet + topShift2) );
      }
      else if(scaleType_.substr(scaleType_.find(':')+1)=="down"){
	// JetMET JES uncertainty
	float jetMet = deltaJEC->getUncertainty(false);
	scaleJetEnergy( scaledJet, 1-std::sqrt(jetMet*jetMet + topShift2) );
      }

      delete deltaJEC;
      delete param;
    }
    // Use AK5PF flavor uncertainty as estimator on the difference between uds- and b-jets
    // Maybe we could make this more generic later (if needed)
    else if(scaleType_.substr(0, scaleType_.find(':'))=="flavor") {
      // get the uncertainty parameters from file, see
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECUncertaintySources
      JetCorrectorParameters* param = new JetCorrectorParameters(JECUncSrcFile_.fullPath(), "Flavor");
      // instantiate the jec uncertainty object
      JetCorrectionUncertainty* deltaJEC = new JetCorrectionUncertainty(*param);
      deltaJEC->setJetEta(jet->eta()); deltaJEC->setJetPt(jet->pt()); 
      
      if (abs(scaledJet.partonFlavour()) == 5) {
        if(scaleType_.substr(scaleType_.find(':')+1)=="up") {
          float jetMetFlavor = deltaJEC->getUncertainty(true);
          scaleJetEnergy( scaledJet, 1+jetMetFlavor );
        }
        else if(scaleType_.substr(scaleType_.find(':')+1)=="down"){
          float jetMetFlavor = deltaJEC->getUncertainty(false);
          scaleJetEnergy( scaledJet, 1-jetMetFlavor );
        }
      }
      delete deltaJEC;
      delete param;
    }
    // Use only a single source for the uncertainty
    else if(scaleType_.substr(0, scaleType_.find(':'))=="source") {
      // get the uncertainty parameters from file, see
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECUncertaintySources
      JetCorrectorParameters* param = new JetCorrectorParameters(JECUncSrcFile_.fullPath(), sourceName_);
      // instantiate the jec uncertainty object
      JetCorrectionUncertainty* deltaJEC = new JetCorrectionUncertainty(*param);
      deltaJEC->setJetEta(jet->eta()); deltaJEC->setJetPt(jet->pt()); 
      
      if(scaleType_.substr(scaleType_.find(':')+1)=="up") {
	float jetMetSource = deltaJEC->getUncertainty(true);
	scaleJetEnergy( scaledJet, 1+jetMetSource );
      }
      else if(scaleType_.substr(scaleType_.find(':')+1)=="down"){
	float jetMetSource = deltaJEC->getUncertainty(false);
	scaleJetEnergy( scaledJet, 1-jetMetSource );
      }
      delete deltaJEC;
      delete param;
    }
    scaledJet.addUserFloat("jerSF"  , jerScaleFactor);
    scaledJet.addUserFloat("jesSF"  , scaledJet.pt()/jetPtAfterJERScaling);
    scaledJet.addUserFloat("totalSF", scaledJet.pt()/jet->pt());
    pJets->push_back( scaledJet );
    
    // consider jet scale shift only if the raw jet pt and emf 
    // is above the thresholds given in the module definition
    if(jet->correctedJet("Uncorrected").pt() > jetPTThresholdForMET_
       && ((!jet->isPFJet() && jet->emEnergyFraction() < jetEMLimitForMET_) ||
           ( jet->isPFJet() && jet->neutralEmEnergyFraction() + jet->chargedEmEnergyFraction() < jetEMLimitForMET_))) {
      //std::cout<<"uncorrJet: "<<jet->correctedJet("Uncorrected")<<std::endl;
      //std::cout<<"scaledJet: "<<scaledJet<<std::endl;
      //std::cout<<"jet      : "<<*jet<<std::endl;
      dPx    += scaledJet.px() - jet->px();
      dPy    += scaledJet.py() - jet->py();
      dSumEt += scaledJet.et() - jet->et();
    }
  }
  
  // scale MET accordingly
  pat::MET met = *(mets->begin());
  //std::cout<<"met before: "<<met.pt()<<std::endl;

  double scaledMETPx = met.px() - dPx;
  double scaledMETPy = met.py() - dPy;
  met.setP4(reco::MET::LorentzVector(scaledMETPx, scaledMETPy, 0, sqrt(scaledMETPx*scaledMETPx+scaledMETPy*scaledMETPy)));
  //std::cout<<"met after: "<<met.pt()<<std::endl;
  pMETs->push_back( met );

  //p4 changes might have changed the pt order, so need to sort the new collections
  sortByPt(pElectrons);
  sortByPt(pJets);

  event.put(pJets, outputJets_);
  event.put(pMETs, outputMETs_);
  event.put(pElectrons, outputElectrons_);
}

double
JetEnergyScale::resolutionFactor(const pat::Jet& jet)
{
  if(!jet.genJet()) { return 1.; }
  // check if vectors are filled properly
  if((2*resolutionFactor_.size())!=resolutionRanges_.size()){
    // eta range==infinity
    if(resolutionFactor_.size()==resolutionRanges_.size()&&resolutionRanges_.size()==1&&resolutionRanges_[0]==-1.){
      resolutionRanges_[0]=0;
      resolutionRanges_.push_back(-1.);
    }
    // others
    else{
      edm::LogError msg("JetEnergyResolution");
      msg << "\n resolutionEtaRanges or resolutionFactors in module JetEnergyScale not filled properly.\n";
      msg << "\n resolutionEtaRanges needs a min and max value for each entry in resolutionFactors.\n";
      throw cms::Exception("invalidVectorFilling");
    }
  }
  // calculate eta dependend JER factor
  double modifiedResolution = 1.;
  for(unsigned int numberOfJERvariation=0; numberOfJERvariation<resolutionFactor_.size(); ++numberOfJERvariation){
    int etaMin = 2*numberOfJERvariation;
    int etaMax = etaMin+1;
    if(std::abs(jet.eta())>=resolutionRanges_[etaMin]&&(std::abs(jet.eta())<resolutionRanges_[etaMax]||resolutionRanges_[etaMax]==-1.)){
      modifiedResolution*=resolutionFactor_[numberOfJERvariation];
      // take care of negative scale factors 
      if(resolutionFactor_[numberOfJERvariation]<0){
	edm::LogError msg("JetEnergyResolution");
	msg << "\n chosen scale factor " << resolutionFactor_[numberOfJERvariation] << " is not valid, must be positive.\n";
	throw cms::Exception("negJERscaleFactors");
      }
    }
  }
  // calculate pt smearing factor
  double factor = 1. + (modifiedResolution-1.)*(jet.pt() - jet.genJet()->pt())/jet.pt();
  return (factor<0 ? 0. : factor);
}

void
JetEnergyScale::scaleJetEnergy(pat::Jet& jet, double factor)
{
  jet.scaleEnergy( factor );

  if(jet.isPFJet()){
    pat::PFSpecific specificPF = jet.pfSpecific();
    specificPF.mChargedHadronEnergy = factor * specificPF.mChargedHadronEnergy;
    specificPF.mNeutralHadronEnergy = factor * specificPF.mNeutralHadronEnergy;
    specificPF.mPhotonEnergy        = factor * specificPF.mPhotonEnergy       ;
    specificPF.mElectronEnergy      = factor * specificPF.mElectronEnergy     ;
    specificPF.mMuonEnergy          = factor * specificPF.mMuonEnergy         ;
    specificPF.mHFHadronEnergy      = factor * specificPF.mHFHadronEnergy     ;
    specificPF.mHFEMEnergy          = factor * specificPF.mHFEMEnergy         ;
    specificPF.mChargedEmEnergy     = factor * specificPF.mChargedEmEnergy    ;
    specificPF.mChargedMuEnergy     = factor * specificPF.mChargedMuEnergy    ;
    specificPF.mNeutralEmEnergy     = factor * specificPF.mNeutralEmEnergy    ;
    jet.setPFSpecific(specificPF);
  }
  else if(jet.isCaloJet() || jet.isJPTJet()){
    pat::CaloSpecific specificCalo = jet.caloSpecific();
    specificCalo.mMaxEInEmTowers         = factor * specificCalo.mMaxEInEmTowers        ;
    specificCalo.mMaxEInHadTowers        = factor * specificCalo.mMaxEInHadTowers       ;
    specificCalo.mHadEnergyInHO          = factor * specificCalo.mHadEnergyInHO         ;
    specificCalo.mHadEnergyInHB          = factor * specificCalo.mHadEnergyInHB         ;
    specificCalo.mHadEnergyInHF          = factor * specificCalo.mHadEnergyInHF         ;
    specificCalo.mHadEnergyInHE          = factor * specificCalo.mHadEnergyInHE         ;
    specificCalo.mEmEnergyInEB           = factor * specificCalo.mEmEnergyInEB          ;
    specificCalo.mEmEnergyInEE           = factor * specificCalo.mEmEnergyInEE          ;
    specificCalo.mEmEnergyInHF           = factor * specificCalo.mEmEnergyInHF          ;
    specificCalo.mEnergyFractionHadronic = factor * specificCalo.mEnergyFractionHadronic;
    specificCalo.mEnergyFractionEm       = factor * specificCalo.mEnergyFractionEm      ;
    jet.setCaloSpecific(specificCalo);

    if(jet.isJPTJet()){
      pat::JPTSpecific specificJPT = jet.jptSpecific();
      specificJPT.mChargedHadronEnergy          = factor * specificJPT.mChargedHadronEnergy         ;
      specificJPT.mNeutralHadronEnergy          = factor * specificJPT.mNeutralHadronEnergy         ;
      specificJPT.mChargedEmEnergy              = factor * specificJPT.mChargedEmEnergy             ;
      specificJPT.mNeutralEmEnergy              = factor * specificJPT.mNeutralEmEnergy             ;
      specificJPT.mSumPtOfChargedWithEff        = factor * specificJPT.mSumPtOfChargedWithEff       ;
      specificJPT.mSumPtOfChargedWithoutEff     = factor * specificJPT.mSumPtOfChargedWithoutEff    ;
      specificJPT.mSumEnergyOfChargedWithEff    = factor * specificJPT.mSumEnergyOfChargedWithEff   ;
      specificJPT.mSumEnergyOfChargedWithoutEff = factor * specificJPT.mSumEnergyOfChargedWithoutEff;
      jet.setJPTSpecific(specificJPT);
    }
  }
}
