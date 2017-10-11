///
/// \class l1t::GtRecordDump.cc
///
/// Description: Dump/Analyze Input Collections for GT.
///
/// Implementation:
///    Based off of Michael Mulhearn's YellowParamTester
///
/// \author: Brian Winer Ohio State
///


//
//  This simple module simply retreives the YellowParams object from the event
//  setup, and sends its payload as an INFO message, for debugging purposes.
//


#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
//#include "FWCore/ParameterSet/interface/InputTag.h"

// system include files
#include <fstream>
#include <iomanip>

// user include files
//   base class
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/L1TGlobal/interface/GlobalExtBlk.h"
#include "DataFormats/L1TGlobal/interface/GlobalObject.h"

#include "DataFormats/L1TGlobal/interface/GlobalObjectMapFwd.h"
#include "DataFormats/L1TGlobal/interface/GlobalObjectMap.h"
#include "DataFormats/L1TGlobal/interface/GlobalObjectMapRecord.h"

#include "L1Trigger/L1TGlobal/interface/L1TGlobalUtil.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/MessageLogger/interface/MessageDrop.h"

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TSystem.h>
#include <TFile.h>

using namespace edm;
using namespace std;

namespace l1t {

  // class declaration
  class CompUGT : public edm::EDAnalyzer {
  public:
    explicit CompUGT(const edm::ParameterSet&);
    virtual ~CompUGT(){};
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
    virtual void endJob();

    InputTag   uGtAlgInputTag1,uGtAlgInputTag2;
    InputTag   uGtExtInputTag1,uGtExtInputTag2;

    EDGetToken uGtAlgToken1,uGtAlgToken2;
    EDGetToken uGtExtToken1,uGtExtToken2;


    std::map<std::string, std::vector<int> > m_algoSummary1,m_algoSummary2;

    L1TGlobalUtil *m_gtUtil1, *m_gtUtil2;

    private:
    int m_tvVersion;
    int iFirst;

    edm::Service<TFileService> fs;
    void fillHist(const TString& histName, const Double_t& value, const Double_t& wt=1.0);
    void fill2DHist(const TString& histName, const Double_t& x,const Double_t& y,const Double_t& wt=1.0);

    // use the map function to access the rest of the histograms
    std::map<TString, TH1*> m_HistNames;
    std::map<TString, TH1*>::iterator hid;

    std::map<TString, TH2*> m_HistNames2D;
    std::map<TString, TH2*>::iterator hid2D;

  };

  CompUGT::CompUGT(const edm::ParameterSet& iConfig)
  {
      uGtAlgInputTag1 = iConfig.getParameter<InputTag>("uGtAlgInputTag1");
      uGtExtInputTag1 = iConfig.getParameter<InputTag>("uGtExtInputTag1");

      uGtAlgInputTag2 = iConfig.getParameter<InputTag>("uGtAlgInputTag2");
      uGtExtInputTag2 = iConfig.getParameter<InputTag>("uGtExtInputTag2");

      uGtAlgToken1 = consumes<BXVector<GlobalAlgBlk>>(uGtAlgInputTag1);
      uGtExtToken1 = consumes<BXVector<GlobalExtBlk>>(uGtExtInputTag1);

      uGtAlgToken2 = consumes<BXVector<GlobalAlgBlk>>(uGtAlgInputTag2);
      uGtExtToken2 = consumes<BXVector<GlobalExtBlk>>(uGtExtInputTag2);


      m_gtUtil1 = new L1TGlobalUtil(iConfig, consumesCollector(), *this, uGtAlgInputTag1, uGtExtInputTag1);
      m_gtUtil2 = new L1TGlobalUtil(iConfig, consumesCollector(), *this, uGtAlgInputTag2, uGtExtInputTag2);
      //m_gtUtil1->OverridePrescalesAndMasks(preScaleFileName,preScColumn);

      iFirst=0;

      TString hname,htitle;

      hname="EventCounter"; htitle="Event Counter";
      m_HistNames[hname] =fs->make<TH1F>(hname,htitle,5,0.,5.);
      m_HistNames[hname]->Sumw2();

      hname="TriggerDecisions1"; htitle="Trigger Decisions -- uGT collection 1";
      m_HistNames[hname] =fs->make<TH1F>(hname,htitle,512,0.,512);
      //m_HistNames[hname]->Sumw2();

      hname="TriggerDecisions2"; htitle="Trigger Decisions -- uGT collection 2";
      m_HistNames[hname] =fs->make<TH1F>(hname,htitle,512,-0.5,511.5);
      //m_HistNames[hname]->Sumw2();

      hname="TriggerMismatches1"; htitle="Trigger Mismatches -- uGT collection 1 fired";
      m_HistNames[hname] =fs->make<TH1F>(hname,htitle,512,-0.5,511.5);

      hname="TriggerMismatches2"; htitle="Trigger Mismatches -- uGT collection 2 fired";
      m_HistNames[hname] =fs->make<TH1F>(hname,htitle,512,-0.5,511.5);
  }

