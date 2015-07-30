#ifndef MyL1_L1Analyzer_L1Bits_h
#define MyL1_L1Analyzer_L1Bits_h
/* \class BitNumberToName
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

#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"

#include <iostream>
#include <iomanip>
#include <fstream>

using namespace edm;
using namespace std;

class BitNumbertoName : public edm::EDAnalyzer {
public:
  BitNumbertoName( const edm::ParameterSet & );
  
private:
  virtual void beginJob();
  virtual void beginRun(const edm::Run&, const edm::EventSetup&);
  virtual void analyze( const edm::Event& , const edm::EventSetup& );
  virtual void endJob();

  L1GtUtils m_l1GtUtils;

  std::string outFile, l1AlgoName;
  InputTag l1GtRecordInputTag;
  InputTag l1GtReadoutRecordInputTag;


  bool initL1, initAnalysis;

  const int errMax(){return 100;}

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
