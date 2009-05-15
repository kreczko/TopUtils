#ifndef TopAnalysis_TopUtils_JetCalibrationVariables_h
#define TopAnalysis_TopUtils_JetCalibrationVariables_h

#include <string>
// For ROOT types with '_t':
#include <Rtypes.h>

/// container to hold data to be written into TTree
struct JetCalibrationVariables
{
  /// constructor initialises to empty values
  JetCalibrationVariables()  { this->clear();}
  
  /// set to empty values
  void clear() {
    GenJet_energy = GenJet_et = GenJet_pt = GenJet_eta = GenJet_phi = RecoJet_energy = RecoJet_et = RecoJet_pt = RecoJet_eta = RecoJet_phi = pt_reco_over_gen = RecoJet_toyet = RecoJet_emFraction = 0.;
    Jet_flavour = 0;
  }
  void fillVariables(Double_t energy_GenJets, Double_t et_GenJets, Double_t pt_GenJets, Double_t eta_GenJets, Double_t phi_GenJets, Double_t energy_RecoJets, Double_t et_RecoJets, Double_t pt_RecoJets, Double_t eta_RecoJets, Double_t phi_RecoJets, Double_t pt_ratio, Int_t parton_flavour, Double_t toyet_RecoJets, Double_t emFraction_RecoJet) {
    GenJet_energy = energy_GenJets; GenJet_et = et_GenJets; GenJet_pt = pt_GenJets; GenJet_eta = eta_GenJets; GenJet_phi = phi_GenJets; RecoJet_energy = energy_RecoJets; RecoJet_et = et_RecoJets; RecoJet_pt = pt_RecoJets; RecoJet_eta = eta_RecoJets; RecoJet_phi = phi_RecoJets; pt_reco_over_gen = pt_ratio; Jet_flavour = parton_flavour; RecoJet_toyet = toyet_RecoJets; RecoJet_emFraction = emFraction_RecoJet;
  }

  Double_t GenJet_energy, GenJet_et, GenJet_pt, GenJet_eta, GenJet_phi, RecoJet_energy, RecoJet_et, RecoJet_pt, RecoJet_eta, RecoJet_phi, pt_reco_over_gen, RecoJet_toyet, RecoJet_emFraction;
 
  Int_t Jet_flavour;
};

#endif
