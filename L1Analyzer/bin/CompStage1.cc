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


#include "DataFormats/Math/interface/deltaR.h"
#include "MyL1/L1Analyzer/interface/myRootIO.h"

typedef std::map<std::string,bool> trigmap_t;
typedef std::map<std::string,bool>::iterator trigmapiter_t;

typedef std::map<std::string,int> algoBitToName_t;
typedef std::map<std::string,int>::iterator algoBitToName_iter_t;

trigmap_t getHLTResults(const edm::TriggerResults&, const edm::TriggerNames&);
void  BookHistograms(TFileDirectory);
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
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("compStage1");
  edm::InputTag TriggerResults_( ana.getParameter<edm::InputTag>("TriggerResults") );
  edm::InputTag GtDigis1_( ana.getParameter<edm::InputTag>("GtDigis1") );
  edm::InputTag GtDigis2_( ana.getParameter<edm::InputTag>("GtDigis2") );
  string Trigger_( ana.getParameter<string>( "Trigger" ) );
  string TriggerBitMap_( ana.getParameter<string>( "TriggerBitMap" ) );
  //bool Debug_( ana.getParameter<bool>( "Debug" ) );
  //int TriggerPS_( ana.getParameter<int>( "TriggerPS" ) );
  uint BeginLumi_( ana.getParameter<uint>("BeginLumi") );
  uint EndLumi_  ( ana.getParameter<uint>("EndLumi") );

  // Prepare output file and book a set of histograms
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  TFileDirectory dir = fs.mkdir("compStage1");
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

  ifstream ifile;
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

}
