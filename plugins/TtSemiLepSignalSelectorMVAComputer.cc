#include "PhysicsTools/JetMCUtils/interface/combination.h"

#include "TopAnalysis/TopUtils/plugins/TtSemiLepSignalSelectorMVAComputer.h"
#include "TopAnalysis/TopUtils/interface/QCDBkgEstimateEval.h"

#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Math/interface/deltaR.h"

TtSemiLepSignalSelectorMVAComputer::TtSemiLepSignalSelectorMVAComputer(const edm::ParameterSet& cfg) :
	leptons_(cfg.getParameter<edm::InputTag> ("muons")), jets_(cfg.getParameter<edm::InputTag> ("jets")), METs_(
			cfg.getParameter<edm::InputTag> ("METs")) {
	produces<double> ("DiscSel");
}

TtSemiLepSignalSelectorMVAComputer::~TtSemiLepSignalSelectorMVAComputer() {
}

void TtSemiLepSignalSelectorMVAComputer::produce(edm::Event& evt, const edm::EventSetup& setup) {

	std::auto_ptr< double> pOutDisc(new double);

	mvaComputer.update<TtSemiLepSignalSelectorMVARcd> (setup, "ttSemiLepSignalSelectorMVA");

	// read name of the last processor in the MVA calibration
	// (to be used as meta information)
	edm::ESHandle<PhysicsTools::Calibration::MVAComputerContainer> calibContainer;
	setup.get<TtSemiLepSignalSelectorMVARcd> ().get(calibContainer);
	std::vector<PhysicsTools::Calibration::VarProcessor*> processors = (calibContainer->find(
			"ttSemiLepSignalSelectorMVA")).getProcessors();

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

	lepton = &(*leptons)[0];

	edm::Handle< std::vector<pat::Jet> > jet_handle;
	evt.getByLabel(jets_, jet_handle);
	if (!jet_handle.isValid())
		return;
	const TopJetCollection jets = *jet_handle;

	double discrim = -1;

	// skip events with no appropriate lepton candidate in
	if (leptons->size() == 1 && jets.size() >= 4)
	{
		QCDBkgEstimateSelector selection(jets, lepton, MET);

		discrim = evaluateQCDBkgEstimateSelector(mvaComputer, selection, weight);
	}

	*pOutDisc = discrim;

	evt.put(pOutDisc, "DiscSel");
}
void TtSemiLepSignalSelectorMVAComputer::endJob(){
}

// implement the plugins for the computer container
// -> register TtSemiLepSignalSelMVARcd
// -> define TtSemiLepSignalSelMVAFileSource
MVA_COMPUTER_CONTAINER_IMPLEMENT(TtSemiLepSignalSelectorMVA);
