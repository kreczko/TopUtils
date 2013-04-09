// -*- C++ -*-
//
// Package:    GenLevelBJetProducer
// Class:      GenLevelBJetProducer
//
/**\class GenLevelBJetProducer GenLevelBJetProducer.cc blutz/GenLevelBJetProducer/src/GenLevelBJetProducer.cc
* @brief matches GenJets versus b-hadrons
*
* Identifies the b-Jets inside the generator jets by matching it with the b-hadrons.
*
* A detailed description of the algorithm can be found at getGenJetWith()
*
* The description of the run-time parameters can be found at fillDescriptions()
*
* The description of the products can be found at GenLevelBJetProducer()
*/

//
// Original Author:  Benjamin Lutz,DESY
//   Second Author:  Nazar Bartosik,DESY
//         Created:  Thu Feb  2 13:30:58 CET 2012
// $Id: GenLevelBJetProducer.cc,v 1.14 2013/04/09 08:35:42 nbartosi Exp $
//
//


// system include files
#include <memory>
#include <algorithm>


// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

// added by me
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "AnalysisDataFormats/TopObjects/interface/TtGenEvent.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

// Nazar's testing classes
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1D.h"
#include "TH1I.h"
#include "TH2D.h"

//
// class declaration
//

class GenLevelBJetProducer : public edm::EDProducer
{
public:
    explicit GenLevelBJetProducer ( const edm::ParameterSet& );
    ~GenLevelBJetProducer();

    static void fillDescriptions ( edm::ConfigurationDescriptions& descriptions );

private:
    virtual void beginJob() ;
    virtual void produce ( edm::Event&, const edm::EventSetup& );
    virtual void endJob() ;

    virtual void beginRun ( edm::Run&, edm::EventSetup const& );
    virtual void endRun ( edm::Run&, edm::EventSetup const& );
    virtual void beginLuminosityBlock ( edm::LuminosityBlock&, edm::EventSetup const& );
    virtual void endLuminosityBlock ( edm::LuminosityBlock&, edm::EventSetup const& );

    std::vector<int> findHadronJets ( const reco::GenJetCollection& genJets,  std::vector<int> &hadIndex, std::vector<reco::GenParticle> &hadMothersGenPart, std::vector<std::vector<int> > &hadMothersIndices, std::vector<int> &hadFlavour );
    std::vector<int> getGenJetWith ( const reco::Candidate* bQuark, const reco::GenJetCollection& genJets, std::vector<reco::GenParticle> &bHadronObjects, std::vector<bool> &isFromBquark, std::vector<int> &bHadronsInJet );
    int getGenJetNear ( const reco::Candidate* particle, std::vector<const reco::GenJet*> &genJets );
    typedef const reco::Candidate* pCRC;
    int analyzeMothers ( const reco::Candidate* thisParticle, pCRC *hadron, std::vector<const reco::Candidate*> &hadMothers, std::vector<std::vector<int> > &hadMothersIndices, std::set<const reco::Candidate*> *analyzedParticles, const int prevPartIndex );
    bool searchInMothers ( const reco::Candidate* bQuark, const reco::Candidate* thisParticle, std::vector<const reco::Candidate*> particleChain, pCRC *bHadron );
    bool putMotherIndex ( std::vector<std::vector<int> > &hadMothersIndices, int partIndex, int mothIndex );
    bool isHadron ( const int flavour, const reco::Candidate* thisParticle );
    bool hasHadronDaughter ( const int flavour, const reco::Candidate* thisParticle );
    int isInList ( std::vector<const reco::Candidate*> particleList, const reco::Candidate* particle );
    void findInMothers ( int idx, std::vector<int> &mothChains, std::vector<std::vector<int> > &hadMothersIndices, std::vector<reco::GenParticle> &hadMothers, int status, int pdgId, bool pdgAbs, int firstLast, bool verbose );
    bool isNeutralPdg ( int pdgId );

    bool checkForLoop ( std::vector<const reco::Candidate*> &particleChain, const reco::Candidate* particle );
    std::string getParticleName ( int id ) const;

    inline std::string printJetInfo ( const size_t iJet, const reco::GenJet* ) const;
    inline std::string printParticleChain ( const std::vector<const reco::Candidate*> &particleChain, const reco::Candidate *bHadron ) const;

// ----------member data ---------------------------
    edm::InputTag ttGenEvent_;
    edm::InputTag genJets_;
    double deltaR_;
    int flavour_;
    bool noBBbarResonances_;
    bool requireTopBquark_;
    bool resolveName_;
    bool doImprovedHadronMatching_;
    bool doValidationPlotsForImprovedHadronMatching_;
    bool toPlot;

    std::string flavourStr_;  // Name of the flavour specified in config file

    edm::ESHandle<ParticleDataTable> pdt_;


// -- testing histograms
    TH1I *h_nHad, *h_hadJetIndex;
    TH2D *h2_Q2MothPdg, *h2_Q2Moth_2Pdg;
    TH1D *h_jetdPt, *h_jetDeltaPt;
    TH2D *h2_QQMothSamePdg;
    TH1D *h_QQMothEFrac, *h_QHQPtFrac, *h_QQMothdR, *h_QQMothdR_min, *h_QHQdR;
    TH1D *h_QQMothEFrac_ambig, *h_QHQPtFrac_ambig, *h_QQMothdR_ambig, *h_QQMothdR_min_ambig, *h_QHQdR_ambig;
    TH1D *h_jetHad_dR, *h_jetHadOld_dR, *h_jetHadOldNoMatch_dR, *h_jetHad_dR_noFlav, *h_jetHadOldNoMatch_dR_noFlav;

