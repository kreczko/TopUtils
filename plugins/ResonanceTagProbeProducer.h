#ifndef ResonanceTagProbeProducer_h
#define ResonanceTagProbeProducer_h

#include <memory>
#include <string>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/**
   \class   ResonanceTagProbeProducer ResonanceTagProbeProducer.h "TopAnalysis/TopUtils/plugins/ResonanceTagProbeProducer.h"

   \brief   Module to produce a collection of probe muons that have been tagged by being part of a reconstructed resonance

   This module produces a collection of probe muons that have been tagged by being part of a resonance. 
   The main use case is muons from Z->mumu decay but the module can easily be configured to work equally 
   well for J/Psi->mumu. This module can only be used with pat::Candidates. The parameters of the module 
   are:

    _tags_         : input collection of tag muons; in general the objects in this collection have to 
                     fulfill stricter requirements on object quality or isolation (edm::InputTag).

    _probes_       : input collection of probe muons; in general the objects in this collection do not
                     to fulfill that strict requirements on quality or isolation (edm::InputTag).

   _mass_          : mass of the resonance (double).

   _deltaM_        : allowed mass window (double). The window is chosen symmetric.

   All tag muons and all probe muons are looped, probe muons that are identical with the tag muon are 
   taken out from the probe loop. The identification is by reference to the original reco::Muon which 
   is stored with all pat::Candidates. This identification is 100\% unique and correct. The invariant 
   mass of the di-muon system is required to lie within a window of width +/-deltaM around the given 
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
  /// produce the tagged probe muon collection
  virtual void produce(edm::Event& evt, const edm::EventSetup& setup);

 private:
  /// input collection for tag muons
  edm::InputTag tags_;
  /// input collection for probe muons
  edm::InputTag probes_;
  /// mass parameter for resonance
  double mass_;
  /// symmetric mass window
  double deltaM_;
};

#endif
