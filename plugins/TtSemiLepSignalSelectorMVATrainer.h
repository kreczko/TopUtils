#ifndef TtSemiLepSignalSelectorMVATrainer_h
#define TtSemiLepSignalSelectorMVATrainer_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "PhysicsTools/MVAComputer/interface/HelperMacros.h"
#include "PhysicsTools/MVAComputer/interface/MVAComputerCache.h"

#ifndef TtSemiLepSignalSelectorMVARcd_defined  // to avoid conflicts with the TtSemiSignalSelectorMVAComputer
#define TtSemiLepSignalSelectorMVARcd_defined
MVA_COMPUTER_CONTAINER_DEFINE(TtSemiLepSignalSelectorMVA); // defines TtSemiLepSignalSelectorMVARcd
#endif

class TtSemiLepSignalSelectorMVATrainer: public edm::EDAnalyzer {
public:

	explicit TtSemiLepSignalSelectorMVATrainer(const edm::ParameterSet&);
	~TtSemiLepSignalSelectorMVATrainer();
private:

	virtual void analyze(const edm::Event& evt, const edm::EventSetup& setup);

	edm::InputTag leptons_;
	edm::InputTag jets_;
	edm::InputTag METs_;

	int lepChannel_;

	PhysicsTools::MVAComputerCache mvaComputer;
};
#endif