  // loop over events
  void CompUGT::analyze(const edm::Event& iEvent, const edm::EventSetup& evSetup){

    fillHist("EventCounter",1);
 //inputs
  Handle<BXVector<GlobalAlgBlk>> uGtAlg1;
  iEvent.getByToken(uGtAlgToken1,uGtAlg1);

  Handle<BXVector<GlobalAlgBlk>> uGtAlg2;
  iEvent.getByToken(uGtAlgToken2,uGtAlg2);

  Handle<BXVector<GlobalExtBlk>> uGtExt1;
  iEvent.getByToken(uGtExtToken1,uGtExt1);

  Handle<BXVector<GlobalExtBlk>> uGtExt2;
  iEvent.getByToken(uGtExtToken2,uGtExt2);


     //Fill the L1 result maps
     m_gtUtil1->retrieveL1(iEvent,evSetup,uGtAlgToken1);
     m_gtUtil2->retrieveL1(iEvent,evSetup,uGtAlgToken2);

     LogDebug("CompUGT") << "retrieved L1 data " << endl;

     // grab the map for the final decisions
     const std::vector<std::pair<std::string, bool> > initialDecisions1 = m_gtUtil1->decisionsInitial();
     const std::vector<std::pair<std::string, bool> > intermDecisions1 = m_gtUtil1->decisionsInterm();
     const std::vector<std::pair<std::string, bool> > finalDecisions1 = m_gtUtil1->decisionsFinal();
     const std::vector<std::pair<std::string, int> >  prescales1 = m_gtUtil1->prescales();
     const std::vector<std::pair<std::string, std::vector<int> > > masks1 = m_gtUtil1->masks();

     const std::vector<std::pair<std::string, bool> > initialDecisions2 = m_gtUtil2->decisionsInitial();
     const std::vector<std::pair<std::string, bool> > intermDecisions2 = m_gtUtil2->decisionsInterm();
     const std::vector<std::pair<std::string, bool> > finalDecisions2 = m_gtUtil2->decisionsFinal();
     const std::vector<std::pair<std::string, int> >  prescales2 = m_gtUtil2->prescales();
     const std::vector<std::pair<std::string, std::vector<int> > > masks2 = m_gtUtil2->masks();

     if (initialDecisions1.size() != initialDecisions2.size() ){
       cout << " !! Trigger map sizes do not match up!!  Skipping rest of routine" << std::endl;
       return;
     }

     bool printTriggerMenu = true;
     if (printTriggerMenu && iFirst==0){
       iFirst=1;

       cout << "    Bit                  Algorithm Name               " << endl;
       cout << "======================================================" << endl;
       for(unsigned int i=0; i<initialDecisions1.size(); i++) {

       // get the name and trigger result
       std::string name1 = (initialDecisions1.at(i)).first;
       cout << i << "\t" << name1 << endl;
       }

       cout << "\n==============================================================\n" << endl;

       for(unsigned int i=0; i<initialDecisions2.size(); i++) {

       // get the name and trigger result
       std::string name2 = (initialDecisions2.at(i)).first;
       cout << i << "\t" << name2 << endl;
       }
     }
     for(unsigned int i=0; i<initialDecisions1.size(); i++) {

       // get the name and trigger result
       std::string name1 = (initialDecisions1.at(i)).first;
       std::string name2 = (initialDecisions2.at(i)).first;
       if (name1 != name2 ){
	 cout << "\t !! Trigger names do not match up!!  " << name1 << "\t" << name2 << "\tSkipping this trigger bit" << std::endl;
	 continue;
       }

       bool resultInit1 = (initialDecisions1.at(i)).second;
       bool resultInit2 = (initialDecisions2.at(i)).second;

       if (resultInit1) {
	 fillHist("TriggerDecisions1",float(i));
	 if ( not resultInit2 ) fillHist("TriggerMismatchs1",float(i));
       }
       if (resultInit2) {
	 fillHist("TriggerDecisions2",float(i));
	 if ( not resultInit1 ) fillHist("TriggerMismatchs2",float(i));
       }

       if (//name1.find("BPTX")                    == std::string::npos &&
	   //name1.find("Bptx")                    == std::string::npos &&
	   //name1.find("L1_FirstBunchInTrain")    == std::string::npos &&
	   //name1.find("L1_FirstCollisionInTrain")== std::string::npos &&
	   //name1.find("L1_IsolatedBunch")        == std::string::npos &&
	   //name1.find("L1_FirstCollisionInOrbit")== std::string::npos &&
	   //name1.find("L1_FirstBunchAfterTrain")== std::string::npos &&
	   name1.find("L1_LastCollisionInTrain")== std::string::npos &&
	   name1.find("L1_ZeroBias")             == std::string::npos )
	 {
	 if (resultInit1 != resultInit2){
	   cout << "\tTrigger Missmatch for Trigger: " << name1 << "\tGT1: " << resultInit1 << "\tGT2: " << resultInit2
		<< "\tRun: " << iEvent.id().run() << "\tEvent: " << iEvent.id().event() << "\tLumi: " << iEvent.id().luminosityBlock()
		<< endl;
	 }
       }
       //  put together our map of algorithms and counts across events
       if(m_algoSummary1.count(name1)==0) {
         std::vector<int> tst;
	 tst.resize(3);
	 m_algoSummary1[name1]=tst;
	 m_algoSummary2[name2]=tst;
       }
       if (resultInit1) (m_algoSummary1.find(name1)->second).at(0) += 1;
       if (resultInit2) (m_algoSummary2.find(name2)->second).at(0) += 1;

       // get prescaled and final results (need some error checking here)
       bool resultInterm = (intermDecisions1.at(i)).second;
       if (resultInterm) (m_algoSummary1.find(name1)->second).at(1) += 1;
       bool resultFin = (finalDecisions1.at(i)).second;
       if (resultFin) (m_algoSummary1.find(name1)->second).at(2) += 1;

       // get the prescale and mask (needs some error checking here)
       //int prescale = (prescales1.at(i)).second;
       //std::vector<int> mask    = (masks1.at(i)).second;

       //if(name1 != "NULL") cout << std::dec << setfill(' ') << "   " << setw(5) << i << "   " << setw(60) << name1.c_str() << "   " << setw(7) << resultInit << setw(7) << resultInterm << setw(7) << resultFin << setw(10) << prescale << setw(11) << mask.size() << endl;
     }


     //cout << "================================================================================================================================" << endl;

  }

// ------------ method called when ending the processing of a run  ------------

void
CompUGT::endRun(edm::Run const&, edm::EventSetup const&)
{

  // cout << "===========================================================================================================" << endl;
}

void
CompUGT::endJob()
{

     cout << "============   Done running CompUGT ==============" << endl;
}

}

void
l1t::CompUGT::fillHist(const TString& histName, const Double_t& value, const Double_t& wt) {

  hid=m_HistNames.find(histName);
  if (hid==m_HistNames.end())
    std::cout << "%fillHist -- Could not find histogram with name: " << histName << std::endl;
  else
    hid->second->Fill(value,wt);

}

void
l1t::CompUGT::fill2DHist(const TString& histName, const Double_t& x,const Double_t& y,const Double_t& wt) {

  hid2D=m_HistNames2D.find(histName);
  if (hid2D==m_HistNames2D.end())
    std::cout << "%fillHist -- Could not find histogram with name: " << histName << std::endl;
  else
    hid2D->second->Fill(x,y,wt);

}

DEFINE_FWK_MODULE(l1t::CompUGT);
