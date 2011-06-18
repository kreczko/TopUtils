#ifndef ResonanceTagProbeProducer_h
#define ResonanceTagProbeProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "TH1.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/**
   \class   ResonanceTagProbeProducer ResonanceTagProbeProducer.h "TopAnalysis/TopUtils/plugins/ResonanceTagProbeProducer.h"

   \brief   Module to produce a collection of probe leptons that have been tagged by being part of a reconstructed resonance

   This module produces a collection of probe leptons that have been tagged by being part of a resonance. 
   The main use case is leptons from Z->ll decay but the module can easily be configured to work equally 
   well for J/Psi->mumu. This module can only be used with pat::Candidates. The parameters of the module 
   are:

    _tags_         : input collection of tag leptons; in general the objects in this collection have to 
                     fulfill stricter requirements on object quality or isolation (edm::InputTag).

    _probes_       : input collection of probe leptons; in general the objects in this collection do not
                     to fulfill that strict requirements on quality or isolation (edm::InputTag).

   _mass_          : mass of the resonance (double).

   _deltaM_        : allowed mass window (double). The window is chosen symmetric.

   All tag leptons and all probe leptons are looped, probe leptons that are identical with the tag lepton are 
   taken out from the probe loop. The identification is by reference to the original reco::Muon which 
   is stored with all pat::Candidates. This identification is 100\% unique and correct. The invariant 
   mass of the di-lepton system is required to lie within a window of width +/-deltaM around the given 
   mass. This module is specific for pat::Muons. It can easily be generalized to any pat::Candidate by
   templating.
*/

class ResonanceTagProbeProducer : public edm::EDProducer {

 public:
  /// default constructor
  explicit ResonanceTagProbeProducer(const edm::ParameterSet& cfg);
  /// default destructor
  ~ResonanceTagProbeProducer(){};
  
 private:
  /// produce the tagged probe lepton collection
  virtual void produce(edm::Event& evt, const edm::EventSetup& setup);

 private:
  /// input collection for tag leptons
  edm::InputTag tags_;
  /// input collection for probe leptons
  edm::InputTag probes_;
  /// mass parameter for resonance
  double mass_;
  /// symmetric mass window
  double deltaM_;
  /// leptonId = 11 for electron, 13 for muon
  int leptonId_;
  /// control histos
  std::map<std::string,TH1F*> hists_;
};

#endif
