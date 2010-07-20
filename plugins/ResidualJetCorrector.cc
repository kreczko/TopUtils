#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "TopAnalysis/TopUtils/plugins/ResidualJetCorrector.h"

ResidualJetCorrector::ResidualJetCorrector(const edm::ParameterSet& cfg):
  jets_            ( cfg.getParameter<edm::InputTag>("jets") ),
  jetType_         ( cfg.getParameter<std::string>("jetType") )
{

  JEC_PATH = "CondFormats/JetMETObjects/data/";

  edm::LogWarning("Disclaimer") << "This module may only be used on real data.\nWhen running on MC this module needs to be removed from workflow!\n";

  if(jetType_ == "PF" || jetType_ == "Calo" || jetType_ == "JPT"){
    fipRes = edm::FileInPath(JEC_PATH+"Spring10DataV1_L2L3Residual_AK5"+jetType_+".txt");
    ResJetCorPar = new JetCorrectorParameters(fipRes.fullPath());
    vParam.push_back(*ResJetCorPar);
    JEC = new FactorizedJetCorrector(vParam);
  }
  else edm::LogInfo("Configuration") << "jetType is not configured. Trying to figure out jetType from inputed jet.\n";
    //throw cms::Exception("Configuration") << "Wrong configured jetType. Please use an appropriate one: Calo, PF or JPT.\n";

  produces< std::vector<pat::Jet> >();
}

void
ResidualJetCorrector::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  /// get original jet collection
  edm::Handle< std::vector<pat::Jet> > jets; 
  evt.getByLabel(jets_, jets);

  

  if(jetType_ != "PF" && jetType_ != "Calo" && jetType_ != "JPT"){
    if(jets->size()){
      if(jets->begin()->isPFJet())        fipRes = edm::FileInPath(JEC_PATH+"Spring10DataV1_L2L3Residual_AK5PF.txt");
      else if(jets->begin()->isCaloJet()) fipRes = edm::FileInPath(JEC_PATH+"Spring10DataV1_L2L3Residual_AK5Calo.txt");
      else throw cms::Exception("Configuration") << "Wrong configured jetType. Please use an appropriate one: Calo, PF or JPT.\n";
      ResJetCorPar = new JetCorrectorParameters(fipRes.fullPath());
      vParam.push_back(*ResJetCorPar);
      JEC = new FactorizedJetCorrector(vParam);
    }
  }

  std::vector<pat::Jet> * newJets = new std::vector<pat::Jet>;
  /// loop over the original jet collection
  for(std::vector<pat::Jet>::const_iterator jet = jets->begin(); jet != jets->end(); ++jet){
    JEC->setJetEta(jet->eta());
    JEC->setJetPt(jet->pt());

    pat::Jet myJet(*jet);
    myJet.setP4(jet->p4() * JEC->getCorrection());
    newJets->push_back(myJet);
  }
  std::auto_ptr< std::vector<pat::Jet> > myJets(newJets);
  evt.put(myJets);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ResidualJetCorrector );
