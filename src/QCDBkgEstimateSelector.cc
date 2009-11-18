#include "TopAnalysis/TopUtils/interface/QCDBkgEstimateSelector.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "PhysicsTools/CandUtils/interface/EventShapeVariables.h"
#include "DataFormats/Math/interface/Vector3D.h"

using namespace std;
std::vector<math::XYZVector> makeVecForEventShape(TopJetCollection jets, const pat::Muon *muon, double scale = 1.) {
	std::vector<math::XYZVector> p;
	TopJetCollection::const_iterator jet = jets.begin();
	for (int i = 0; i < 4; i++) {
		math::XYZVector Vjet(jet->px() * scale, jet->py() * scale, jet->pz() * scale);
		p.push_back(Vjet);
		jet++;
	}
	math::XYZVector mu(muon->px() * scale, muon->py() * scale, muon->pz() * scale);
	p.push_back(mu);
	return p;
}

QCDBkgEstimateSelector::QCDBkgEstimateSelector() {
}

QCDBkgEstimateSelector::QCDBkgEstimateSelector(TopJetCollection jets, const pat::Muon *lepton, const pat::MET *met) {

	if (jets.size() >= 4) {
		std::vector<math::XYZVector> p;

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
		EventShapeVariables eventshape(p);

		dphiMETJet1_ = deltaPhi(met->phi(), jet1Phi);
		dphiMETJet2_ = deltaPhi(met->phi(), jet2Phi);
		dphiMETJet3_ = deltaPhi(met->phi(), jet3Phi);
		dphiMETJet4_ = deltaPhi(met->phi(), jet4Phi);
		dphiMETLepton_ = deltaPhi(met->phi(), lepton->phi());
		dphiMuJ1J2_ = deltaPhi(lepton->phi(), jet1Phi) + deltaPhi(lepton->phi(), jet2Phi);
		METTimesLeadingJet_ = met->et() * jet1Et;
		sumEtJet3And4_ = jet3Et + jet4Et;
		sumAllJetsEtAndLepton_ = jet1Et + jet2Et + jet3Et + jet4Et + lepton->et();
		vecSumAllJetsAndLepton_ = vec.pt();
		sumEtleadingJetAnd2TimesLeptonPt_ = jet1Et + 2 * lepton->et();
		MET_ = met->et();

		leptonPt_ = lepton->pt();
		aplanarity_ = eventshape.aplanarity();
		sphericity_ = eventshape.sphericity();
		circularity_ = eventshape.circularity();
		isotropy_ = eventshape.isotropy();
		Jet3EtOverJet1EtJet3Et_ = jet3Et / (jet3Et + jet1Et);
		Jet3EtOverJet2EtJet3Et_ = jet3Et / (jet3Et + jet2Et);
		Jet4EtOverJet1EtJet4Et_ = jet4Et / (jet4Et + jet1Et);
		Jet4EtOverJet2EtJet4Et_ = jet4Et / (jet4Et + jet2Et);
		invariantMassJ3andJ4_ = sqrt((jet3Et + jet4Et) * (jet3Et + jet4Et) - (jet3pt + jet4pt) * (jet3pt + jet4pt));
		DeltaPhiMuonJet3_ = deltaPhi(lepton->phi(), jet3Phi);
		DeltaPhiMuonJet4_ = deltaPhi(lepton->phi(), jet4Phi);

		//trying to reconstruct the deltaPhi between the two top quarks
		double phi1, phi2, phi3, phi4, phi5, phi6, phi7, phi8;
		//get closest jet to mu (should be bjet from semileptonic t-decay)
		pat::Jet closestMuJet = getClosestJetInDeltaPhi(jets, lepton->phi());
		//get 4th jet + closest (hadronic W)
		phi1 = jet3Phi + (0.5 * deltaPhi(jet4Phi, jet3Phi));
		phi2 = jet4Phi + (0.5 * deltaPhi(jet3Phi, jet4Phi));
		pat::Jet closest34Jet = getClosestJetInDeltaPhi(jets, phi1);
		phi3 = phi1 + 0.5 * deltaPhi(closest34Jet.phi(), phi1);
		// phi of hadronic top
		phi4 = closest34Jet.phi() + 0.5 * deltaPhi(phi1, closest34Jet.phi());
		// get neutrino and leptonon
		phi5 = lepton->phi() + 0.5 * deltaPhi(met->phi(), lepton->phi());
		phi6 = met->phi() + 0.5 * deltaPhi(lepton->phi(), met->phi());
		phi7 = phi5 + 0.5 * deltaPhi(closestMuJet.phi(), phi5);
		// phi of leptonic top
		phi8 = closestMuJet.phi() + 0.5 * deltaPhi(phi5, closestMuJet.phi());
		DeltaPhiTtbar_ = deltaPhi(phi8, phi4);
		deltaPhiJet1Jet2_ = deltaPhi(jet1Phi, jet2Phi);

		double mt3Jet1 = sqrt((jet1Et + jet3Et + jet4Et) * (jet1Et + jet3Et + jet4Et) - (jet1pt + jet3pt + jet4pt)
				* (jet1pt + jet3pt + jet4pt));
		double mt3Jet2 = sqrt((jet2Et + jet3Et + jet4Et) * (jet2Et + jet3Et + jet4Et) - (jet2pt + jet3pt + jet4pt)
				* (jet2pt + jet3pt + jet4pt));
		//TODO: make W-mass as config parameter
		if (fabs(mt3Jet1 - 170) < fabs(mt3Jet2 - 170)) {
			//mt3Jet1 closer to W mass
			TriJetTMass_ = mt3Jet1;
		} else
			TriJetTMass_ = mt3Jet2;
		DeltaPhiTimesDeltaEta_ = deltaPhi(met->phi(), jet1Phi) * fabs(met->eta() - jet1Eta);
		minDeltaPhiMETJets_ = min(fabs(deltaPhi(met->phi(), jet1Phi)), fabs(deltaPhi(met->phi(), jet2Phi)));
		minDeltaPhiMETJets_ = min(minDeltaPhiMETJets_, fabs(deltaPhi(met->phi(), jet3Phi)));
		minDeltaPhiMETJets_ = min(minDeltaPhiMETJets_, fabs(deltaPhi(met->phi(), jet4Phi)));
	}

}

QCDBkgEstimateSelector::~QCDBkgEstimateSelector() {

}

pat::Jet QCDBkgEstimateSelector::getClosestJetInDeltaPhi(TopJetCollection j, double phi) {
	pat::Jet closestJet;
	if (j.size() >= 4) {
		double dr2 = 999.;
		TopJetCollection::const_iterator jet = j.begin();
		//only the four highest jets
		for (unsigned x = 0; x < 4; x++) {
			if (jet == j.end())
				break;
			double temp = fabs(deltaPhi(jet->phi(), phi));
			if (temp < dr2) {
				dr2 = temp;
				closestJet = *jet;
			}
			++jet;
		}
	}
	return closestJet;
}
