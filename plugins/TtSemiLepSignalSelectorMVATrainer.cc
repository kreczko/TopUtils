#include "PhysicsTools/MVATrainer/interface/HelperMacros.h"

#include "TopAnalysis/TopUtils/plugins/TtSemiLepSignalSelectorMVATrainer.h"
#include "TopAnalysis/TopUtils/interface/QCDBkgEstimateEval.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
//#include "DataFormats/PatCandidates/interface/Lepton.h"
#include "DataFormats/Math/interface/deltaR.h"
typedef std::vector<pat::Jet> TopJetCollection;

TtSemiLepSignalSelectorMVATrainer::TtSemiLepSignalSelectorMVATrainer(const edm::ParameterSet& cfg) :
	leptons_(cfg.getParameter<edm::InputTag> ("leptons")), jets_(cfg.getParameter<edm::InputTag> ("jets")), METs_(
			cfg.getParameter<edm::InputTag> ("mets")), lepChannel_(
			cfg.getParameter<int> ("lepChannel")) {
}

TtSemiLepSignalSelectorMVATrainer::~TtSemiLepSignalSelectorMVATrainer() {
}

void TtSemiLepSignalSelectorMVATrainer::analyze(const edm::Event& evt, const edm::EventSetup& setup) {
	mvaComputer.update<TtSemiLepSignalSelectorMVARcd> ("trainer", setup, "ttSemiLepSignalSelectorMVA");

	// can occur in the last iteration when the
	// MVATrainer is about to save the result
	if (!mvaComputer)
		return;

	const pat::Muon *lepton;
	const pat::MET* MET;
	double weight = 0.;


	edm::Handle<double> weightHandle;
	evt.getByLabel("eventWeight", weightHandle);
	weight = *weightHandle;

	edm::Handle<std::vector<pat::MET> > metH;
	evt.getByLabel(METs_, metH);
	MET = &(*metH)[0];

	edm::Handle<std::vector<pat::Muon> > leptons;
	evt.getByLabel(leptons_, leptons);

	//lepton cuts
	if (leptons->size() != 1)
		return;

	lepton = &(*leptons)[0];

	edm::Handle< std::vector<pat::Jet> > jet_handle;
	evt.getByLabel(jets_, jet_handle);
	if (!jet_handle.isValid())
		return;
	const TopJetCollection jets = *jet_handle;

//	double dRmin = 9999.;
//	for (std::vector<pat::Jet>::const_iterator it = jets.begin(); it != jets.end(); it++) {
//		double tmpdR = deltaR(it->eta(), it->phi(), lepton->eta(), lepton->phi());
//		if (tmpdR < dRmin)
//			dRmin = tmpdR;
//	}


	//at last one lepton
	if (!(leptons->size() == 0)) {

		QCDBkgEstimateSelector selection(jets, lepton, MET);

//		if (genEvt->isSemiLeptonic() && genEvt->semiLeptonicChannel() == lepChannel_) {
//			evaluateQCDBkgEstimateSelector(mvaComputer, selection, weight, true, true);
//		} else{
//			evaluateQCDBkgEstimateSelector(mvaComputer, selection, weight, true, false);
//		}
		evaluateQCDBkgEstimateSelector(mvaComputer, selection, weight, true, true);

	} else
		return;
}

// implement the plugins for the trainer
// -> defines TtSemiLepSignalSelMVAContainerSaveCondDB
// -> defines TtSemiLepSignalSelMVASaveFile
// -> defines TtSemiLepSignalSelMVATrainerLooper
MVA_TRAINER_IMPLEMENT(TtSemiLepSignalSelectorMVA);
