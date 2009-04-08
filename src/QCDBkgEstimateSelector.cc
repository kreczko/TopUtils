#include "TopAnalysis/TopUtils/interface/QCDBkgEstimateSelector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "TopQuarkAnalysis/TopTools/interface/EventShapeVariables.h"
#include "TVector3.h"

std::vector<TVector3> makeVecForEventShape(TopJetCollection jets, const pat::Muon *muon, double scale = 1.) {
	std::vector<TVector3> p;
	TopJetCollection::const_iterator jet = jets.begin();
	for (int i = 0; i < 4; i++) {
		TVector3 Vjet(jet->px() * scale, jet->py() * scale, jet->pz() * scale);
		p.push_back(Vjet);
		jet++;
	}
	TVector3 mu(muon->px() * scale, muon->py() * scale, muon->pz() * scale);
	p.push_back(mu);
	return p;
}

QCDBkgEstimateSelector::QCDBkgEstimateSelector() {
}

QCDBkgEstimateSelector::QCDBkgEstimateSelector(TopJetCollection jets, const pat::Muon *lepton, const pat::MET *met) {

	if (jets.size() >= 4) {
		std::vector<TVector3> p;

		TopJetCollection::const_iterator jet = jets.begin();

		double jet1Phi, jet2Phi, jet3Phi, jet4Phi;
		double jet1Eta, jet2Eta, jet3Eta, jet4Eta;
		double jet1pt, jet2pt, jet3pt, jet4pt;
		double jet1Et, jet2Et, jet3Et, jet4Et;
		reco::Particle::LorentzVector vec;

		jet1pt = jet->pt();
		jet1Et = jet->et();
		jet1Eta = jet->eta();
		jet1Phi = jet->phi();

		vec = jet->p4();

		++jet;
		jet2pt = jet->pt();
		jet2Et = jet->et();
		jet2Eta = jet->eta();
		jet2Phi = jet->phi();

		vec += jet->p4();

		++jet;
		jet3pt = jet->pt();
		jet3Et = jet->et();
		jet3Eta = jet->eta();
		jet3Phi = jet->phi();

		vec += jet->p4();

		++jet;
		jet4pt = jet->pt();
		jet4Et = jet->et();
		jet4Eta = jet->eta();
		jet4Phi = jet->phi();

		vec += jet->p4();
		vec += lepton->p4();
		p = makeVecForEventShape(jets, lepton);
		EventShapeVariables eventshape;

		dphiMETJet4_ = deltaPhi(jet4Phi, met->phi());
		dphiMETLepton_ = deltaPhi(met->phi(), lepton->phi());
		METTimesLeadingJet_ = met->et() * jet1Et;
		sumEtJet3And4_ = jet3Et + jet4Et;
		sumAllJetsEtAndLepton_ = jet1Et + jet2Et + jet3Et + jet4Et + lepton->et();
		vecSumAllJetsAndLepton_ = vec.pt();
		sumEtleadingJetAnd2TimesLeptonPt_ = jet1Et + 2 * lepton->et();
		dphiMETJet2_ = deltaPhi(jet2Phi, met->phi());
		MET_ = met->et();
		dphiMuJ1J2_ = deltaPhi(lepton->phi(), jet1Phi) + deltaPhi(lepton->phi(), jet2Phi);
		leptonPt_ = lepton->pt();
		aplanarity_ = eventshape.aplanarity(p);
		sphericity_ = eventshape.sphericity(p);
		circularity_ = eventshape.circularity(p);
		isotropy_ = eventshape.isotropy(p);
	}
}

QCDBkgEstimateSelector::~QCDBkgEstimateSelector() {

}
