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
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/Math/interface/deltaR.h"

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TSystem.h>
#include <TFile.h>
#include <TCanvas.h>
#include <cmath>
#include <map>
#include <iostream>
#include <fstream>

using namespace edm;
using namespace std;

class L1Bits : public edm::EDAnalyzer {
public:
  L1Bits( const edm::ParameterSet & );

private:
  void beginJob();
  void analyze( const edm::Event& , const edm::EventSetup& );
  void endJob();

  void doL1Analysis();

  void getL1Results(const L1GlobalTriggerReadoutRecord&,
		    const L1GtTriggerMenu&
		  );

  edm::Service<TFileService> fs;

  std::string text_output, l1AlgoName;
  InputTag l1GtRecordInputTag;


  int errCnt;
  bool initL1, initAnalysis;

  const int errMax(){return 100;}

  TH1F* evtCounter;
  TH1F* h_L1AlgoResults, *h_L1TechResults;

  unsigned int numberTriggerBits;
  unsigned int numberTechnicalTriggerBits;

  static const size_t nL1BitsMax=128;
  string algoBitToName[nL1BitsMax];
  string techBitToName[nL1BitsMax];

  // use the map function to access trigger information
  std::map <string,bool> l1TriggerDecision,l1TechTriggerDecision;
  std::map<string,bool>::iterator trig_iter;

};

#endif
