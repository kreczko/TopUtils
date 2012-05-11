// system include files
#include <memory>

// framework include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/Common/interface/View.h"
// user include files
#include "TTree.h"
#include "TMath.h"

#include "TopAnalysis/TopUtils/interface/ResolutionVariables.h"

using namespace reco;
using namespace std;

edm::Service<TFileService> fs;
TTree *tree;
ResolutionVariables *treeMemPtr;
edm::InputTag jetLabel_;
edm::InputTag electronLabel_;
edm::InputTag muonLabel_;
edm::InputTag METLabel_;
std::vector<edm::InputTag> weightTags_;
