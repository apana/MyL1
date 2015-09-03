#include "MyL1/L1Analyzer/interface/BitNumbertoName.h"


#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"

static const bool useL1EventSetup(true);
static const bool useL1GtTriggerMenuLite(false);

// Get the algorithm of the jet collections we will read from the .cfg file
// which defines the value of the strings CaloJetAlgorithm and GenJetAlgorithm.
//BitNumbertoName::BitNumbertoName( const ParameterSet & cfg ) :
//  m_l1GtUtils(cfg, consumesCollector(), useL1GtTriggerMenuLite, *this)
BitNumbertoName::BitNumbertoName( const ParameterSet & cfg )
{
  cout << " Beginning Analysis " << endl;
  outFile        = cfg.getParameter<string>( "BitsAndPrescales" );
  initL1=false;
  initAnalysis=false;
}

void BitNumbertoName::beginJob() {

}

void BitNumbertoName::beginRun( const Run& iRun, const EventSetup& evSetup ) {

  // m_l1GtUtils.retrieveL1EventSetup(evSetup);
  m_l1GtUtils.getL1GtRunCache(iRun, evSetup, useL1EventSetup,
			      useL1GtTriggerMenuLite);

}

void BitNumbertoName::analyze( const Event& iEvent, const EventSetup& evSetup ) {

  string errMsg("");

  bool useL1EventSetup = true;
  bool useL1GtTriggerMenuLite = false;

  // m_l1GtUtils.retrieveL1EventSetup(evSetup);
  m_l1GtUtils.getL1GtRunCache(iEvent, evSetup, useL1EventSetup,
			      useL1GtTriggerMenuLite);


  edm::ESHandle<L1GtTriggerMenu> l1GtMenu;
  evSetup.get<L1GtTriggerMenuRcd>().get(l1GtMenu);
  const L1GtTriggerMenu* m_l1GtMenu = l1GtMenu.product();  
  const AlgorithmMap* m_algorithmMap = &(m_l1GtMenu->gtAlgorithmMap());
  
  unsigned int maxLen=0;
  for (CItAlgo algo = m_algorithmMap->begin(); algo!=m_algorithmMap->end(); ++algo) {
    // cout << "Name: " << (algo->second).algoName() << " Alias: " << (algo->second).algoAlias() << endl;
    int itrig = (algo->second).algoBitNumber();
    algoBitToName[itrig] = (algo->second).algoName();
    if (algoBitToName[itrig].length()>maxLen) maxLen=algoBitToName[itrig].length();
    }


  cout << "\nL1 trigger menu: \n" << m_l1GtUtils.l1TriggerMenu()
	       << std::endl;
  
  cout
    << "\n******** Results found with input tags retrieved from provenance ******** \n"
    << std::endl;

  // access L1 trigger results using public methods from L1GtUtils
  // always check on error code returned by that method
  // std::string triggerAlgTechTrig = "PhysicsAlgorithms";
  
  L1GtUtils::TriggerCategory trigCategory = L1GtUtils::AlgorithmTrigger;

  int iErrorCode = -1;
  const int pfSetIndexPhysicsAlgorithms = m_l1GtUtils.prescaleFactorSetIndex(
					iEvent, trigCategory, iErrorCode);

  if (iErrorCode == 0) {
    cout << "%Prescale set index: " <<  pfSetIndexPhysicsAlgorithms << endl;
  }else{
    cout << "%Could not extract Prescale set index from event record. Error code: " << iErrorCode << endl;
  }


  iErrorCode = -1;


  //  const std::vector<int>& pfSetPhysicsAlgorithms =
  //    m_l1GtUtils.prescaleFactorSet(iEvent, 
  //				  l1GtRecordInputTag,
  //				  l1GtReadoutRecordInputTag,
  //				  triggerAlgTechTrig,
  //				  iErrorCode);

  const std::vector<int>& pfSetPhysicsAlgorithms =
    m_l1GtUtils.prescaleFactorSet(iEvent, 
				  trigCategory,
				  iErrorCode);

  if (iErrorCode == 0) {


    //ccla add stuff so we can get the name corresponding to bit number
    //  edm::InputTag l1GtRecordInputTag;
    //  edm::InputTag l1GtReadoutRecordInputTag;
    //  m_l1GtUtils.getL1GtRecordInputTag(iEvent, l1GtRecordInputTag, l1GtReadoutRecordInputTag);
    //  cout << "l1GtRecordInputTag :" << l1GtRecordInputTag << endl;
    //  cout << "l1GtReadoutRecordInputTag :" << l1GtReadoutRecordInputTag << endl;
    //ccla end 

    cout << "\nPhysics algorithms: prescale factor set for run "
	 << iEvent.run() << ", luminosity block "
	 << iEvent.luminosityBlock() << ", with L1 menu \n  "
	 << m_l1GtUtils.l1TriggerMenu() << "\n" << std::endl;
    

    ofstream ofile;
    ofile.open (outFile.c_str());

    ofile <<"# Bit"<< std::right << std::setw(maxLen) << "Trigger Name" << "   Prescale" << endl;
    int iBit = -1;
    for (std::vector<int>::const_iterator cItBit =
	   pfSetPhysicsAlgorithms.begin(); cItBit
	   != pfSetPhysicsAlgorithms.end(); ++cItBit) {
    
      iBit++;
      //cout << "Bit number " << std::right << std::setw(4) << iBit
      //   << ": "          << std::right << std::setw(maxLen) << algoBitToName[iBit] 
      //   << ": prescale factor = " << (*cItBit) << endl;

      ofile << std::right << std::setw(4) << iBit
	   << ": "          << std::right << std::setw(maxLen) << algoBitToName[iBit] 
	   << ": " << (*cItBit) << endl;

    }
    ofile.close();    


  } else if (iErrorCode == 1) {
        
      // algorithm / technical trigger  does not exist in the L1 menu
      // do something
    cout << "Trouble -- error code: " << iErrorCode << endl;
  } else {

      // error - see error code
      // do whatever needed
    cout << "Trouble -- error code: " << iErrorCode << endl;
  }

}


void BitNumbertoName::endJob() {


}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(BitNumbertoName);
