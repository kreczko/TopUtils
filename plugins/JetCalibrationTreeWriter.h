// system include files
#include <memory>

// framework include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Common/interface/View.h"
// user include files
#include "TTree.h"
#include "TMath.h"
#include "TRandom.h"
#include "TRandom3.h"

#include "TopAnalysis/TopUtils/interface/JetCalibrationVariables.h"

using namespace reco;
using namespace std;

edm::Service<TFileService> fs;
TTree *tree;
JetCalibrationVariables *treeMemPtr;
edm::InputTag jetLabel_;