    TH1I *h_nFirstQ,*h_firstQstatus, *h_firstQPdg;
    TH2D *h2_firstQFlav_HadFlav, *h2_firstQFlav_QMotherFlav;
    TH1I *h_nLastQ,*h_lastQstatus, *h_lastQPdg, *h_nHadTop, *h_nHadHiggs, *h_nHadGluon, *h_nHadZ, *h_nHadNo;
    TH2D *h2_lastQFlav_HadFlav, *h2_lastQFlav_QMotherFlav, *h2_lastQPdg_QMotherPdg;
    TH1D *h_lastQ_lastQ_dRmin, *h_hadJet_dRmin;
    TH1D *h_dEHadJet_T, *h_dEHadJet_H, *h_dEHadJet_Z, *h_dEHadJet_G, *h_dEHadJet_P, *h_dEHadJet_Q, *h_dEHadJet_Other, *h_dEHadJet[7];
    TH1I *h_nHadInJet;


};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
/**
* @brief constructor initialising producer products and config parameters
*
* Output generated by this producer:
* <TABLE>
* <TR><TH> name                </TH><TH> type                           </TH><TH> description </TH> </TR>
* <TR><TD> BHadJetIndex        </TD><TD> std::vector<int>               </TD><TD> position of the jet identified as b-hadron jet in the input GenJetCollection </TD></TR>
* <TR><TD> BHadrons            </TD><TD> std::vector<reco::GenParticle> </TD><TD> vector of the identified b-hadrons (definition of b-hadron and anti-b-hadron below) </TD></TR>
* <TR><TD> BHadronFromTopB     </TD><TD> std::vector<bool>              </TD><TD> true if the corresponding b-hadron originates from the ttGenEvent b-quark </TD></TR>
* <TR><TD> BHadronVsJet        </TD><TD> std::vector<int>               </TD><TD> matrix of which b-hadron appears in which GenJet, access by [iJet * BHadrons.size() + iBhadron] </TD></TR>
*
* <TR><TD> AntiBHadJetIndex    </TD><TD> std::vector<int>               </TD><TD> position of the jet identified as anti-b-hadron jet in the input GenJetCollection </TD></TR>
* <TR><TD> AntiBHadrons        </TD><TD> std::vector<reco::GenParticle> </TD><TD> vector of the identified anti-b-hadrons (definition of b-hadron and anti-b-hadron below) </TD></TR>
* <TR><TD> AntiBHadronFromTopB </TD><TD> std::vector<bool>              </TD><TD> true if the corresponding anti-b-hadron originates from the ttGenEvent anti-b-quark </TD></TR>
* <TR><TD> AntiBHadronVsJet    </TD><TD> std::vector<int>               </TD><TD> matrix of which anti-b-hadron appears in which GenJet, access by [iJet * AntiBHadrons.size() + iBhadron] </TD></TR>
*
* </TABLE>
*
* @warning Definition of b-hadron and anti-b-hadron: The term b-hadron and anti-b-hadron is in reference to the quark content and <b>not</b> to distinguish particles from anti-particles.
* Here a b-hadron contains a b-quark and an anti-b-hadron contains an anti-b-quark.
* For mesons this means an inversion with respect to the PDG definition, as mesons actually contain anti-b-quarks and anti-mesons contain b-quarks.
*
*/
GenLevelBJetProducer::GenLevelBJetProducer ( const edm::ParameterSet& cfg )
{

    ttGenEvent_        = cfg.getParameter<edm::InputTag> ( "ttGenEvent" );
    genJets_           = cfg.getParameter<edm::InputTag> ( "genJets" );
    deltaR_            = cfg.getParameter<double> ( "deltaR" );
    flavour_           = cfg.getParameter<int> ( "flavour" );
    noBBbarResonances_ = cfg.getParameter<bool> ( "noBBbarResonances" );
    requireTopBquark_  = cfg.getParameter<bool> ( "requireTopBquark" );
    resolveName_       = cfg.getParameter<bool> ( "resolveParticleName" );
    doImprovedHadronMatching_ = cfg.getParameter<bool> ( "doImprovedHadronMatching" );
    doValidationPlotsForImprovedHadronMatching_ = cfg.getParameter<bool> ( "doValidationPlotsForImprovedHadronMatching" );

    if ( !doImprovedHadronMatching_ ) {
        doValidationPlotsForImprovedHadronMatching_=false;
    }
    toPlot = doValidationPlotsForImprovedHadronMatching_;


    flavour_ = abs ( flavour_ ); // Make flavour independent of sign given in configuration
    if ( flavour_==5 ) {
        flavourStr_="B";
    } else if ( flavour_==4 ) {
        flavourStr_="C";
    } else {
        edm::LogError ( "GenLevelBJetProducer" ) << "Flavour option must be 4 (c-jet) or 5 (b-jet), but is: " << flavour_ << ". Correct this!";
    }

    produces< std::vector<int> > ( "BHadJetIndex" );
    produces< std::vector<reco::GenParticle> > ( "BHadrons" );
    produces< std::vector<bool> > ( "BHadronFromTopB" );
    produces< std::vector<int> > ( "BHadronVsJet" );

    produces< std::vector<int> > ( "AntiBHadJetIndex" );
    produces< std::vector<reco::GenParticle> > ( "AntiBHadrons" );
    produces< std::vector<bool> > ( "AntiBHadronFromTopB" );
    produces< std::vector<int> > ( "AntiBHadronVsJet" );

    if ( !doImprovedHadronMatching_ ) {
        return;
    }

// Hadron matching variables
    produces< std::vector<reco::GenParticle> > ( "gen"+flavourStr_+"HadPlusMothers" ); // All mothers in all decay chains above any hadron of specified flavour
    produces< std::vector< std::vector<int> > > ( "gen"+flavourStr_+"HadPlusMothersIndices" ); // Indices of mothers of each hadMother
    produces< std::vector<int> > ( "gen"+flavourStr_+"HadIndex" ); // Index of hadron in the vector of hadMothers
    produces< std::vector<int> > ( "gen"+flavourStr_+"HadFlavour" ); // PdgId of the first non-b(c) quark mother with sign corresponding to hadron charge
    produces< std::vector<int> > ( "gen"+flavourStr_+"HadJetIndex" ); // Index of genJet matched to each hadron by jet clustering algorithm

    if ( !doValidationPlotsForImprovedHadronMatching_ ) {
        return;
    }

// Initializing ROOT file for storing histograms
    edm::Service<TFileService> fs;

    h_nHad = fs->make<TH1I> ( "nHad", "Nr. of hadrons;N", 10, 0, 10 );
    h_hadJetIndex = fs->make<TH1I> ( "hadJetIndex", "Index of hadron jet;index;hadrons", 102, -2, 100 );
    h_jetdPt = fs->make<TH1D> ( "jetdPt", "#DeltaPt(Clust/dR);#DeltaPt(Clust/dR)", 50, 0, 3 );
    h_jetDeltaPt = fs->make<TH1D> ( "jetDeltaPt", "#DeltaPt(Clust-dR);#DeltaPt [GeV]", 50, -100, 100 );

    h2_QQMothSamePdg = fs->make<TH2D> ( "QQMothSamePdg", "PdgId of quark and mother quark with same abs(pdgId);Daughter PdgId;Mother PdgId", 14, -7, 7,14,-7,7 );
    h_QQMothEFrac = fs->make<TH1D> ( "QQMothEFrac", "E fraction of daughter;#frac{E}{E^{mother}}", 50, 0., 10.0 );
    h_QHQPtFrac = fs->make<TH1D> ( "QHQPtFrac", "Pt fraction of highest Pt daughter;#frac{Pt^{highest}-Pt}{Pt^{highest}}", 50, 0., 1.01 );
    h_QQMothdR = fs->make<TH1D> ( "QQMothdR", "dR between mother and same pdgId daughters;dR", 100, 0.0, 5.0 );
    h_QQMothdR_min = fs->make<TH1D> ( "QQMothdR_min", "Min. dR between mother and same pdgId daughters;dR", 100, 0.0, 5.0 );
    h_QHQdR = fs->make<TH1D> ( "QHQdR", "dR between mother and highest Pt daughter;dR", 100, 0.0, 5.0 );

    h_QQMothEFrac_ambig = fs->make<TH1D> ( "QQMothEFrac_ambig", "E fraction of daughter;#frac{E}{E^{mother}}", 50, 0., 10.0 );
    h_QHQPtFrac_ambig = fs->make<TH1D> ( "QHQPtFrac_ambig", "Pt fraction of highest Pt daughter;#frac{Pt^{highest}-Pt}{Pt^{highest}}", 50, 0., 1.01 );
    h_QQMothdR_ambig = fs->make<TH1D> ( "QQMothdR_ambig", "dR between mother and same pdgId daughters;dR", 100, 0.0, 5.0 );
    h_QQMothdR_min_ambig = fs->make<TH1D> ( "QQMothdR_min_ambig", "Min. dR between mother and same pdgId daughters;dR", 100, 0.0, 5.0 );
    h_QHQdR_ambig = fs->make<TH1D> ( "QHQdR_ambig", "dR between mother and highest Pt daughter;dR", 100, 0.0, 5.0 );

    h_jetHad_dR = fs->make<TH1D> ( "jetHad_dR", "dR between hadron and jet matched by jet clustering algo;dR_{hadron}^{jet}", 50, 0, 4 );
    h_jetHadOld_dR = fs->make<TH1D> ( "jetHadOld_dR", "dR between hadron and jet matched by decay products;dR_{hadron}^{jet}", 50, 0, 4 );
    h_jetHadOldNoMatch_dR = fs->make<TH1D> ( "jetHadOldNoMatch_dR", "dR between hadron and jet matched by decay products;dR_{hadron}^{jet}", 50, 0, 4 );
    h_jetHad_dR_noFlav = fs->make<TH1D> ( "jetHad_dR_noFlav", "dR between hadron and jet matched by jet clustering algo;dR_{hadron}^{jet}", 50, 0, 4 );
    h_jetHadOldNoMatch_dR_noFlav = fs->make<TH1D> ( "jetHadOldNoMatch_dR_noFlav", "dR between hadron and jet matched by decay products;dR_{hadron}^{jet}", 50, 0, 4 );

    h_hadJet_dRmin = fs->make<TH1D> ( "hadJet_dRmin", "Min dR between hadron and any jet;dR_{hadron}^{jet}", 50, 0, 4 );

    h_nFirstQ = fs->make<TH1I> ( "nFirstQ", "Number of the 1-st quarks;N", 10, 0, 10 );
    h_firstQstatus = fs->make<TH1I> ( "firstQstatus", "Status of the 1-st quark;status", 5, 0, 5 );
    h_firstQPdg = fs->make<TH1I> ( "firstQPdg", "PdgId of the 1-st quark;PdgId", 60, -30, 30 );
    h2_firstQFlav_HadFlav = fs->make<TH2D> ( "firstQFlav_HadFlav", "Charge of the 1-st quark and hadron;Hadron charge;Quark charge", 5, -2, 3, 5,-2,3 );
    h2_firstQFlav_QMotherFlav = fs->make<TH2D> ( "firstQMotherFlav_QMotherFlav", "Charge of the 1-st quark and its mother;Quark charge;Quark's mother charge", 5, -2, 3, 5, -2, 3 );
    h_nLastQ = fs->make<TH1I> ( "nLastQ", "Number of the last quarks;N", 10, 0, 10 );
    h_lastQstatus = fs->make<TH1I> ( "lastQstatus", "Status of the last quark;status", 5, 0, 5 );
    h_lastQPdg = fs->make<TH1I> ( "lastQPdg", "PdgId of the last quark;PdgId", 60, -30, 30 );
    h2_lastQFlav_HadFlav = fs->make<TH2D> ( "lastQFlav_HadFlav", "Charge of the last quark and hadron;Hadron charge;Quark charge", 5, -2, 2,5,-2,2 );
    h2_lastQFlav_QMotherFlav = fs->make<TH2D> ( "lastQFlav_QMotherFlav", "Charge of the last quark and its mother;Quark charge;Quark's mother charge", 5, -2, 3,5,-2,3 );
    h2_lastQPdg_QMotherPdg = fs->make<TH2D> ( "lastQPdg_QMotherPdg", "PdgId of the last quark and its mother;Quark pdgId;Quark's mother pdgId", 13, -6, 7,35,-8,27 );
    h_lastQ_lastQ_dRmin = fs->make<TH1D> ( "lastQ_lastQ_dRmin", "dR_{min} between hadron and closest last quark;dR_{hadron}^{Quark}", 50, 0, 4 );

    h_nHadTop = fs->make<TH1I> ( "nHadTop", "Nr. of hadrons from Top;N", 10, 0, 10 );
    h_nHadHiggs = fs->make<TH1I> ( "nHadHiggs", "Nr. of hadrons from Higgs;N", 10, 0, 10 );
    h_nHadGluon = fs->make<TH1I> ( "nHadGluon", "Nr. of hadrons from Gluon;N", 10, 0, 10 );
    h_nHadZ = fs->make<TH1I> ( "nHadZ", "Nr. of hadrons from Z;N", 10, 0, 10 );
    h_nHadNo = fs->make<TH1I> ( "nHadNo", "Nr. of hadrons without flavour;N", 10, 0, 10 );

    h_nHadInJet = fs->make<TH1I> ( "nHadInJet", "N hadrons associated to a jet;N hadrons;Jets", 10, 0, 10 );
}

