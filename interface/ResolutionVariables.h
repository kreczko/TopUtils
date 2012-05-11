#ifndef TopAnalysis_TopUtils_ResolutionVariables_h
#define TopAnalysis_TopUtils_ResolutionVariables_h

#include <string>
// For ROOT types with '_t':
#include <Rtypes.h>

/// container to hold data to be written into TTree
struct ResolutionVariables
{
  /// constructor initialises to empty values
  ResolutionVariables()  { this->clear();}
  
  /// set to empty values
  void clear() {
    Gen_energy = Gen_et = Gen_pt = Gen_eta = Gen_phi = Reco_energy = Reco_et = Reco_pt = Reco_eta = Reco_phi = pt_reco_over_gen = Reco_emFraction = Gen_sumEt = Reco_sumEt = DeltaPhi = DeltaR = nextDeltaR = pTinnerTracker = PUweight = PUweightUp = PUweightDown = 0.;
    Particle_ID = whichMuon = -9999;
  }
  void fillVariables(Float_t energy_GenJets, Float_t et_GenJets, Float_t pt_GenJets, Float_t eta_GenJets, Float_t phi_GenJets, Float_t energy_RecoJets, Float_t et_RecoJets, Float_t pt_RecoJets, Float_t eta_RecoJets, Float_t phi_RecoJets, Float_t pt_ratio, Int_t parton_flavour, Float_t emFraction_RecoJet, Float_t sumEt_Gen, Float_t sumEt_Reco, Float_t PhiDelta, Float_t RDelta, Float_t nextRDelta, Int_t whichmuon, Float_t ptinnerTracker, Float_t PUWeight, Float_t PUWeightUp, Float_t PUWeightDown) {
    Gen_energy = energy_GenJets; Gen_et = et_GenJets; Gen_pt = pt_GenJets; Gen_eta = eta_GenJets; Gen_phi = phi_GenJets; Reco_energy = energy_RecoJets; Reco_et = et_RecoJets; Reco_pt = pt_RecoJets; Reco_eta = eta_RecoJets; Reco_phi = phi_RecoJets; pt_reco_over_gen = pt_ratio; Particle_ID = parton_flavour; Reco_emFraction = emFraction_RecoJet; Gen_sumEt = sumEt_Gen; Reco_sumEt = sumEt_Reco; DeltaPhi = PhiDelta; DeltaR = RDelta; nextDeltaR = nextRDelta; whichMuon = whichmuon; pTinnerTracker = ptinnerTracker; PUweight = PUWeight; PUweightUp = PUWeightUp; PUweightDown = PUWeightDown;
  }

  Float_t Gen_energy, Gen_et, Gen_pt, Gen_eta, Gen_phi, Reco_energy, Reco_et, Reco_pt, Reco_eta, Reco_phi, pt_reco_over_gen, Reco_emFraction, Gen_sumEt, Reco_sumEt, DeltaPhi, DeltaR, nextDeltaR, pTinnerTracker, PUweight, PUweightUp, PUweightDown;
 
  Int_t Particle_ID, whichMuon;
};

#endif
