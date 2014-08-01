#ifndef MyL1_L1Analyzer_L1JetPlots_h
#define MyL1_L1Analyzer_L1JetPlots_h
/* \class L1JetPlots
 *
 * \author Leonard Apanasevich
 *
 * \version 1
 *
 */
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "MyL1/L1Analyzer/interface/L1JetPlots.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/CaloJet.h"

#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"

#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"

#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"

#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

#include "DataFormats/Math/interface/deltaR.h"

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TSystem.h>
#include <TFile.h>
#include <TCanvas.h>
#include <cmath>
#include <map>

using namespace edm;
using namespace reco;
using namespace std;

class L1JetPlots : public edm::EDAnalyzer {
public:
  L1JetPlots( const edm::ParameterSet & );

private:
  void beginJob();
  void analyze( const edm::Event& , const edm::EventSetup& );
  void endJob();

  void L1Analysis( const CaloJetCollection&,
		   const PFJetCollection&,
		   const GenJetCollection&,
		   const l1extra::L1JetParticleCollection&,
		   const l1extra::L1JetParticleCollection&,
		   const l1extra::L1JetParticleCollection&
		   );

  bool checkPFJetID(const std::vector<reco::PFJet>::const_iterator);
  bool checkCaloJetID(const std::vector<reco::CaloJet>::const_iterator);

  edm::Service<TFileService> fs;

  template <typename T> void mtchL1(const Double_t&, const Double_t&, const Double_t&, 
				    const T& jets, const TString&);

  edm::InputTag CaloJetAlgorithm, PFJetAlgorithm, GenJetAlgorithm, recmet_,genmet_, l1CollectionsTag_;
  int errCnt;
  const int errMax(){return 100;}

  void fillHist(const TString& histName, const Double_t& value, const Double_t& wt=1.0);
  void fill2DHist(const TString& histName, const Double_t& x,const Double_t& y,const Double_t& wt=1.0);

  bool doCaloJets,doGenJets,doCaloMET,doGenMET,doPFJets;
  bool doL1Jets;

  TH1F *ptCal, *etaCal, *phiCal;
  TH1F *ptCalL,*ptCalL8,*ptCalL12,*ptCalL16,*ptCalL36,*ptCalL52,*ptCalL68,*ptCalL92,*ptCalL128;
  TH1F *ptPFL, *ptPFL8, *ptPFL12, *ptPFL16, *ptPFL36, *ptPFL52, *ptPFL68,*ptPFL92,*ptPFL128;

  TH1F *ptGen, *etaGen, *phiGen;

  TH1F *MetPt, *genMetPt;

  // use the map function to access the rest of the histograms
  std::map<TString, TH1*> m_HistNames;
  std::map<TString, TH1*>::iterator hid;

  std::map<TString, TH2*> m_HistNames2D;
  std::map<TString, TH2*>::iterator hid2D;

  int nL1Jet8,nL1Jet12,nL1Jet16,nL1Jet36,nL1Jet52,nL1Jet68,nL1Jet92,nL1Jet128;
  int nL1TauJet20;

};

#endif
