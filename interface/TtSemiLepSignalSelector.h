#ifndef TtSemiLepSignalSelector_h
#define TtSemiLepSignalSelector_h

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
typedef std::vector<pat::Jet> TopJetCollection;

class TtSemiLepSignalSelector {
	// common calculator class for likelihood
	// variables in semi leptonic ttbar decays
public:
	TtSemiLepSignalSelector();
	TtSemiLepSignalSelector(edm::Handle<TopJetCollection>, pat::Particle*, const pat::MET*);
	~TtSemiLepSignalSelector();

	//deltaPhi(MET, jet4)
	double dphiMETJet4() {
		return dphiMETJet4_;
	}
	//deltaPhi(MET, leading lepton)
	double dphiMETLepton() {
		return dphiMETLepton_;
	}
	//MET * Et(jet1)
	double METTimesLeadingJet() {
		return METTimesLeadingJet_;
	}
	//Et(jet3) + Et(jet4)
	double sumEtJet3And4() {
		return sumEtJet3And4_;
	}
	//summ Et(all jets) + pt(lepton)
	double sumAllJetsEtAndLepton() {
		return sumAllJetsEtAndLepton_;
	}

	//|sumVec p(all jets, lepton)|
	double vecSumAllJetsAndLepton() {
		return vecSumAllJetsAndLepton_;
	}
	//Et(jet1) + 2*pt(lepton)
	double sumEtleadingJetAnd2TimesLeptonPt() {
		return sumEtleadingJetAnd2TimesLeptonPt_;
	}

	//deltaPhi(MET, jet2)
	double dphiMETJet2() {
		return dphiMETJet2_;
	}
private:
	double dphiMETJet4_, dphiMETLepton_;
	double METTimesLeadingJet_, sumEtJet3And4_;
	double sumAllJetsEtAndLepton_, vecSumAllJetsAndLepton_;
	double sumEtleadingJetAnd2TimesLeptonPt_;
	double dphiMETJet2_;

};

#endif
