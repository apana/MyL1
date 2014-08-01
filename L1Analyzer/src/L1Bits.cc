#include "MyL1/L1Analyzer/interface/L1Bits.h"

// Get the algorithm of the jet collections we will read from the .cfg file
// which defines the value of the strings CaloJetAlgorithm and GenJetAlgorithm.
L1Bits::L1Bits( const ParameterSet & cfg ) {
  cout << " Beginning L1Jet Analysis " << endl;
  l1GtRecordInputTag = cfg.getParameter<InputTag>( "L1GtRecordInputTag" );
  l1AlgoName         = cfg.getParameter<string> ("L1AlgoName");
  text_output        = cfg.getParameter<string>( "Outfile" );
  errCnt=0;
  initL1=false;
  initAnalysis=false;
  h_L1AlgoResults=0;
  h_L1TechResults=0;
}

void L1Bits::beginJob() {

  evtCounter=fs->make<TH1F>("EventCounter","Event Counter",5,0.,5.);

}

void L1Bits::analyze( const Event& evt, const EventSetup& es ) {

  string errMsg("");
  bool gotL1=true;

  evtCounter->Fill(0.);

  //Get the collections
  edm::ESHandle<L1GtTriggerMenu> menuRcd;
  es.get<L1GtTriggerMenuRcd>().get(menuRcd) ;

  Handle<L1GlobalTriggerReadoutRecord>   gtRecord;
  evt.getByLabel(l1GtRecordInputTag, gtRecord);

  if (!gtRecord.isValid()) {

    cout << "\nL1GlobalTriggerReadoutRecord with \n  "
	 << l1GtRecordInputTag << "\nnot found"
      "\n  --> returning false by default!\n" << endl;

    gotL1=false;
    
  }


  if (gotL1) {
    evtCounter->Fill(1.);
    getL1Results(*gtRecord.product(), *menuRcd.product());
    doL1Analysis();
  }
}

void L1Bits::doL1Analysis(){

    bool l1Fired=false;


    // fill histogram with L1 results
    if (!initAnalysis){
      initAnalysis=true;
      h_L1AlgoResults = fs->make<TH1F>( "h_L1AlgoResults", "L1 Algo Trigger Results", numberTriggerBits, 0, numberTriggerBits );
      trig_iter=l1TriggerDecision.begin();
      for ( unsigned int ibin = 0; ibin < numberTriggerBits; ++ ibin){
	trig_iter=l1TriggerDecision.find(algoBitToName[ibin]);
	const char* trigName =  trig_iter->first.c_str();
	h_L1AlgoResults->GetXaxis()->SetBinLabel(ibin+1,trigName);
      }
    }

    for ( unsigned int ibin = 0; ibin < numberTriggerBits; ++ ibin){
      trig_iter=l1TriggerDecision.find(algoBitToName[ibin]);
      bool accept = trig_iter->second;
      if (accept) h_L1AlgoResults->Fill(float(ibin));
    }


    // get results for particular L1 algorithm

    trig_iter=l1TriggerDecision.find(l1AlgoName);
    if (trig_iter==l1TriggerDecision.end()){
      cout << "Could not find trigger path with name: " << l1AlgoName << endl;
    }else{
      l1Fired=trig_iter->second;
    }
    if (l1Fired) evtCounter->Fill(2.);
}

