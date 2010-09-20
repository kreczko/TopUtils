#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "TopAnalysis/TopUtils/plugins/ResidualJetCorrector.h"

ResidualJetCorrector::ResidualJetCorrector(const edm::ParameterSet& cfg):
  jets_        ( cfg.getParameter<edm::InputTag>("jets"     ) ),
  corrections_ ( cfg.getParameter<std::string>("corrections") )
{

  JEC_PATH = "CondFormats/JetMETObjects/data/";

  edm::LogWarning("Disclaimer") << "This module may only be used on real data.\nWhen running on MC this module needs to be removed from workflow!\n";

  fipRes = edm::FileInPath(JEC_PATH+corrections_);
  ResJetCorPar = new JetCorrectorParameters(fipRes.fullPath());
  vParam.push_back(*ResJetCorPar);
  JEC = new FactorizedJetCorrector(vParam);

  produces< std::vector<pat::Jet> >();
}

void
ResidualJetCorrector::produce(edm::Event& evt, const edm::EventSetup& setup)
{
  /// get original jet collection
  edm::Handle< std::vector<pat::Jet> > jets; 
  evt.getByLabel(jets_, jets);

  std::auto_ptr< std::vector<pat::Jet> > myJets( new std::vector<pat::Jet>() );
  /// loop over the original jet collection
  for(std::vector<pat::Jet>::const_iterator jet = jets->begin(); jet != jets->end(); ++jet){
    JEC->setJetEta(jet->eta());
    JEC->setJetPt(jet->pt());

    pat::Jet myJet(*jet);
    myJet.setP4(jet->p4() * JEC->getCorrection());
    myJets->push_back(myJet);
  }
  evt.put(myJets);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( ResidualJetCorrector );
