#include <TH1F.h>
#include <TROOT.h>
#include <TFile.h>
#include <TSystem.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"

#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctHFRingEtSums.h"
#include "DataFormats/L1GlobalCaloTrigger/interface/L1GctHFBitCounts.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "MyL1/L1Analyzer/interface/myRootIO.h"

typedef std::map<std::string,bool> trigmap_t;
typedef std::map<std::string,bool>::iterator trigmapiter_t;

typedef std::map<std::string,int> algoBitToName_t;
typedef std::map<std::string,int>::iterator algoBitToName_iter_t;

trigmap_t getHLTResults(const edm::TriggerResults&, const edm::TriggerNames&);
void  BookHistograms(TFileDirectory);
template <typename T> void CompareL1Extra(const T&,const T&, const int&, const int&,const int&, std::string );

void CompareGT(const L1GlobalTriggerReadoutRecord* ,const L1GlobalTriggerReadoutRecord* );
algoBitToName_t FillAlgoToBitMap(const std::string&);

using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[]) 
{

  // ----------------------------------------------------------------------
  // First Part: 
  //
  //  * enable the AutoLibraryLoader 
  //  * book the histograms of interest 
  //  * open the input file
  // ----------------------------------------------------------------------

  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();

  // parse arguments
  if ( argc < 2 ) {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  if( !edm::readPSetsFrom(argv[1])->existsAs<edm::ParameterSet>("process") ){
    std::cout << " ERROR: ParametersSet 'process' is missing in your configuration file" << std::endl; exit(0);
  }
  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  fwlite::InputSource inputHandler_(process); fwlite::OutputFiles outputHandler_(process);


  // now get each parameter
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("compL1Extra");
  edm::InputTag TriggerResults_( ana.getParameter<edm::InputTag>("TriggerResults") );
  edm::InputTag GtDigis1_( ana.getParameter<edm::InputTag>("GtDigis1") );
  edm::InputTag GtDigis2_( ana.getParameter<edm::InputTag>("GtDigis2") );  
  edm::InputTag L1ExtraEMIsol1_( ana.getParameter<edm::InputTag>("L1ExtraEMIsol1") );
  edm::InputTag L1ExtraEMIsol2_( ana.getParameter<edm::InputTag>("L1ExtraEMIsol2") );
  edm::InputTag L1ExtraCenJet1_( ana.getParameter<edm::InputTag>("L1ExtraCenJet1") );
  edm::InputTag L1ExtraCenJet2_( ana.getParameter<edm::InputTag>("L1ExtraCenJet2") );
  edm::InputTag L1ExtraForJet1_( ana.getParameter<edm::InputTag>("L1ExtraForJet1") );
  edm::InputTag L1ExtraForJet2_( ana.getParameter<edm::InputTag>("L1ExtraForJet2") );
  edm::InputTag L1ExtraEMNonIsol1_( ana.getParameter<edm::InputTag>("L1ExtraEMNonIsol1") );
  edm::InputTag L1ExtraEMNonIsol2_( ana.getParameter<edm::InputTag>("L1ExtraEMNonIsol2") );
  edm::InputTag L1ExtraTauIsol1_( ana.getParameter<edm::InputTag>("L1ExtraTauIsol1") );
  edm::InputTag L1ExtraTauIsol2_( ana.getParameter<edm::InputTag>("L1ExtraTauIsol2") );
  edm::InputTag L1ExtraTauRlx1_( ana.getParameter<edm::InputTag>("L1ExtraTauRlx1") );
  edm::InputTag L1ExtraTauRlx2_( ana.getParameter<edm::InputTag>("L1ExtraTauRlx2") );
  edm::InputTag L1ExtraMET1_( ana.getParameter<edm::InputTag>("L1ExtraMET1") );
  edm::InputTag L1ExtraMET2_( ana.getParameter<edm::InputTag>("L1ExtraMET2") );
  edm::InputTag L1ExtraMHT1_( ana.getParameter<edm::InputTag>("L1ExtraMHT1") );
  edm::InputTag L1ExtraMHT2_( ana.getParameter<edm::InputTag>("L1ExtraMHT2") );

  string Trigger_( ana.getParameter<string>( "Trigger" ) );
  string TriggerBitMap_( ana.getParameter<string>( "TriggerBitMap" ) );
  //bool Debug_( ana.getParameter<bool>( "Debug" ) );
  //int TriggerPS_( ana.getParameter<int>( "TriggerPS" ) );
  uint BeginLumi_( ana.getParameter<uint>("BeginLumi") );
  uint EndLumi_  ( ana.getParameter<uint>("EndLumi") );

  // Prepare output file and book a set of histograms
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  TFileDirectory dir = fs.mkdir("compL1Extra");
  BookHistograms(dir);

  //read trigger bit map
  algoBitToName_t algoBitToName = FillAlgoToBitMap(TriggerBitMap_);

  // loop the events
  int ievt=0;  
  int maxEvents_( inputHandler_.maxEvents() );
  cout << "\nNumber of input files: " << inputHandler_.files().size() << "\n";
  cout << "Number of events to process: " << maxEvents_ << "\n";
  cout << "Output histograms written to: " << outputHandler_.file() << "\n" << endl;

  for(unsigned int iFile=0; iFile<inputHandler_.files().size(); ++iFile){
    // open input file (can be located on castor)
    cout << iFile << ": " << inputHandler_.files()[iFile] << endl;
    TFile* inFile = TFile::Open(inputHandler_.files()[iFile].c_str());
    if( inFile ){
      // ----------------------------------------------------------------------
      // Second Part: 
      //
      //  * loop the events in the input file 
      //  * receive the collections of interest via fwlite::Handle
      //  * fill the histograms
      //  * after the loop close the input file
      // ----------------------------------------------------------------------
      fwlite::Event ev(inFile);
      fwlite::EventSetup evSetup(inFile);

      for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt){
	edm::EventBase const & event = ev;
	// break loop if maximal number of events is reached 
	if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
	// simple event counter
	if(inputHandler_.reportAfter()!=0 ? (ievt>0 && ievt%inputHandler_.reportAfter()==0) : false) {
	  cout << "Processing " << ievt<< "th event: "
	       << "run " << ev.id().run() 
	       << ", lumi " << ev.luminosityBlock() 
	       << ", evt " << ev.id().event() << endl;
	}
	
	if (ev.luminosityBlock() < BeginLumi_ || ev.luminosityBlock() > EndLumi_) continue;

	edm::Handle<edm::TriggerResults> TriggerResults;
	event.getByLabel(TriggerResults_,TriggerResults);
	if (! TriggerResults.isValid()){
	  std::cout << " ERROR: Could not read TriggerResults object -- Exiting" << std::endl; 
	  exit(0);
	}

	const edm::TriggerNames triggerNames = event.triggerNames(*TriggerResults);
	trigmap_t hltTriggers=getHLTResults(*TriggerResults,triggerNames);


	// filter on Trigger Bits from Original Trigger Collections: RefTriggerResults
	trigmapiter_t trig_iter=hltTriggers.find(Trigger_);

	bool myTrig=false;
	if (Trigger_ == "Any")
	  {
	    myTrig =true;
	  }
	else
	  {
	    if (trig_iter==hltTriggers.end()){
	      cout << "Could not find trigger path with name: " << Trigger_ << endl;
	      exit(0);
	    }else{
	      myTrig=trig_iter->second;
	    }
	  }
	if (myTrig){

	  fillHist("LumiBlocks",ev.luminosityBlock(),1.); // keep track of the lumiBlocks analysed


	  edm::Handle<L1GlobalTriggerReadoutRecord> gtrr_handle1,gtrr_handle2;
	  event.getByLabel(GtDigis1_.label(), gtrr_handle1);
	  event.getByLabel(GtDigis2_.label(), gtrr_handle2);
	  if ( (! gtrr_handle1.isValid()) or (! gtrr_handle2.isValid()) ){
	    std::cout << "Trouble " << std::endl; 
	    continue;
	  }
	  L1GlobalTriggerReadoutRecord const* gtrr1 = gtrr_handle1.product();
	  L1GlobalTriggerReadoutRecord const* gtrr2 = gtrr_handle2.product();
	  CompareGT(gtrr1,gtrr2);


	  edm::Handle<l1extra::L1JetParticleCollection>  L1ExtraCenJet1,L1ExtraCenJet2;
	  event.getByLabel(L1ExtraCenJet1_, L1ExtraCenJet1);
	  event.getByLabel(L1ExtraCenJet2_, L1ExtraCenJet2);

	  edm::Handle<l1extra::L1JetParticleCollection>  L1ExtraForJet1,L1ExtraForJet2;
	  event.getByLabel(L1ExtraForJet1_, L1ExtraForJet1);
	  event.getByLabel(L1ExtraForJet2_, L1ExtraForJet2);
	  
	  edm::Handle<l1extra::L1EmParticleCollection>  L1ExtraEMIsol1,L1ExtraEMIsol2;
	  event.getByLabel(L1ExtraEMIsol1_, L1ExtraEMIsol1);
	  event.getByLabel(L1ExtraEMIsol2_, L1ExtraEMIsol2);

	  edm::Handle<l1extra::L1EmParticleCollection>  L1ExtraEMNonIsol1,L1ExtraEMNonIsol2;
	  event.getByLabel(L1ExtraEMNonIsol1_, L1ExtraEMNonIsol1);
	  event.getByLabel(L1ExtraEMNonIsol2_, L1ExtraEMNonIsol2);
	  
	  edm::Handle<l1extra::L1JetParticleCollection>  L1ExtraTauIsol1,L1ExtraTauIsol2;
	  event.getByLabel(L1ExtraTauIsol1_, L1ExtraTauIsol1);
	  event.getByLabel(L1ExtraTauIsol2_, L1ExtraTauIsol2);

	  edm::Handle<l1extra::L1JetParticleCollection>  L1ExtraTauRlx1,L1ExtraTauRlx2;
	  event.getByLabel(L1ExtraTauRlx1_, L1ExtraTauRlx1);
	  event.getByLabel(L1ExtraTauRlx2_, L1ExtraTauRlx2);

	  edm::Handle<l1extra::L1EtMissParticleCollection>  L1ExtraMET1,L1ExtraMET2;
	  event.getByLabel(L1ExtraMET1_, L1ExtraMET1);
	  event.getByLabel(L1ExtraMET2_, L1ExtraMET2);

	  edm::Handle<l1extra::L1EtMissParticleCollection>  L1ExtraMHT1,L1ExtraMHT2;
	  event.getByLabel(L1ExtraMHT1_, L1ExtraMHT1);
	  event.getByLabel(L1ExtraMHT2_, L1ExtraMHT2);

	  CompareL1Extra(* L1ExtraCenJet1, * L1ExtraCenJet2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hCenJet");
	  CompareL1Extra(* L1ExtraForJet1, * L1ExtraForJet2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hForJet");	  
	  CompareL1Extra(* L1ExtraEMIsol1, * L1ExtraEMIsol2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hIsolEm");
	  CompareL1Extra(* L1ExtraEMIsol1, * L1ExtraEMIsol2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hNonIsolEm");	  
	  CompareL1Extra(* L1ExtraTauIsol1, * L1ExtraTauIsol2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hIsolTau");
	  CompareL1Extra(* L1ExtraTauRlx1, * L1ExtraTauRlx2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hRlxTau");
	  CompareL1Extra(* L1ExtraMET1, * L1ExtraMET2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hMET");
	  CompareL1Extra(* L1ExtraMHT1, * L1ExtraMHT2,ev.id().run(),ev.luminosityBlock(), ev.id().event(),"hMHT");	  	  
	  

	} // end of myTrig block
      }  
      // close input file
      inFile->Close();
    }
    // break loop if maximal number of events is reached:
    // this has to be done twice to stop the file loop as well
    if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
  }

  // count number of lumiSections processed
  int nbins=m_HistNames["LumiBlocks"]->GetNbinsX();
  int nlumi(0);
  for (Int_t ibin=0; ibin<nbins; ++ibin){
    Float_t cont =m_HistNames["LumiBlocks"]->GetBinContent(ibin+1);
    if (cont>0)nlumi++;
  }
  std::cout<<"\nNumber of lumi sections processed: " << nlumi << std::endl;
  m_HistNames["hL1_1"]->Scale(1./(nlumi*23.3));
  m_HistNames["hL1_2"]->Scale(1./(nlumi*23.3));
  
  for (algoBitToName_iter_t it = algoBitToName.begin(); it != algoBitToName.end(); it++)
    {
      m_HistNames["hL1_1"]->GetXaxis()->SetBinLabel(it->second+1,it->first.c_str());
      m_HistNames["hL1_2"]->GetXaxis()->SetBinLabel(it->second+1,it->first.c_str());
    }
  
  cout << "\nTotal number of events processed: " << ievt<< endl;
  return 0;
}

//std::map <std::string,bool> getHLTResults(const edm::TriggerResults& hltresults,
//					  const edm::TriggerNames& triggerNames_){

trigmap_t getHLTResults(const edm::TriggerResults& hltresults,
					  const edm::TriggerNames& triggerNames_){

  trigmap_t hltTriggerMap;
  std::map<std::string,bool>::iterator trig_iter;

  int ntrigs=hltresults.size();
  // std::cout << "\nNumber of HLT Paths: " << ntrigs << "\n\n";

  for (int itrig = 0; itrig != ntrigs; ++itrig){
    string trigName = triggerNames_.triggerName(itrig);
    bool accept=hltresults.accept(itrig);
    
    //if (accept) h_TriggerResults->Fill(float(itrig));

    // fill the trigger map
    typedef std::map<string,bool>::value_type valType;
    trig_iter=hltTriggerMap.find(trigName);
    if (trig_iter==hltTriggerMap.end())
      hltTriggerMap.insert(valType(trigName,accept));
    else
      trig_iter->second=accept;
  }

  return hltTriggerMap;
}

algoBitToName_t FillAlgoToBitMap(const std::string& infile){

  algoBitToName_t l1Map;

  std::ifstream ifile;
  string line;
  ifile.open (infile.c_str());
  if (ifile.is_open())
  {
    while ( getline (ifile,line) )
    {
      if ( line.find("#") != 0){
	std::vector<std::string> contents;
	boost::split(contents, line, boost::is_any_of(":"));
	//cout << line << " " << contents.size() << "\t" << contents[0] << "\t" << contents[1] << '\n';
	if (contents.size() == 3 ){
	  cout << line << " " << contents.size() << '\n';
	  std::stringstream ss(contents.at(0));
	  int bit;
	  ss >> bit;
	  boost::erase_all(contents.at(1), " ");
	  l1Map[contents.at(1)] = bit;
	  // cout << contents.at(0) << "\t " << bit << "\t" << contents.at(1) << '\n';
	}
      }
    }
    ifile.close();
  }

  else cout << "Unable to open file"; 


  return l1Map;
}

template <typename T> void CompareL1Extra(const T& l1Extra1, const T& l1Extra2,
					  const int& run, const int& lumi ,const int& evt, std::string WhichColl)
{
  typedef typename T::const_iterator iter;

  // fill histograms
  for(iter l1 = l1Extra1.begin(); l1 != l1Extra1.end(); ++l1) {
    Double_t pt=l1->pt();
    //Double_t eta=l1->eta();
    //Double_t phi=l1->phi();
    std::string hname=WhichColl + "1_pt";
    fillHist(hname,pt);
  }

  for(iter l1 = l1Extra2.begin(); l1 != l1Extra2.end(); ++l1) {
    Double_t pt=l1->pt();
    //Double_t eta=l1->eta();
    //Double_t phi=l1->phi();
    std::string hname=WhichColl + "2_pt";
    fillHist(hname,pt);      
  }
  
  if (l1Extra1.size() != l1Extra2.size()){
    std::cout << WhichColl << ":  Run/Lumi/Evt: " << run << "\t" << lumi << "\t" << evt <<std::endl;
    std::cout << "\tL1Extra collection size mismatch! " << l1Extra1.size() << "\t" << l1Extra2.size() << std::endl;
    for(iter l1 = l1Extra1.begin(); l1 != l1Extra1.end(); ++l1) {
      Double_t pt1=l1->pt();
      Double_t eta1=l1->eta();
      Double_t phi1=l1->phi();
      std::cout << "\t L1Extra1 --  pt/eta/phi: " << pt1 << "\t" << eta1  << "\t" << phi1 << std::endl;
    }
    std::cout << "\n";
    for(iter l1 = l1Extra2.begin(); l1 != l1Extra2.end(); ++l1) {
      Double_t pt1=l1->pt();
      Double_t eta1=l1->eta();
      Double_t phi1=l1->phi();
      std::cout << "\t L1Extra2 --  pt/eta/phi: " << pt1 << "\t" << eta1  << "\t" << phi1 << std::endl;
    }
    std::cout << "\n";    
    return;
  }


  iter l1,l2;  
  // l1extra::L1EmParticleCollection::const_iterator l1,l2;
  l2=l1Extra2.begin();
  int nerrors(0);
  for(l1 = l1Extra1.begin(); l1 != l1Extra1.end(); ++l1) {
    Double_t pt1=l1->pt();
    Double_t eta1=l1->eta();
    Double_t phi1=l1->phi();

    Double_t pt2=l2->pt();
    Double_t eta2=l2->eta();
    Double_t phi2=l2->phi();
    ++l2;

    if ( (pt1>0.1 || pt2>0.1) &&
	 ( fabs(pt1-pt2)> 0.0001 || fabs(eta1-eta2)> 0.0001 || fabs(phi1-phi2)> 0.0001)){
      std::cout << WhichColl << ":  Run/Lumi/Evt: " << run << "\t" << lumi << "\t" << evt <<std::endl;
      nerrors++;
      if (pt1 != pt2) std::cout << "\tpT mismatch: \t" << pt1 << "\t" << pt2 << std::endl;
      if (eta1 != eta2) std::cout << "\teta mismatch: \t" << eta1 << "\t" << eta2 << std::endl;
      if (phi1 != phi2) std::cout << "\tphi mismatch: \t" << phi1 << "\t" << phi2 << std::endl;
    }
  }
  if (nerrors>0) std::cout << "\n";
}

void CompareGT(const L1GlobalTriggerReadoutRecord* gtrr1,const L1GlobalTriggerReadoutRecord* gtrr2){

  std::vector<ULong64_t> tw1_gtrr1,tw1_gtrr2;
  std::vector<ULong64_t> tw2_gtrr1,tw2_gtrr2;
  //std::vector<ULong64_t> tt_gtrr1,tt_gtrr2;
	
  tw1_gtrr1.resize(5,0);
  tw1_gtrr2.resize(5,0);
  tw2_gtrr1.resize(5,0);
  tw2_gtrr2.resize(5,0);  

  for(int iebx=0; iebx<5; iebx++)
  {
    DecisionWord gtDecisionWord1 = gtrr1->decisionWord(iebx-2);
    DecisionWord gtDecisionWord2 = gtrr2->decisionWord(iebx-2);

    int dbitNumber = 0;

    DecisionWord::const_iterator GTdbitItr;
    for(GTdbitItr = gtDecisionWord1.begin(); GTdbitItr != gtDecisionWord1.end(); GTdbitItr++) {
      if (*GTdbitItr) {
        if(dbitNumber<64) { tw1_gtrr1[iebx] |= (1LL<<dbitNumber); }
        else { tw2_gtrr1[iebx] |= (1LL<<(dbitNumber-64)); }
      }
      dbitNumber++; 
    }
    dbitNumber = 0;
    for(GTdbitItr = gtDecisionWord2.begin(); GTdbitItr != gtDecisionWord2.end(); GTdbitItr++) {
      if (*GTdbitItr) {
        if(dbitNumber<64) { tw1_gtrr2[iebx] |= (1LL<<dbitNumber); }
        else { tw2_gtrr2[iebx] |= (1LL<<(dbitNumber-64)); }
      }
      dbitNumber++; 
    }

  }

  for (int itrig=0; itrig<64; itrig++){
    // std::cout << "\t" << event_->hlt[itrig] << std::endl;
    Int_t BitFired1(0),BitFired2(0);
    for(int iebx=0; iebx<5; iebx++)
      {
	BitFired1 = BitFired1 + ((tw1_gtrr1[iebx]>>itrig)&1);
	BitFired2 = BitFired2 + ((tw1_gtrr2[iebx]>>itrig)&1);
      }
    if (BitFired1>0) fillHist("hL1_1",itrig,1.);
    if (BitFired2>0) fillHist("hL1_2",itrig,1.);

    BitFired1=0;
    BitFired2=0;
    for(int iebx=0; iebx<5; iebx++)
      {    
	BitFired1 = BitFired1 + ((tw2_gtrr1[iebx]>>itrig)&1);
	BitFired2 = BitFired2 + ((tw2_gtrr2[iebx]>>itrig)&1);
      }
    if (BitFired1>0) fillHist("hL1_1",itrig+64,1.);
    if (BitFired2>0) fillHist("hL1_2",itrig+64,1.);
  }
}

void  BookHistograms(TFileDirectory dir){

  TString hname;
  TString htitle;

  hname="LumiBlocks"; htitle="Events per Luminosity Block";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle,   2000,   0.,  2000.,false);

  hname="hL1_1"; htitle="Level-1 Trigger Bits Fired -- GTRR1";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 128,0,128,false);

  hname="hL1_2"; htitle="Level-1 Trigger Bits Fired -- GTRR2";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 128,0,128,false);

  hname="hIsolEm1_pt"; htitle="L1Extra1 p_{T} -- Isolated EGamma Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,64,false);
  hname="hIsolEm2_pt"; htitle="L1Extra2 p_{T} -- Isolated EGamma Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,64,false);

  hname="hNonIsolEm1_pt"; htitle="L1Extra1 -- p_{T} Non-Isolated EGamma Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,64,false);
  hname="hNonIsolEm2_pt"; htitle="L1Extra2 -- p_{T} Non-Isolated EGamma Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,64,false);

  hname="hCenJet1_pt"; htitle="L1Extra1 p_{T} -- Central Jets";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,254,false);
  hname="hForJet1_pt"; htitle="L1Extra1 p_{T} -- Forward Jets";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,254,false);
  
  hname="hIsolTau1_pt"; htitle="L1Extra1 p_{T} -- Isolated Tau Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,254,false);
  hname="hIsolTau2_pt"; htitle="L1Extra2 p_{T} -- Isolated Tau Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,254,false);

  hname="hRlxTau1_pt"; htitle="L1Extra1 -- p_{T} Relaxed Tau Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,254,false);
  hname="hRlxTau2_pt"; htitle="L1Extra2 -- p_{T} Relaxed Tau Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 64,0,254,false);

  hname="hMET1_pt"; htitle="L1Extra1 -- p_{T} MET Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 5096,0,2048,false);
  hname="hMET2_pt"; htitle="L1Extra2 -- p_{T} MET Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 5096,0,2048,false);

  hname="hMHT1_pt"; htitle="L1Extra1 -- p_{T} MHT Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 128,0,1.28,false);
  hname="hMHT2_pt"; htitle="L1Extra2 -- p_{T} MHT Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 128,0,1.28,false);

  hname="hETT1_pt"; htitle="L1Extra1 -- ETT Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 5096,0,2048,false);
  hname="hETT2_pt"; htitle="L1Extra2 -- ETT Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 5096,0,2048,false);

  hname="hHTT1_pt"; htitle="L1Extra1 -- HTT Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 5096,0,2048,false);
  hname="hHTT2_pt"; htitle="L1Extra2 -- HTT Candidates";
  m_HistNames[hname]=Book1dHist(dir,hname, htitle, 5096,0,2048,false);
  
}