void L1Bits::getL1Results(const L1GlobalTriggerReadoutRecord& gtRecord,
			  const L1GtTriggerMenu& menu
			) {

  // this will get the decision word *before* masking disabled bits
  const DecisionWord dWord = gtRecord.decisionWord();  
  numberTriggerBits= dWord.size();
  if (numberTriggerBits > nL1BitsMax) {
    cout << "number of Trigger Bits exceed max allowed in trigger array -- truncating" << "\n";
    numberTriggerBits = nL1BitsMax;
  }

  const TechnicalTriggerWord technicalTriggerWordBeforeMask = gtRecord.technicalTriggerWord();
  numberTechnicalTriggerBits = technicalTriggerWordBeforeMask.size();
  if (numberTechnicalTriggerBits > nL1BitsMax) {
    cout << "number of Technical Trigger Bits exceed max allowed in trigger array -- truncating" << "\n";
    numberTechnicalTriggerBits = nL1BitsMax;
  }


  if ( !initL1){
    initL1=true;
    cout << "\n  Number of Algorithm Trigger bits " << numberTriggerBits << "\n\n";
    cout << "\tBit \t L1 Algorithm " << endl;

    // get L1 menu from event setup
    for (CItAlgo algo = menu.gtAlgorithmMap().begin(); algo!=menu.gtAlgorithmMap().end(); ++algo) {
      cout << "Name: " << (algo->second).algoName() << " Alias: " << (algo->second).algoAlias() << endl;
      int itrig = (algo->second).algoBitNumber();
      algoBitToName[itrig] = TString( (algo->second).algoName() );
    }

    // technical trigger bits
    cout << "\n  Number of Technical Trigger bits " << numberTechnicalTriggerBits << "\n\n";
    for (CItAlgo techTrig = menu.gtTechnicalTriggerMap().begin(); techTrig != menu.gtTechnicalTriggerMap().end(); ++techTrig) {
      int itrig = (techTrig->second).algoBitNumber();
      techBitToName[itrig] = TString( (techTrig->second).algoName() );
      cout << "tech bit " << itrig << ": " << techBitToName[itrig] << " " << endl;
      
    } // end of if
  }

  //Fill maps with trigger bit info
  for (unsigned int iBit = 0; iBit < numberTriggerBits; ++iBit) {  

    bool accept = dWord[iBit];

      // fill the trigger map
    typedef std::map<string,bool>::value_type valType;
    trig_iter=l1TriggerDecision.find(algoBitToName[iBit]);
    if (trig_iter==l1TriggerDecision.end())
      l1TriggerDecision.insert(valType(algoBitToName[iBit],accept));
    else
      trig_iter->second=accept;
    }

  //now technical triggerbits
  for (unsigned int iBit = 0; iBit < numberTechnicalTriggerBits; ++iBit) {  

    bool accept = technicalTriggerWordBeforeMask[iBit];

      // fill the trigger map
    typedef std::map<string,bool>::value_type valType;
    trig_iter=l1TechTriggerDecision.find(techBitToName[iBit]);
    if (trig_iter==l1TechTriggerDecision.end())
      l1TechTriggerDecision.insert(valType(techBitToName[iBit],accept));
    else
      trig_iter->second=accept;
    }
}

void L1Bits::endJob() {

  double ntot=evtCounter->GetBinContent(1);
  double nacc=evtCounter->GetBinContent(2);

  if (h_L1AlgoResults){

    cout << "XXX" << endl;
    ofstream ofile;
    ofile.open (text_output.c_str());

    int nbins=h_L1AlgoResults->GetNbinsX(); 

    cout  << "Number of L1 Triggers: " << nbins << "\n\n";
    ofile << "Number of L1 Triggers: " << nbins << "\n\n";

    cout  << "\tL1 Algorithm \t\t # of Accepts" << "\n";
    cout  << "\t------------ \t\t ------------" << "\n";
    ofile << "\tL1 Algorithm \t\t # of Accepts" << "\n";
    ofile << "\t------------ \t\t ------------" << "\n";
    for (int ibin=0; ibin<nbins; ++ibin){
      float cont=h_L1AlgoResults->GetBinContent(ibin+1);
      string trigName = string (h_L1AlgoResults->GetXaxis()->GetBinLabel(ibin+1));
      
      if (!trigName.empty()){
	cout  << "\t" << trigName << ":\t" << cont << endl;
	ofile << "\t" << trigName << ":\t" << cont << endl;
      }
    }
    
    ofile << "\n";
    ofile << "Number of Events Processed  : " << ntot << "\n";
    ofile << "Number of L1 Accepted Events: " << nacc << "\n";
    ofile << "                      Ratio : " << nacc/ntot << endl;
    
    ofile.close();
  }

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1Bits);
