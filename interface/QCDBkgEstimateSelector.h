#ifndef QCDBkgEstimateSelector_h
#define QCDBkgEstimateSelector_h

#include <vector>
#include <string>

#include "TMath.h"
#include "Math/VectorUtil.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

typedef std::vector<pat::Jet> TopJetCollection;

class QCDBkgEstimateSelector {
	// common calculator class for likelihood
	// variables in semi leptonic ttbar decays
public:
	QCDBkgEstimateSelector();
	QCDBkgEstimateSelector(TopJetCollection, const pat::Muon*, const pat::MET*);
	~QCDBkgEstimateSelector();

	//deltaPhi(MET, jet1)
	double dphiMETJet1() const {
		return fabs(dphiMETJet1_);
	}
	//deltaPhi(MET, jet2)
	double dphiMETJet2() const {
		return fabs(dphiMETJet2_);
	}
	//deltaPhi(MET, jet3)
	double dphiMETJet3() const {
		return fabs(dphiMETJet3_);
	}
	//deltaPhi(MET, jet4)
	double dphiMETJet4() const {
		return fabs(dphiMETJet4_);
	}
	//deltaPhi(MET, leading lepton)
	double dphiMETLepton() const {
		return fabs(dphiMETLepton_);
	}

	double dphiMuJ1J2() const {
			return fabs(dphiMuJ1J2_);
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

	double MET() const {
		return MET_;
	}

	double leptonPt() const {
		return leptonPt_;
	}

	double aplanarity() const {
		return aplanarity_;
	}

	double sphericity() const {
		return sphericity_;
	}

	double circularity() const {
		return circularity_;
	}

	double isotropy() const {
		return isotropy_;
	}

	double Jet3EtOverJet1EtJet3Et() const{
		return Jet3EtOverJet1EtJet3Et_;
	}

	double Jet3EtOverJet2EtJet3Et() const{
			return Jet3EtOverJet2EtJet3Et_;
		}
	double Jet4EtOverJet1EtJet4Et() const{
			return Jet4EtOverJet1EtJet4Et_;
		}
	double Jet4EtOverJet2EtJet4Et() const{
			return Jet4EtOverJet2EtJet4Et_;
		}

	double invariantMassJ3andJ4() const{
				return invariantMassJ3andJ4_;
			}
	double DeltaPhiMuonJet3() const{
				return fabs(DeltaPhiMuonJet3_);
			}
	double DeltaPhiMuonJet4() const{
				return fabs(DeltaPhiMuonJet4_);
			}
	double deltaPhiJet1Jet2() const{
					return fabs(deltaPhiJet1Jet2_);
				}
	double DeltaPhiTtbar() const{
					return fabs(DeltaPhiTtbar_);
				}
	double TriJetTMass() const{
					return TriJetTMass_;
				}
	double DeltaPhiTimesDeltaEta() const{
						return fabs(DeltaPhiTimesDeltaEta_);
					}
	double minDeltaPhiMETJets() const{
						return fabs(minDeltaPhiMETJets_);
					}

	pat::Jet getClosestJetInDeltaPhi(TopJetCollection, double phi);
private:
	double dphiMETJet1_, dphiMETJet2_, dphiMETJet3_, dphiMETJet4_, dphiMETLepton_, dphiMuJ1J2_;
	double METTimesLeadingJet_, sumEtJet3And4_;
	double sumAllJetsEtAndLepton_, vecSumAllJetsAndLepton_;
	double sumEtleadingJetAnd2TimesLeptonPt_;
	double MET_, leptonPt_;
	double aplanarity_, sphericity_, circularity_, isotropy_;
	double Jet3EtOverJet1EtJet3Et_, Jet3EtOverJet2EtJet3Et_, Jet4EtOverJet1EtJet4Et_, Jet4EtOverJet2EtJet4Et_;
	double invariantMassJ3andJ4_, DeltaPhiMuonJet3_, DeltaPhiMuonJet4_;
	double deltaPhiJet1Jet2_, DeltaPhiTtbar_, TriJetTMass_, DeltaPhiTimesDeltaEta_, minDeltaPhiMETJets_;

};

#endif
