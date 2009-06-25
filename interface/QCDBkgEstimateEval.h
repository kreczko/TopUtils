#ifndef QCDBkgEstimateEval_h
#define QCDBkgEstimateEval_h

#include "PhysicsTools/MVAComputer/interface/MVAComputerCache.h"
#include "PhysicsTools/MVATrainer/interface/MVATrainer.h"
#include "TopAnalysis/TopUtils/interface/QCDBkgEstimateSelector.h"

inline double evaluateQCDBkgEstimateSelector(PhysicsTools::MVAComputerCache& mvaComputer,
		const QCDBkgEstimateSelector& sigsel, float weight = 1., const bool training = false, const bool isSignal =
				false) {
	std::vector<PhysicsTools::Variable::Value> values;

	if (training)
		values.push_back(PhysicsTools::Variable::Value(PhysicsTools::MVATrainer::kTargetId, isSignal));
	if (training)
		values.push_back(PhysicsTools::Variable::Value(PhysicsTools::MVATrainer::kWeightId, weight));

	values.push_back(PhysicsTools::Variable::Value("dphiMETJet1", sigsel.dphiMETJet1()));
	values.push_back(PhysicsTools::Variable::Value("dphiMETJet2", sigsel.dphiMETJet2()));
	values.push_back(PhysicsTools::Variable::Value("dphiMETJet3", sigsel.dphiMETJet3()));
	values.push_back(PhysicsTools::Variable::Value("dphiMETJet4", sigsel.dphiMETJet4()));

	values.push_back(PhysicsTools::Variable::Value("dphiMETLepton", sigsel.dphiMETLepton()));
	values.push_back(PhysicsTools::Variable::Value("dphiMuJ1J2", sigsel.dphiMuJ1J2()));
	values.push_back(PhysicsTools::Variable::Value("METTimesLeadingJet", sigsel.METTimesLeadingJet()));
	values.push_back(PhysicsTools::Variable::Value("sumEtJet3And4", sigsel.sumEtJet3And4()));

	values.push_back(PhysicsTools::Variable::Value("sumAllJetsEtAndLepton", sigsel.sumAllJetsEtAndLepton()));
	values.push_back(PhysicsTools::Variable::Value("vecSumAllJetsAndLepton", sigsel.vecSumAllJetsAndLepton()));
	values.push_back(PhysicsTools::Variable::Value("sumEtleadingJetAnd2TimesLeptonPt",
			sigsel.sumEtleadingJetAnd2TimesLeptonPt()));
	values.push_back(PhysicsTools::Variable::Value("MET", sigsel.MET()));

	values.push_back(PhysicsTools::Variable::Value("leptonPt", sigsel.leptonPt()));

	values.push_back(PhysicsTools::Variable::Value("aplanarity", sigsel.aplanarity()));
	values.push_back(PhysicsTools::Variable::Value("sphericity", sigsel.sphericity()));
	values.push_back(PhysicsTools::Variable::Value("circularity", sigsel.circularity()));
	values.push_back(PhysicsTools::Variable::Value("isotropy", sigsel.isotropy()));

	values.push_back(PhysicsTools::Variable::Value("Jet3EtOverJet1EtJet3Et", sigsel.Jet3EtOverJet1EtJet3Et()));
	values.push_back(PhysicsTools::Variable::Value("Jet3EtOverJet2EtJet3Et", sigsel.Jet3EtOverJet2EtJet3Et()));
	values.push_back(PhysicsTools::Variable::Value("Jet4EtOverJet1EtJet4Et", sigsel.Jet4EtOverJet1EtJet4Et()));
	values.push_back(PhysicsTools::Variable::Value("Jet4EtOverJet2EtJet4Et", sigsel.Jet4EtOverJet2EtJet4Et()));

	values.push_back(PhysicsTools::Variable::Value("invariantMassJ3andJ4", sigsel.invariantMassJ3andJ4()));
	values.push_back(PhysicsTools::Variable::Value("DeltaPhiMuonJet3", sigsel.DeltaPhiMuonJet3()));
	values.push_back(PhysicsTools::Variable::Value("DeltaPhiMuonJet4", sigsel.DeltaPhiMuonJet4()));
	values.push_back(PhysicsTools::Variable::Value("deltaPhiJet1Jet2", sigsel.deltaPhiJet1Jet2()));

	values.push_back(PhysicsTools::Variable::Value("DeltaPhiTtbar", sigsel.DeltaPhiTtbar()));
	values.push_back(PhysicsTools::Variable::Value("TriJetTMass", sigsel.TriJetTMass()));
	values.push_back(PhysicsTools::Variable::Value("DeltaPhiTimesDeltaEta", sigsel.DeltaPhiTimesDeltaEta()));
	values.push_back(PhysicsTools::Variable::Value("minDeltaPhiMETJets", sigsel.minDeltaPhiMETJets()));

	return mvaComputer->eval(values);

}

#endif
