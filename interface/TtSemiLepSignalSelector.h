#ifndef TtSemiLepSignalSelector_h
#define TtSemiLepSignalSelector_h

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
typedef std::vector<pat::Jet> TopJetCollection;

class TtSemiLepSignalSelector {
	// common calculator class for likelihood
	// variables in semi leptonic ttbar decays
public:
	TtSemiLepSignalSelector();
	TtSemiLepSignalSelector(TopJetCollection, const pat::Muon*, const pat::MET*);
	~TtSemiLepSignalSelector();

	//deltaPhi(MET, jet4)
	double dphiMETJet4() const {
		return fabs(dphiMETJet4_);
	}
	//deltaPhi(MET, leading lepton)
	double dphiMETLepton() const {
		return fabs(dphiMETLepton_);
	}
	//MET * Et(jet1)
	double METTimesLeadingJet() const {
		return METTimesLeadingJet_;
	}
	//Et(jet3) + Et(jet4)
	double sumEtJet3And4() const {
		return sumEtJet3And4_;
	}
	//summ Et(all jets) + pt(lepton)
	double sumAllJetsEtAndLepton() const {
		return sumAllJetsEtAndLepton_;
	}

//	double sumAllJetsEt() const {
//			return sumAllJetsEt_;
//		}

	//|sumVec p(all jets, lepton)|
	double vecSumAllJetsAndLepton() const {
		return vecSumAllJetsAndLepton_;
	}
	//Et(jet1) + 2*pt(lepton)
	double sumEtleadingJetAnd2TimesLeptonPt() const {
		return sumEtleadingJetAnd2TimesLeptonPt_;
	}

	//deltaPhi(MET, jet2)
	double dphiMETJet2() const {
		return fabs(dphiMETJet2_);
	}

	double MET() const {
		return MET_;
	}

	double dphiMuJ1J2() const {
		return fabs(dphiMuJ1J2_);
	}

	double leptonPt() const {
		return leptonPt_;
	}

	double aplanarity() const {
		return aplanarity_;
	}

	double sphericity() const{
		return sphericity_;
	}

	double circularity() const {
		return circularity_;
	}

	double isotropy() const{
		return isotropy_;
	}
private:
	double dphiMETJet4_, dphiMETLepton_;
	double METTimesLeadingJet_, sumEtJet3And4_;
	double sumAllJetsEtAndLepton_, vecSumAllJetsAndLepton_;
	double sumEtleadingJetAnd2TimesLeptonPt_;
	double dphiMETJet2_, MET_, dphiMuJ1J2_;
	double leptonPt_;//, sumAllJetsEt_;
	double aplanarity_, sphericity_, circularity_, isotropy_;

};

#endif