GenLevelBJetProducer::~GenLevelBJetProducer()
{
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
/**
* @brief description of the run-time parameters
*
* <TABLE>
* <TR><TH> name                </TH><TH> description </TH> </TR>
* <TR><TD> ttGenEvent          </TD><TD> input collection of TtGenEvent, used to identify the b-quark from top </TD></TR>
* <TR><TD> genJets             </TD><TD> input GenJet collection </TD></TR>
* <TR><TD> deltaR              </TD><TD> maximum angle between hadron and GenJet </TD></TR>
* <TR><TD> noBBbarResonances   </TD><TD> exclude resonances to be identified as hadrons </TD></TR>
* <TR><TD> requireTopBquark    </TD><TD> only accept hadrons coming from the top-quark </TD></TR>
* <TR><TD> resolveParticleName </TD><TD> print particle name during warning and debug output instead of PDG ID </TD></TR>
* <TR><TD> flavour		</TD><TD> flavour of weakly decaying hadron that the jets should be matched to (5-b, 4-c) </TD></TR>
* </TABLE>
*
*/
void GenLevelBJetProducer::fillDescriptions ( edm::ConfigurationDescriptions& descriptions )
{

    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag> ( "ttGenEvent",edm::InputTag ( "genEvt" ) )->setComment ( "Input collection of TtGenEvent, used to identify the quark from top" );
    desc.add<edm::InputTag> ( "genJets",edm::InputTag ( "ak5GenJets","","HLT" ) )->setComment ( "Input GenJet collection" );
    desc.add<double> ( "deltaR",.5 )->setComment ( "Maximum angle between hadron and GenJet" );
    desc.add<bool> ( "noBBbarResonances",true )->setComment ( "Exclude resonances to be identified as hadrons" );
    desc.add<bool> ( "requireTopBquark",false )->setComment ( "Only accept hadrons coming from the top-quark" );
    desc.add<bool> ( "resolveParticleName",false )->setComment ( "Print particle name during warning and debug output instead of PDG ID" );
    desc.add<int> ( "flavour",5 )->setComment ( "Flavour of weakly decaying hadron that should be added to the jet for futher tagging. (4-c, 5-b)" );
    desc.add<bool> ( "doImprovedHadronMatching",false )->setComment ( "Whether to store improved information about bHadron-bQuark-Jet matching." );
    desc.add<bool> ( "doValidationPlotsForImprovedHadronMatching",false )->setComment ( "Whether to store validation plots for improved matching." );
    descriptions.add ( "produceGenLevelBJets",desc );
}



//
// member functions
//

// ------------ method called to produce the data  ------------
void GenLevelBJetProducer::produce ( edm::Event& evt, const edm::EventSetup& setup )
{

    setup.getData ( pdt_ );

    using namespace edm;

    std::auto_ptr<std::vector<int> > bIdx ( new std::vector<int> );
    std::auto_ptr<std::vector<int> > anti_bIdx ( new std::vector<int> );
    std::auto_ptr<std::vector<int> > bHadronVsJet ( new std::vector<int> );
    std::auto_ptr<std::vector<int> > anti_bHadronVsJet ( new std::vector<int> );
    std::auto_ptr<std::vector<reco::GenParticle> > bHadrons ( new std::vector<reco::GenParticle> );
    std::auto_ptr<std::vector<reco::GenParticle> > anti_bHadrons ( new std::vector<reco::GenParticle> );
    std::auto_ptr<std::vector<bool> > bHadronFromTopBqark ( new std::vector<bool> );
    std::auto_ptr<std::vector<bool> > anti_bHadronFromTopBqark ( new std::vector<bool> );

    edm::Handle<TtGenEvent> genEvt;
    evt.getByLabel ( ttGenEvent_, genEvt );

    edm::Handle<reco::GenJetCollection> genJets;
    evt.getByLabel ( genJets_, genJets );

    LogDebug ( "bJet" ) << "searching for b-jets in " << genJets_;
    *bIdx = getGenJetWith ( genEvt->b(), *genJets, *bHadrons, *bHadronFromTopBqark, *bHadronVsJet );
    LogDebug ( "bJet" ) << "searching for anti-b-jets in " << genJets_;
    *anti_bIdx = getGenJetWith ( genEvt->bBar(), *genJets, *anti_bHadrons, *anti_bHadronFromTopBqark, *anti_bHadronVsJet );

    evt.put ( bIdx,                "BHadJetIndex" );
    evt.put ( bHadrons,            "BHadrons" );
    evt.put ( bHadronFromTopBqark, "BHadronFromTopB" );
    evt.put ( bHadronVsJet,        "BHadronVsJet" );

    evt.put ( anti_bIdx,                "AntiBHadJetIndex" );
    evt.put ( anti_bHadrons,            "AntiBHadrons" );
    evt.put ( anti_bHadronFromTopBqark, "AntiBHadronFromTopB" );
    evt.put ( anti_bHadronVsJet,        "AntiBHadronVsJet" );

    if ( !doImprovedHadronMatching_ ) {
        return;
    }

// Additional information -------------------------->>>>>>>>>>>

// Hadron matching variables
    std::auto_ptr<std::vector<reco::GenParticle> > hadMothers ( new std::vector<reco::GenParticle> );
    std::auto_ptr<std::vector<std::vector<int> > > hadMothersIndices ( new std::vector<std::vector<int> > );
    std::auto_ptr<std::vector<int> > hadIndex ( new std::vector<int> );
    std::auto_ptr<std::vector<int> > hadFlavour ( new std::vector<int> );
    std::auto_ptr<std::vector<int> > hadJetIndex ( new std::vector<int> );

    LogDebug ( flavourStr_+"Jet (new)" ) << "searching for "<< flavourStr_ <<"-jets in " << genJets_;
    *hadJetIndex = findHadronJets ( *genJets, *hadIndex, *hadMothers, *hadMothersIndices, *hadFlavour );

// Hadron matching variables
    evt.put ( hadMothers,         "gen"+flavourStr_+"HadPlusMothers" );
    evt.put ( hadMothersIndices,  "gen"+flavourStr_+"HadPlusMothersIndices" );
    evt.put ( hadIndex,           "gen"+flavourStr_+"HadIndex" );
    evt.put ( hadFlavour,         "gen"+flavourStr_+"HadFlavour" );
    evt.put ( hadJetIndex,        "gen"+flavourStr_+"HadJetIndex" );
}

// ------------ method called once each job just before starting event loop  ------------
void GenLevelBJetProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void GenLevelBJetProducer::endJob()
{
}

// ------------ method called when starting to processes a run  ------------
void GenLevelBJetProducer::beginRun ( edm::Run&, edm::EventSetup const& )
{
}

// ------------ method called when ending the processing of a run  ------------
void
GenLevelBJetProducer::endRun ( edm::Run&, edm::EventSetup const& )
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void GenLevelBJetProducer::beginLuminosityBlock ( edm::LuminosityBlock&, edm::EventSetup const& )
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void GenLevelBJetProducer::endLuminosityBlock ( edm::LuminosityBlock&, edm::EventSetup const& )
{
}

// ------------ helper functions -------------

/**
* @brief identify the jets that contain b-hadrons
*
* All jets originating from a b-hadron with the right b content (b or anti-b) are identified in the GenJetCollection.
* This is done by searching through the generator particle decay tree starting from the constituents of the generator jets.
* The b-hadron which is lowest in the decay-chain (just before the weak decay of the b) is considered the b-hadron of this jet.
* Only jets within the given @f$\delta_{r}@f$ to the b-hadron are accepted.
* In case more than one jet originates from the same b-hadron, the jet which is nearer in @f$\delta_{r}@f$ is chosen.
*
* b-bbar resonances can either be considered b-hadrons or not depending on the configuration.
*
* The b-hadrons can be required to originate from the given b-quark depending on the configuration.
*
* To allow performance studies following informations are tracked and returned:
* \arg the GenParticles identified as b-hadrons
* \arg if the b-hadron originates from the top-b-quark
* \arg a matrix showing which jets contain particles from which b-hadron
*
* @param[in] bQuark used to set the charge of the b-Hadrons
* @param[in] genJets the GenJetCollection to be searched
* @param[out] bHadronObjects list of identified b-hadrons
* @param[out] isFromBquark tells which bHadrons originate from the given b-quark
* @param[out] bHadronsInJet relation between b-hadron and genJets. 0 => no daughters of bHadron in this jet, 1 => daughters of this bHadron in this jet. Access (iJet,iBhadron) -> [iJet * bHadronObject.size() + iBhadron]
* @returns a list with the indicies of the GenJets that fulfill the requirements
*/
std::vector<int> GenLevelBJetProducer::getGenJetWith ( const reco::Candidate* bQuark, const reco::GenJetCollection& genJets, std::vector<reco::GenParticle> &bHadronObjects, std::vector<bool> &isFromBquark, std::vector<int> &bHadronsInJet )
{

    std::vector<int> result;
    if ( ! bQuark ) {
        return result;    //no b-quark given, so so b-jets
    }

    std::vector<const reco::GenJet*> bQuarkCandidates;
    std::vector<const reco::GenJet*> bHadronCandidates;
    std::vector<size_t> bQuarkCandidateIndex;
    std::vector<size_t> bHadronCandidateIndex;
    std::vector<unsigned int> nBquarkParticles;
    std::vector<unsigned int> nBhadronParticles;
    std::vector<const reco::Candidate*> motherBhadron;

    typedef std::map<double, const reco::Candidate*> bHadronMap_t;
    bHadronMap_t bHadronList;
    typedef std::map<double, bool> bHadronFlagMap_t;
    bHadronFlagMap_t bHadronFlags;
    typedef std::map<double, std::vector<size_t> > jetCandidatesVsBhadron_t;
    jetCandidatesVsBhadron_t jetCandidatesVsBhadron;

    for ( size_t iJet = 0; iJet < genJets.size(); ++iJet ) {
        const reco::GenJet* thisJet = & ( genJets[iJet] );
        std::vector<const reco::GenParticle*> particles = thisJet->getGenConstituents();


        LogTrace ( "Jets" ) << printJetInfo ( iJet, thisJet );

        bool isBquarkCandidate = false;
        bool isBhadronCandidate = false;
        unsigned int nBquarkDaughters = 0;
        unsigned int nBhadronDaughters = 0;
        bHadronMap_t bHadrons;

        for ( unsigned int iParticle = 0; iParticle < particles.size(); ++iParticle ) {
            const reco::GenParticle* thisParticle = particles[iParticle];
            const reco::Candidate* bHadron = 0;
            std::vector<const reco::Candidate*> particleChain;
            checkForLoop ( particleChain,thisParticle );
            if ( searchInMothers ( bQuark, thisParticle, particleChain, &bHadron ) ) {
                isBquarkCandidate = true;
                ++nBquarkDaughters;
                if ( bHadron ) {
                    bHadronFlags[bHadron->p()] = true;
                }
            }
            if ( bHadron ) {
                isBhadronCandidate = true;
                ++nBhadronDaughters;
                bHadrons[bHadron->p()] = bHadron;
                bHadronList[bHadron->p()] = bHadron;
            }
        }
        if ( isBquarkCandidate ) {
            bQuarkCandidates.push_back ( thisJet );
            bQuarkCandidateIndex.push_back ( iJet );
            nBquarkParticles.push_back ( nBquarkDaughters );
        }
        if ( isBhadronCandidate && ( isBquarkCandidate || !requireTopBquark_ ) ) {
            bHadronCandidates.push_back ( thisJet );
            bHadronCandidateIndex.push_back ( iJet );
            nBhadronParticles.push_back ( nBhadronDaughters );
            motherBhadron.push_back ( bHadrons.begin()->second );
            for ( bHadronMap_t::iterator it = bHadrons.begin(); it != bHadrons.end(); ++it ) {
                if ( it->second->pt() > motherBhadron.back()->pt() ) {
                    motherBhadron.back() = it->second;    // this is only for debug output and not the real decision
                }
                jetCandidatesVsBhadron[it->first].push_back ( iJet );
            }
        }
    }

// debug output no functionality
#ifdef EDM_ML_DEBUG
    LogTrace ( "bQuark" )  << "found " << bQuarkCandidates.size() << " jets with b-quark particles" << std::endl;
    LogTrace ( "bHadron" ) << "found " << bHadronCandidates.size() << " jets with b-hadron particles" << std::endl
                           << "found " << bHadronList.size() << " different b-Hadrons" << std::endl;

    for ( unsigned int i = 0; i < bQuarkCandidates.size(); ++i ) {
        const reco::GenJet *jet = bQuarkCandidates.at ( i );
        size_t index = bQuarkCandidateIndex.at ( i );
        LogTrace ( "bQuark" ) << "GenJet " << index << " has " << nBquarkParticles.at ( i ) << "/" << jet->getGenConstituents().size() << " b-quark daughters" << std::endl
                              << "GenJet " << index << " " << jet->p4() << " p: " << jet->p() << " p_T: " << jet->pt() << std::endl
                              << "GenJet " << index << " eta: " << jet->eta() << " phi: " << jet->phi() << std::endl;
    }


    for ( unsigned int i = 0; i < bHadronCandidates.size(); ++i ) {
        const reco::GenJet *jet = bHadronCandidates.at ( i );
        size_t index = bHadronCandidateIndex.at ( i );
        const reco::Candidate *bMother = motherBhadron.at ( i );
        LogTrace ( "bHadron" ) << "GenJet " << index << " has " << nBhadronParticles.at ( i ) << "/" << jet->getGenConstituents().size() << " b-hadron daughters" << std::endl
                               << "GenJet " << index << " " << jet->p4() << " p: " << jet->p() << " p_T: " << jet->pt() << std::endl
                               << "GenJet " << index << " eta: " << jet->eta() << " phi: " << jet->phi() << std::endl
                               << "GenJet " << index << " mother b-hadron: " << getParticleName ( bMother->pdgId() ) <<  std::endl
                               << "GenJet " << index << " mother b-hadron: " << bMother->p4() << " p: " << bMother->p() << " p_T: " << bMother->pt() << std::endl
                               << "GenJet " << index << " mother b-hadron: eta: " << bMother->eta() << " phi: " << bMother->phi() << std::endl;
    }
#endif // EDM_ML_DEBUG -- debug output


    bHadronObjects.clear();
    isFromBquark.clear();
    bHadronsInJet.clear();

    unsigned int nJets = genJets.size();
    unsigned int nBhadrons = bHadronList.size();

    bHadronsInJet.resize ( nJets*nBhadrons,0 );

    for ( jetCandidatesVsBhadron_t::iterator it = jetCandidatesVsBhadron.begin(); it != jetCandidatesVsBhadron.end(); ++it ) {

        const reco::Candidate *bHadron = bHadronList[it->first];
        std::vector<size_t> candidateIdx = it->second;
        LogTrace ( "bHadron" ) << "analysing jets for " << getParticleName ( bHadron->pdgId() ) << std::endl;
        LogTrace ( "bHadron" ) << "   " << candidateIdx.size() <<  " candidate jets: ";

        unsigned int bHadronIndex = bHadronObjects.size();
        bHadronObjects.push_back ( * ( dynamic_cast<const reco::GenParticle*> ( bHadron ) ) );
        isFromBquark.push_back ( bHadronFlags[it->first] );

        std::vector<const reco::GenJet*> jetCandidates;
        for ( unsigned int idx = 0; idx < candidateIdx.size(); ++idx ) {
            LogTrace ( "bHadron" ) << "     " << candidateIdx[idx] << std::endl;
            jetCandidates.push_back ( & ( genJets[candidateIdx[idx]] ) );
            bHadronsInJet.at ( candidateIdx[idx]*nBhadrons + bHadronIndex ) = 1;
        }

        int bestJet = getGenJetNear ( bHadron, jetCandidates );
        if ( bestJet >= 0 ) {
            result.push_back ( candidateIdx.at ( bestJet ) );
            LogTrace ( "bHadron" ) << "    best matching jet: " << candidateIdx.at ( bestJet ) << std::endl;
        } else {
            LogTrace ( "bHadron" ) << "    no matching jet in cone of " << deltaR_ << std::endl;
        }
    }

    std::sort ( result.begin(),result.end() );

    return result;
}

/**
* @brief do a recursive search for the mother particles until the b-quark is found or the absolute mother is found
*
* the treatment of b-bar resonances depends on the global parameter noBBbarResonances_
*
* @param[in] bQuark ancestor b-quark for the current search
* @param[in] thisParticle current particle to be analysed
* @param[in] particleChain current version of the search path
* @param[out] bHadron the oldest b-hadron found in the search
*
* @returns if the b-quark was found or not
*/
bool GenLevelBJetProducer::searchInMothers ( const reco::Candidate* bQuark, const reco::Candidate* thisParticle, std::vector<const reco::Candidate*> particleChain, pCRC *bHadron )
{

    if ( *bHadron == 0 // find only the first b-hadron on the way (the one that decays weekly)
            && ( thisParticle->pdgId() / 1000  == bQuark->pdgId()  // b-baryions
                 || ( thisParticle->pdgId() / 100 % 10 == -bQuark->pdgId() // b-mesons
                      && ! ( noBBbarResonances_ && thisParticle->pdgId() / 10 % 100 == 55 ) // but not a b-bbar resonance
                    ) )
       ) {
        *bHadron = thisParticle;
    }

    for ( size_t iMother = 0; iMother < thisParticle->numberOfMothers(); ++iMother ) {
        const reco::Candidate* mother = thisParticle->mother ( iMother );
        if ( checkForLoop ( particleChain, mother ) ) {
            edm::LogWarning ( "decayChain" ) << "Identified a loop in the current decay chain."
                                             << " Paricle " << mother
                                             << " PDG: " << mother->pdgId()
//       << " " << mother->p4()
                                             << " pt: " << mother->pt() << ", eta: " << mother->eta()
                                             << " is already in the chain."
                                             << " Will stop this branch here. B-hadron identification might be incomplete."
                                             << " " << printParticleChain ( particleChain,*bHadron );
            return false;
        }
        if ( bQuark->p4() == mother->p4() && bQuark->pdgId() == mother->pdgId() && bQuark->status() == mother->status() ) {
            LogTrace ( "decayChain" ) << printParticleChain ( particleChain,*bHadron );
            return true;
        } else if ( searchInMothers ( bQuark, mother, particleChain, bHadron ) ) {
            return true;
        }
    }

    return false;
}

/**
* @brief identify the jets that contain b-hadrons
*
* All jets originating from a b-hadron with the right b (c) content (b or anti-b) are identified in the GenJetCollection.
* The b (c) jet is identified by searching for a hadron of corresponding flavour in the jet. Hadron are put in jets
* by "TopAnalysis.TopUtils.GenJetParticles" plugin.
* For each hadron all mothers from all levels and chains are analyzed to find the quark or gluon from which the hadron has originated.
* This is done by searching through the generator particle decay tree starting from the hadron, performing checks for flavour and kinematic consistency.
* The hadrons that are not last in the decay chain (don't decay weakly) are skipped.
*
* b-bbar (c-cbar) resonances can either be considered as hadrons or not depending on the configuration.
*
*
* To allow performance studies following informations are tracked and returned:
* \arg the GenParticles identified as b-hadrons
* \arg if the b-hadron originates from the top-b-quark
* \arg a matrix showing which jets contain particles from which b-hadron (remnant from original approach in TopAnalysis)
*
* @param[in] genJets the GenJetCollection to be searched
* @param[out] hadIndex vector of indices of found hadrons in hadMothers
* @param[out] hadMothers vector of all mothers at all levels of each found hadron
* @param[out] hadMothersIndices connection between each particle from hadMothers and its mothers
* @param[out] hadFlavour flavour of each found hadron
* @returns vector of jets being matched to each hadron by jet clustering algorithm
*/
std::vector<int> GenLevelBJetProducer::findHadronJets ( const reco::GenJetCollection& genJets, std::vector<int> &hadIndex, std::vector<reco::GenParticle> &hadMothers, std::vector<std::vector<int> > &hadMothersIndices, std::vector<int> &hadFlavour )
{

    std::vector<int> result;
    std::vector<const reco::Candidate*> hadMothersCand;

    const unsigned int nJets = genJets.size();
    bool hadVsJet[100][400]= {{false}};				// Whether jet contains decay products of the hadron (max 100 hadrons, 250 jets foreseen)


    for ( size_t iJet = 0; iJet < nJets; ++iJet )  {
        const reco::GenJet* thisJet = & ( genJets[iJet] );
        std::vector<const reco::GenParticle*> particles = thisJet->getGenConstituents();

//     printf(" genJet: %d pt: %.4f\teta: %.4f\tphi: %.4f\n",(int)iJet,thisJet->pt(),thisJet->eta(),thisJet->phi());
        for ( unsigned int iParticle = 0; iParticle < particles.size(); ++iParticle ) {
            const reco::GenParticle* thisParticle = particles[iParticle];
            const reco::Candidate* hadron = 0;
            if ( thisParticle->status() >1 ) {
                continue;    // Excluding non-final state particles (e.g. bHadrons)
            }
            int hadronIndex = analyzeMothers ( thisParticle, &hadron, hadMothersCand, hadMothersIndices, 0, -1 );
            if ( hadron ) { // Putting hadron index to the list if it is not yet
                int hadListIndex=-1;
                for ( unsigned int hadNum=0; hadNum<hadIndex.size(); hadNum++ ) {
                    if ( hadIndex[hadNum]!=hadronIndex ) {
                        continue;
                    }
                    hadListIndex=hadNum;
                    break;
                }
                if ( hadListIndex<0 ) {
                    hadIndex.push_back ( hadronIndex );
                    hadListIndex=hadIndex.size();
                }
                hadVsJet[hadListIndex][iJet]=true;		// Setting current jet as containing decay products of the found hadron
            }
        }   // End of loop over jet consituents
    }   // End of loop over jets

    for ( int i=0; i< ( int ) hadMothersCand.size(); i++ ) {
        hadMothers.push_back ( ( *dynamic_cast<const reco::GenParticle*> ( hadMothersCand[i] ) ) );
    }

// Checking mothers of hadrons in order to assign flags (where the hadron comes from)
    unsigned int nHad = hadIndex.size();

// Checking number of different flavours of hadrons in the event
    int nFlavHadrons[5] = {0};		// Numbers of hadrons (0-Top, 1-Higgs, 2-Gluon, 3-Z)

    std::vector<std::vector<int> > LastQuarkMotherIds;

// Looping over all hadrons
    for ( unsigned int hadNum=0; hadNum<nHad; hadNum++ ) {

        unsigned int hadIdx = hadIndex[hadNum];   // Index of hadron in the hadMothers
        const reco::GenParticle* hadron = &hadMothers[hadIdx];
        int jetIndex = -1;

// Looping over all jets to match them to current hadron
        for ( unsigned int jetNum = 0; jetNum < nJets; jetNum++ ) {
// Checking whether jet contains this hadron in it (that was put in jet by clustering algorithm)
            std::vector<const reco::GenParticle*> particles = genJets[jetNum].getGenConstituents();
            for ( unsigned int partNum=0; partNum<particles.size(); partNum++ ) {
                const reco::GenParticle* particle = particles[partNum];
                if ( particle->status() <2 ) {
                    continue;    // Skipping final state particles
                }
                if ( !isHadron ( flavour_,particle ) ) {
                    continue;
                }
// Checking whether hadron and particle in jet are identical
                if ( hadron->pdgId() !=particle->pdgId() || fabs ( hadron->eta()-particle->eta() ) >0.00001 || fabs ( hadron->phi()-particle->phi() ) >0.00001 ) {
                    continue;
                }
                jetIndex=jetNum;
                break;
            }   // End of loop over jet constituents
            if ( jetIndex>=0 ) {
                break;
            }
        }   // End of loop over jets

        result.push_back ( jetIndex );  // Putting jet index to the result list

        std::vector <int> FirstQuarkId;
        std::vector <int> LastQuarkId;
        std::vector <int> LastQuarkMotherId;

        int hadFlav = hadMothers[hadIdx].pdgId() <0?-1:1; // Charge of the hadron (-1,1)
        if ( abs ( hadMothers[hadIdx].pdgId() ) /1000 < 1 ) {
            hadFlav*=-1;    // Inverting flavour of hadron if it is a meson
        }

// Searching only first quark in the chain with the same flavour as hadron
        findInMothers ( hadIdx, FirstQuarkId, hadMothersIndices, hadMothers, 0, hadFlav*flavour_, false, 1, false );

// Filling histograms for each 1-st quark
        for ( unsigned int qId=0; qId<FirstQuarkId.size(); qId++ ) {
            int qFlav = ( hadMothers[FirstQuarkId[qId]].pdgId() <0 ) ?-1:1;
            if ( toPlot ) {
                h_firstQstatus->Fill ( hadMothers[FirstQuarkId[qId]].status() );
                h_firstQPdg->Fill ( hadMothers[FirstQuarkId[qId]].pdgId() );
                h2_firstQFlav_HadFlav->Fill ( hadFlav,qFlav );
            }

// Getting mothers of the first quark
            std::vector<int> FirstQMotherId = hadMothersIndices.at ( FirstQuarkId[qId] );
            for ( unsigned int qmId=0; qmId<FirstQMotherId.size(); qmId++ ) {
                int qmFlav = ( hadMothers[FirstQMotherId[qmId]].pdgId() <0 ) ?-1:1;
                if ( toPlot ) {
                    h2_firstQFlav_QMotherFlav->Fill ( qFlav,qmFlav );
                }
            }		// End of loop over all mothers of the first quark of the hadron

// Finding last quark of the hadron starting from the first quark
            findInMothers ( FirstQuarkId[qId], LastQuarkId, hadMothersIndices, hadMothers, 0, hadFlav*flavour_, false, 2, false );

        }		// End of loop over all first quarks of the hadron

// Setting iinitial flavour of the hadron
        int hadronFlavour = 0;

//  Setting initial values for the quark that is closest to the hadron
        float dRmin = 999.9;
        int dRminQMId = -1;

// Finding the closest quark in dR
        for ( unsigned int qId=0; qId<LastQuarkId.size(); qId++ ) {
            int qIdx = LastQuarkId[qId];
            int qFlav = ( hadMothers.at ( qIdx ).pdgId() <0 ) ?-1:1;
// Checking the dR between hadron and quark
            float dR = deltaR ( hadMothers[hadIdx].eta(),hadMothers[hadIdx].phi(),hadMothers[qIdx].eta(),hadMothers[qIdx].phi() );
            if ( dR>=dRmin ) {
                continue;
            }
            if ( toPlot ) {
                h_lastQstatus->Fill ( hadMothers.at ( qIdx ).status() );
                h_lastQPdg->Fill ( hadMothers.at ( qIdx ).pdgId() );
                h2_lastQFlav_HadFlav->Fill ( hadFlav,qFlav );
            }
// Getting mothers of the last quark
            std::vector<int> LastQMotherId = hadMothersIndices.at ( qIdx );
            int inList = -1;
            for ( unsigned int i=0; i<LastQuarkMotherId.size(); i++ ) {
                if ( LastQuarkMotherId.at ( i ) ==LastQMotherId.at ( 0 ) ) {
                    inList=i;
                }
                break;
            }
            if ( inList<0 ) {
                LastQuarkMotherId.push_back ( LastQMotherId.at ( 0 ) );
                inList = LastQuarkMotherId.size()-1;
            }

            int qmFlav = ( hadMothers.at ( LastQMotherId.at ( 0 ) ).pdgId() <0 ) ?-1:1;
            if ( toPlot ) {
                h2_lastQFlav_QMotherFlav->Fill ( qFlav,qmFlav );
                h2_lastQPdg_QMotherPdg->Fill ( hadMothers.at ( qIdx ).pdgId(),hadMothers.at ( LastQMotherId.at ( 0 ) ).pdgId() );
            }
// Updating indices of closest quark mother
            dRmin=dR;
            dRminQMId=inList;
        }		// End of loop over all last quarks of the hadron

        LastQuarkMotherIds.push_back ( LastQuarkMotherId );

        if ( toPlot ) {
            h_lastQ_lastQ_dRmin->Fill ( dRmin );
        }

        if ( dRminQMId>=0 ) {
            int qmIdx = LastQuarkMotherId.at ( dRminQMId );
            hadronFlavour = abs ( hadMothers.at ( qmIdx ).pdgId() ) *hadFlav;		// Assigning hadron flavour to the pdg of the closest quark's mother
        }

// Comparing the matched quarks to quarks matched to the previous hadrons for selecting the highest Pt hadron
        if ( LastQuarkMotherId.size() >0 && dRminQMId>=0 ) {
            int sameQHadId = -1;
            for ( unsigned int j=0; j<hadNum; j++ ) {
                if ( hadFlavour.at ( j ) !=hadronFlavour ) {
                    continue;    // Skipping hadrons that don't have the same flavour
                }
                std::vector<int> LastQuarkMotherId_ = LastQuarkMotherIds.at ( j );
                for ( unsigned int i=0; i<LastQuarkMotherId_.size(); i++ ) {		// Looping over quark mothers of the hadron
                    if ( LastQuarkMotherId_.at ( i ) !=LastQuarkMotherId.at ( dRminQMId ) ) {
                        continue;    // Skipping quark mothers that are not identical to the current one
                    }
                    sameQHadId = j;		// Assigning id of the hadron that has the same associated quark mother
                    break;
                }		// End of loop over quark mothers of previous hadrons
                if ( sameQHadId>=0 ) {
                    break;    // Stopping if hadron with the same mother has been found
                }
            }		// End of loop over previous hadrons
// Setting flavour to 0 for the lower Pt hadron that was matched to the same quark
            if ( sameQHadId>=0 ) {
                if ( hadMothers.at ( hadIndex.at ( sameQHadId ) ).pt() > hadMothers.at ( hadIdx ).pt() ) {
                    hadronFlavour=0;
                } else {
                    hadFlavour.at ( sameQHadId ) =0;
                }
            }
        }

// 		printf("  hadronFlavour: %d (%d)\n",hadronFlavour,hadFlav);
        hadFlavour.push_back ( hadronFlavour );

        int jetIdx1 = result[hadNum];   // Index of jet from matching by jet clustering algorithm

        int jetIdx2 = -1;
        float maxPt = 0.0;
        float dR_min=999.9;
// Looping over jets containing decay products of hadron to find the highest Pt one
        for ( unsigned int jetNum=0; jetNum<nJets; jetNum++ ) {
            if ( toPlot ) {
                float dR = deltaR ( genJets[jetNum].eta(),genJets[jetNum].phi(),hadron->eta(),hadron->phi() );
                if ( dR<dR_min ) {
                    dR_min=dR;
                }
            }
            if ( !hadVsJet[hadNum][jetNum] ) {
                continue;
            }
            if ( genJets[jetNum].pt() <=maxPt ) {
                continue;
            }
            maxPt=genJets[jetNum].pt();
            jetIdx2=jetNum;
        }

        if ( toPlot ) {
            h_hadJetIndex->Fill ( jetIdx1 );
            h_hadJet_dRmin->Fill ( dR_min );
            if ( jetIdx1>=0 ) {
                h_jetHad_dR->Fill ( deltaR ( genJets[jetIdx1].eta(),genJets[jetIdx1].phi(),hadron->eta(),hadron->phi() ) );
            }
            if ( jetIdx2>=0 && jetIdx1>=0 ) {
                h_jetdPt->Fill ( genJets[jetIdx1].pt() /genJets[jetIdx2].pt() );
                h_jetDeltaPt->Fill ( genJets[jetIdx1].pt()-genJets[jetIdx2].pt() );
            }
            if ( jetIdx2>=0 ) {
                h_jetHadOld_dR->Fill ( deltaR ( genJets[jetIdx2].eta(),genJets[jetIdx2].phi(),hadron->eta(),hadron->phi() ) );
            }
// Setting index to highest Pt jet containing decay products of the hadron if no jet was associated by jet clustering algorithm
            if ( jetIdx2>=0 && jetIdx1<0 ) {
                h_jetHadOldNoMatch_dR->Fill ( deltaR ( genJets[jetIdx2].eta(),genJets[jetIdx2].phi(),hadron->eta(),hadron->phi() ) );
            }
            if ( jetIdx2>=0 && jetIdx1<0 && abs ( hadronFlavour ) ==0 ) {
                h_jetHadOldNoMatch_dR_noFlav->Fill ( deltaR ( genJets[jetIdx2].eta(),genJets[jetIdx2].phi(),hadron->eta(),hadron->phi() ) );
            }
            if ( abs ( hadronFlavour ) ==0 && jetIdx1>=0 ) {
                h_jetHad_dR_noFlav->Fill ( deltaR ( genJets[jetIdx1].eta(),genJets[jetIdx1].phi(),hadron->eta(),hadron->phi() ) );
            }

            h_nFirstQ->Fill ( FirstQuarkId.size() );
            h_nLastQ->Fill ( LastQuarkId.size() );
        }
    }		// End of loop over all hadrons

// Checking number of different flavours of hadrons in the event
    for ( unsigned int i=0; i<hadFlavour.size(); i++ ) {
        switch ( abs ( hadFlavour.at ( i ) ) ) {
        case 6:
            nFlavHadrons[0]++;	// Top
            break;
        case 25:
            nFlavHadrons[1]++;	// Higgs
            break;
        case 21:
            nFlavHadrons[2]++;	// Gluon
            break;
        case 23:
            nFlavHadrons[3]++;	// Z
            break;
        default:
            nFlavHadrons[4]++;	// No
            break;
        }
    }

    if ( toPlot ) {
        h_nHad->Fill ( nHad );
        h_nHadTop->Fill ( nFlavHadrons[0] );
        h_nHadHiggs->Fill ( nFlavHadrons[1] );
        h_nHadGluon->Fill ( nFlavHadrons[2] );
        h_nHadZ->Fill ( nFlavHadrons[3] );
        h_nHadNo->Fill ( nFlavHadrons[4] );

// Filling number of hadrons associated to a single jet
        for ( unsigned int iJet=0; iJet<nJets; iJet++ ) {
            int N = 0;
            for ( unsigned int i=0; i<result.size(); i++ ) if ( result[i]== ( int ) iJet ) {
                    N++;
                }
            h_nHadInJet->Fill ( N );
        }

    }		// If validation histograms should be filled

    return result;
}

/**
* @brief helper function to keep track of the decay chain and identify loops in the decay tree
*
* @param[out] particleChain vector of particles building up the current chain
* @param[in] particle particle that should be added
*
* returns true if a particle is already in the chain
*/
bool GenLevelBJetProducer::checkForLoop ( std::vector<const reco::Candidate*> &particleChain, const reco::Candidate* particle )
{

    for ( unsigned int i = 0; i < particleChain.size(); ++i ) {
        if ( particleChain[i]->pdgId() == 91 || particleChain[i]->pdgId() == 92 ) {
            continue;    // Ignoring clusters and strings
        }
        if ( particleChain[i] ==  particle ) {
            return true;
        }
    }

    particleChain.push_back ( particle );
    return false;
}

/**
* @brief Check if the cpecified particle is already in the list of particles
*
* @param[in] particleList - list of particles to be checked
* @param[in] particle - particle that should be checked
*
* returns true if a particle is already in the list
*/
int GenLevelBJetProducer::isInList ( std::vector<const reco::Candidate*> particleList, const reco::Candidate* particle )
{
    for ( unsigned int i = 0; i<particleList.size(); i++ )
        if ( particleList[i]==particle ) {
            return i;
        }

    return -1;
}

/**
* @brief Check the pdgId of a given particle if it is a hadron
*
* @param[in] flavour - flavour of a hadron that is being searched (5-B, 4-C)
* @param[in] thisParticle - a particle that is to be analysed
*
* @returns if the particle is a hadron of specified flavour
*/
bool GenLevelBJetProducer::isHadron ( const int flavour, const reco::Candidate* thisParticle )
{
    if ( abs ( thisParticle->pdgId() / 1000 )  == flavour // baryions
            || ( abs ( thisParticle->pdgId() / 100 % 10 ) == flavour // mesons
                 && ! ( noBBbarResonances_ && abs ( thisParticle->pdgId() / 10 % 100 ) == 11*flavour ) // but not a resonance
               )
       ) {
        return true;
    } else {
        return false;
    }
}

/**
* @brief Check if the particle has bHadron among daughters
*
* @param[in] flavour - flavour of a hadron that is being searched (5-B, 4-C)
* @param[in] thisParticle - a particle that is to be analysed
*
* @returns if the particle has a hadron among its daughters
*/
bool GenLevelBJetProducer::hasHadronDaughter ( const int flavour, const reco::Candidate* thisParticle )
{
// Looping through daughters of the particle
    bool hasDaughter = false;
    for ( int k=0; k< ( int ) thisParticle->numberOfDaughters(); k++ ) {
        if ( !isHadron ( flavour, thisParticle->daughter ( k ) ) ) {
            continue;
        }
        hasDaughter = true;
        break;
    }
    return hasDaughter;
}

/**
* @brief do a recursive search for the mother particles until the b-quark is found or the absolute mother is found
*
* the treatment of b-bar resonances depends on the global parameter noBBbarResonances_
*
* @param[in] thisParticle current particle from which starts the search of the hadron and all its mothers up to proton
* @param[out] hadron the last hadron in the decay chain (that decays weekly)
* @param[out] hadMothers list of all particles starting with hadron and ending with proton
* @param[out] hadMothersIndices list of i-vectors containing j-indices representing particles that are mothers of each i-particle from hadMothers
*
* @returns index of the hadron in the hadMothers list. -1 if no hadron found
*/
int GenLevelBJetProducer::analyzeMothers ( const reco::Candidate* thisParticle, pCRC *hadron, std::vector<const reco::Candidate*> &hadMothers, std::vector<std::vector<int> > &hadMothersIndices, std::set<const reco::Candidate*> *analyzedParticles, const int prevPartIndex )
{

    int hadronIndex=-1;	// Index of the hadron that is returned by this function
// Storing the first hadron has been found in the chain when going up from the final particle of the jet
    if ( *hadron == 0 // find only the first b-hadron on the way (the one that decays weekly)
            && isHadron ( flavour_, thisParticle ) // is a hadron
            && !hasHadronDaughter ( flavour_, thisParticle ) // has no hadron daughter (decays weekly)
       ) {
        *hadron = thisParticle;

        int index = isInList ( hadMothers, thisParticle );

        if ( index<0 ) { // If hadron is not in the list of mothers yet
            hadMothers.push_back ( thisParticle );
            hadronIndex=hadMothers.size()-1;
        } else {	    // If hadron is in the list of mothers already
            hadronIndex=index;
        }
    }

    int partIndex = -1;	  // Index of particle being checked in the list of mothers
    partIndex = isInList ( hadMothers, thisParticle );

// Checking whether this particle is already in the chain of analyzed particles in order to identify a loop
    bool isLoop = false;
    if ( !analyzedParticles ) {
        analyzedParticles = new std::set<const reco::Candidate*>;
    }
    for ( unsigned int i=0; i<analyzedParticles->size(); i++ ) {
        if ( analyzedParticles->count ( thisParticle ) <=0 ) {
            continue;
        }
        isLoop = true;
        break;
    }


// If a loop has been detected
    if ( isLoop ) {
        if ( prevPartIndex>=0 ) {
            putMotherIndex ( hadMothersIndices, prevPartIndex, -1 );    // Setting mother index of previous particle to -1
        }
        return hadronIndex;		// Stopping further processing of the current chain
    }
    analyzedParticles->insert ( thisParticle );


// Putting the mothers to the list of mothers
    for ( size_t iMother = 0; iMother < thisParticle->numberOfMothers(); ++iMother ) {
        const reco::Candidate* mother = thisParticle->mother ( iMother );
        int mothIndex = isInList ( hadMothers, mother );
        if ( mothIndex == partIndex && partIndex>=0 ) {
            continue;		// Skipping the mother that is its own daughter
        }

// Checking whether this mother has other daughters that have the same pdgId [Only if plotting option is enabled]
        if ( toPlot && abs ( thisParticle->pdgId() ) ==flavour_ && abs ( thisParticle->pdgId() ) == abs ( mother->pdgId() ) ) {
            double highPt = -1.0;
            int highPtId = -1;
// Loop over daughters of the mother to look how many daughters with the same pdgId it has
            float dR_min=999.9;
            int nSameDaugh=0;
            for ( unsigned int iDaughter = 0; iDaughter<mother->numberOfDaughters(); iDaughter++ ) {
                const reco::Candidate* daughter = mother->daughter ( iDaughter );
                if ( abs ( daughter->pdgId() ) !=abs ( thisParticle->pdgId() ) ) {
                    continue;
                }
                nSameDaugh++;
            }
            for ( unsigned int iDaughter = 0; iDaughter<mother->numberOfDaughters(); iDaughter++ ) {
                const reco::Candidate* daughter = mother->daughter ( iDaughter );
                if ( abs ( daughter->pdgId() ) !=abs ( thisParticle->pdgId() ) ) {
                    continue;
                }
                if ( daughter->pdgId() *mother->pdgId() < 0 ) {
                    continue;
                }
                if ( daughter->pt() >highPt ) {
                    highPt=daughter->pt();
                    highPtId=iDaughter;
                }
                float dR=deltaR ( daughter->eta(),daughter->phi(),mother->eta(),mother->phi() );

                h_QQMothEFrac->Fill ( daughter->energy() /mother->energy() );
                h_QQMothdR->Fill ( dR );
                if ( nSameDaugh>1 ) {
                    h_QQMothEFrac_ambig->Fill ( daughter->energy() /mother->energy() );
                    h_QQMothdR_ambig->Fill ( dR );
                }

                if ( dR<dR_min ) {
                    dR_min=dR;
                }
            }

            if ( highPt<0.0 ) {
                continue;
            }
            for ( unsigned int iDaughter = 0; iDaughter<mother->numberOfDaughters(); iDaughter++ ) {
                const reco::Candidate* daughter = mother->daughter ( iDaughter );
                if ( daughter->pdgId() *mother->pdgId() < 0 ) {
                    continue;    // Skipping particles with wrong charge
                }
                if ( ( int ) iDaughter==highPtId ) {
                    if ( toPlot ) {
                        h_QHQdR->Fill ( deltaR ( daughter->eta(),daughter->phi(),mother->eta(),mother->phi() ) );
                        if ( nSameDaugh>1 ) {
                            h_QHQdR_ambig->Fill ( deltaR ( daughter->eta(),daughter->phi(),mother->eta(),mother->phi() ) );
                        }
                    }
                    continue;		// Skipping the highest Pt particle
                }
                h_QHQPtFrac->Fill ( ( highPt-daughter->pt() ) /highPt );
                if ( nSameDaugh>1 ) {
                    h_QHQPtFrac_ambig->Fill ( ( highPt-daughter->pt() ) /highPt );
                }
            }
            h_QQMothdR_min->Fill ( dR_min );
            if ( nSameDaugh>1 ) {
                h_QQMothdR_min_ambig->Fill ( dR_min );
            }
            h2_QQMothSamePdg->Fill ( thisParticle->pdgId(),mother->pdgId() );
        }	// If both particle and mother are quarks and have same abs(pdgId)

// If this mother isn't yet in the list and hadron is in the list
        if ( mothIndex<0 && ( *hadron ) !=0 ) {
            hadMothers.push_back ( mother );
            mothIndex=hadMothers.size()-1;
        }
// If hadron has already been found in current chain and the mother isn't a duplicate of the particle being checked
        if ( ( *hadron ) !=0 && mothIndex!=partIndex && partIndex>=0 ) {
            putMotherIndex ( hadMothersIndices, partIndex, mothIndex );			// Putting the index of mother for current particle
        }
        int index = analyzeMothers ( mother, hadron, hadMothers, hadMothersIndices, analyzedParticles, partIndex );
        hadronIndex = index<0?hadronIndex:index;
    }			// End of loop over mothers

    analyzedParticles->erase ( thisParticle );		// Removing current particle from the current chain that is being analyzed

    if ( partIndex<0 ) {
        return hadronIndex;    // Safety check
    }

// Adding -1 to the list of mother indices for current particle if it has no mothers (for consistency between numbering of indices and mothers)
    if ( ( int ) thisParticle->numberOfMothers() <=0 && ( *hadron ) !=0 ) {
        putMotherIndex ( hadMothersIndices, partIndex, -1 );
    }


    return hadronIndex;

}

/**
* @brief puts mother index to the list of mothers of particle, if it isn't there already
*
* @param[in] hadMothersIndices vector of indices of mothers for each particle
* @param[in] partIndex index of the particle for which the mother index should be stored
* @param[in] mothIndex index of mother that should be stored for current particle
*
*/
bool GenLevelBJetProducer::putMotherIndex ( std::vector<std::vector<int> > &hadMothersIndices, int partIndex, int mothIndex )
{
    // Putting vector of mothers indices for the given particle
    bool inList=false;
    if ( partIndex<0 ) {
        return false;
    }

    while ( ( int ) hadMothersIndices.size() <=partIndex ) { // If there is no list of mothers for current particle yet
        std::vector<int> mothersIndices;
        hadMothersIndices.push_back ( mothersIndices );
    }

    std::vector<int> *hadMotherIndices=&hadMothersIndices.at ( partIndex );
// Removing other mothers if particle must have no mothers
    if ( mothIndex==-1 ) {
        hadMotherIndices->clear();
    } else {
// Checking if current mother is already in the list of theParticle's mothers
        for ( int k=0; k< ( int ) hadMotherIndices->size(); k++ ) {
            if ( hadMotherIndices->at ( k ) !=mothIndex && hadMotherIndices->at ( k ) !=-1 ) {
                continue;
            }
            inList=true;
            break;
        }
    }
// Adding current mother to the list of mothers of this particle
    if ( !inList ) {
        hadMotherIndices->push_back ( mothIndex );
    }

    return inList;
}


/**
* @brief helper function to resolve realname of PDG-ID
*/
std::string GenLevelBJetProducer::getParticleName ( int id ) const
{

    const ParticleData * pd = 0;
    if ( resolveName_ ) {
        pdt_->particle ( id );
    }
    if ( !pd ) {
        std::ostringstream ss;
        ss << "P" << id;
        return ss.str();
    } else {
        return pd->name();
    }

}


/**
* @brief helper function to print jet properies
*
* this function is inlined to allow efficient optimisation when compiled without debug option
*
* @param[in] iJet the index of the jet
* @param[in] thisJet pointer to the jet
*
* @returns string to be printed
*/
inline std::string GenLevelBJetProducer::printJetInfo ( const size_t iJet, const reco::GenJet* thisJet ) const
{
    std::ostringstream out;
    out <<  "Jet: " << iJet
        << " particles: " << thisJet->getGenConstituents().size()
        << " p4: " << thisJet->p4()
        << " p_t: " << thisJet->pt()
        << std::endl;
    return out.str();
}

/**
* @brief helper function to generate a human readable representation of the decay chain
*
* this function is inlined to allow efficient optimisation when compiled without debug option
*
* @param[in] particleChain the decay chain
* @param[in] hadron the identified b-hadron
*
* @returns string to be printed
*/
inline std::string GenLevelBJetProducer::printParticleChain ( const std::vector<const reco::Candidate*> &particleChain, const reco::Candidate* hadron ) const
{
    std::ostringstream out;

    for ( int i = particleChain.size()-1; i >= 0; --i ) {
        if ( particleChain[i] == hadron ) {
            out << "*" << getParticleName ( particleChain[i]->pdgId() ) <<"*";
        } else {
            out << getParticleName ( particleChain[i]->pdgId() );
        }
        if ( i>0 ) {
            out  << "-->";
        }
    }
    return out.str();
}


/**
* @brief identify which of the jets is the nearest to the particle
*
* @param particle reference particle
* @param genJets candidate jets
* @returns index of the best jet in the vector or -1 if no jet within the maximum @f$\delta_{r}@f$
*/
int GenLevelBJetProducer::getGenJetNear ( const reco::Candidate* particle, std::vector<const reco::GenJet*> &genJets )
{
    int result = -1;
    double bestDr = deltaR_;
    for ( unsigned int i = 0; i < genJets.size(); ++i ) {
        double dr = deltaR ( genJets[i]->eta(), genJets[i]->phi(), particle->eta(), particle->phi() );
        if ( dr < bestDr ) {
            bestDr = dr;
            result = i;
        }
    }
    return result;
}


/**
* @brief helper function to find indices of particles with particular pdgId and status from the list of mothers of a given particle
*
* @param[in] idx index of particle, mothers of which should be searched
* @param[in] hadMothersIndices list of indices pointing to mothers of each particle from list of mothers
* @param[in] hadMothers vector of all hadron mother particles of all levels
* @param[in] status status of mother that is being looked for
* @param[in] pdgId pdgId of mother that is being looked for
* @param[in] pdgAbs option, whether the sign of pdgId matters
* @param[in] charge charge of the particle (0 = any charge)
* @param[in] firstLast should all(0), first(1) or last(2) occurances of the searched particle be stored
* @param[in] verbose option to print every particle that is processed during the search
* @param[out] mothChains vector of indices where the found mothers are put
*
*/

void GenLevelBJetProducer::findInMothers ( int idx, std::vector<int> &mothChains, std::vector<std::vector<int> > &hadMothersIndices, std::vector<reco::GenParticle> &hadMothers, int status, int pdgId, bool pdgAbs=false, int firstLast=0, bool verbose=false	)
{

    int partCharge = ( hadMothers.at ( idx ).pdgId() >0 ) ?1:-1;
// Inverting charge if mother is a b(c) meson
    if ( abs ( hadMothers.at ( idx ).pdgId() ) /1000 < 1 && ( abs ( hadMothers.at ( idx ).pdgId() ) /100%10 == 4 || abs ( hadMothers.at ( idx ).pdgId() ) /100%10 == 5 ) ) {
        partCharge*=-1;
    }

    if ( ( int ) hadMothersIndices.size() <=idx ) {
        if ( verbose ) {
            printf ( " Stopping checking particle %d. No mothers are stored.\n",idx );
        }
        return;	  // Skipping if no mothers are stored for this particle
    }

    std::vector<int> mothers = hadMothersIndices.at ( idx );
    unsigned int nMothers = mothers.size();
    bool isCorrect=false;		// Whether current particle is what is being searched
    if ( verbose ) {
        if ( abs ( hadMothers.at ( idx ).pdgId() ) ==2212 ) {
            printf ( "Chk:  %d\tpdg: %d\tstatus: %d",idx, hadMothers.at ( idx ).pdgId(), hadMothers.at ( idx ).status() );
        } else {
            printf ( " Chk:  %d(%d mothers)\tpdg: %d\tstatus: %d\tPt: %.3f\tEta: %.3f",idx, nMothers, hadMothers.at ( idx ).pdgId(), hadMothers.at ( idx ).status(), hadMothers.at ( idx ).pt(),hadMothers.at ( idx ).eta() );
        }
    }
    bool hasCorrectMothers = false;
    for ( unsigned int i=0; i<nMothers; i++ ) {
        int motherId = mothers.at ( i );
        if ( motherId<0 ) {
            continue;
        }
        if ( !isNeutralPdg ( hadMothers.at ( motherId ).pdgId() ) && hadMothers.at ( motherId ).pdgId() *pdgId<0 && !pdgAbs ) {
            continue;    // Skipping if mother of the particle has wrong charge
        }
        hasCorrectMothers=true;
    }
// Checking whether current mother satisfies selection criteria
    if ( ( ( hadMothers.at ( idx ).pdgId() == pdgId && pdgAbs==false )
            || ( abs ( hadMothers.at ( idx ).pdgId() ) == abs ( pdgId ) && pdgAbs==true ) )
            && ( hadMothers.at ( idx ).status() == status || status==0 )
            && hasCorrectMothers ) {
        isCorrect=true;
        bool inList=false;
        for ( unsigned int k=0; k<mothChains.size(); k++ ) if ( mothChains[k]==idx ) {
                inList=true;    // Checking whether isn't already in the list
                break;
            }
        if ( !inList && mothers.at ( 0 ) >=0 && ( hadMothers.at ( idx ).pdgId() *pdgId>0 || !pdgAbs ) ) {		// If not in list and mother of this quark has correct charge
            if ( firstLast==0 || firstLast==1 ) {
                mothChains.push_back ( idx );
            }
            if ( verbose ) {
                printf ( "   *" );
            }
        }
        if ( verbose ) {
            printf ( "   +++" );
        }
    }
    if ( verbose ) {
        printf ( "\n" );
    }

    if ( isCorrect && firstLast==1 ) {
        return;   // Stopping if only the first particle in the chain is looked for
    }

// Checking next level mothers
    for ( unsigned int i=0; i<nMothers; i++ ) {
        int idx2 = mothers[i];
        if ( idx2<0 ) {
            continue;    // Skipping if mothers id is -1 (no mother), that means current particle is a proton
        }
        if ( idx2==idx ) {
            continue;    // Skipping if particle is stored as its own mother
        }
        if ( firstLast==2 && isCorrect && (
                    ( abs ( hadMothers[idx2].pdgId() ) != abs ( pdgId ) && pdgAbs==true ) ||
                    ( hadMothers[idx2].pdgId() != pdgId && pdgAbs==false ) ) ) {		// If only last occurance must be stored and mother has different flavour
            if ( verbose ) {
                printf ( "Checking mother %d out of %d mothers once more to store it as the last quark\n",i,nMothers );
            }
            findInMothers ( idx, mothChains, hadMothersIndices, hadMothers, 0, pdgId, pdgAbs, 1, verbose );
        }

// Checking next level mother
        if ( verbose ) {
            printf ( "Checking mother %d out of %d mothers\n",i,nMothers );
        }
        findInMothers ( idx2, mothChains, hadMothersIndices, hadMothers, status, pdgId, pdgAbs, firstLast, verbose );
    }
}


/**
* @brief Check whether a given pdgId represents neutral particle
*
* @param[in] pdgId
* @param[in] thisParticle - a particle that is to be analysed
*
* @returns if the particle has a hadron among its daughters
*/
bool GenLevelBJetProducer::isNeutralPdg ( int pdgId )
{
    const int max = 5;
    int neutralPdgs[max]= {9,21,22,23,25};
    for ( int i=0; i<max; i++ ) if ( abs ( pdgId ) ==neutralPdgs[i] ) {
            return true;
        }
    return false;
}


//define this as a plug-in
DEFINE_FWK_MODULE ( GenLevelBJetProducer );
