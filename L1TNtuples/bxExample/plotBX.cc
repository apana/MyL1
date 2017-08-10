#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include <regex>

#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TList.h"
#include "TIterator.h"

#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "L1Trigger/L1TNtuples/interface/L1AnalysisEventDataFormat.h"


class BXPlots
{
public:
// constructor
  BXPlots(std::string infile, std::string outfile,std::string uGTTreeName, bool verbosity){

    std::cout << "\nInput file: " << infile << std::endl;
    std::cout << "Writing output to: " << outfile << "\n\n";
    verbose=verbosity;
    ofile =  new TFile( outfile.c_str(), "RECREATE");

    ifile = TFile::Open(infile.c_str());

    tree = (TTree *) ifile->Get(uGTTreeName.c_str());
    // extract the map between algo name  and bit number
    algoBitMap = GetBitMapping();

  }
  ~BXPlots() {
    writeHistograms();
    ofile->Close();
    std::cout << "Program execution terminated" << std::endl;
  }

  void loop(int);
  void bookHistograms();
  void writeHistograms();
  unsigned int ParseAlias(std::string);
  std::map<std::string, unsigned int> GetBitMapping();

private:
  TFile        *ifile, *ofile;
  TTree        *tree;
  std::map<std::string,TH1F*> hTH1Fs;
  std::map<std::string, unsigned int> algoBitMap;
  bool verbose;
};


unsigned int BXPlots::ParseAlias(std::string alias){
  std::smatch base_match;
  std::regex integer("L1uGT\\.m_algoDecisionInitial\\[([0-9]+)\\]");
  unsigned int nbit = 0;

  if (std::regex_match(alias, base_match, integer))
  {
    nbit = std::stoi(base_match[1].str(), nullptr);
  }

  return nbit;
}       // -----  end of function L1uGT::ParseAlias  -----

std::map<std::string, unsigned int> BXPlots::GetBitMapping(){

  std::map<std::string, unsigned int> bitMap;

  TList * aliases = tree->GetListOfAliases();
  TIter iter(aliases);
  if (verbose) std::cout << "\n";
  std::for_each(iter.Begin(), TIter::End(), [&](TObject* alias){
      if (verbose) std::cout << "\t" << alias->GetName() << "\t" << tree->GetAlias(alias->GetName()) << "\t" << ParseAlias(tree->GetAlias(alias->GetName()))<< std::endl;
      bitMap[alias->GetName()] = ParseAlias(tree->GetAlias(alias->GetName()));
    } );

  if (verbose) std::cout << "\n";
  return bitMap;
}

void BXPlots::bookHistograms(){

  ofile->cd();

  // generic histograms
  hTH1Fs["NEV"] = new TH1F("NEV","NEV; Number of events processed; ",1,-.5,.5);

  //create output hists
  for (auto it = algoBitMap.begin(); it != algoBitMap.end(); ++it) {
    const char* trigName =  it->first.c_str();
    hTH1Fs[trigName] = new TH1F(trigName, trigName, 5, -2.5, 2.5 );
  }

  ifile->cd();
}

void BXPlots::writeHistograms()
{

  ofile->cd();
  for(auto h : hTH1Fs)
  {
    h.second->Write();
  }
  return;
}       // -----  end of function L1Menu2016::WriteHistogram  -----

void BXPlots::loop(int maxEvents){

  GlobalAlgBlk *l1uGT_bx_minus2        = new GlobalAlgBlk();
  GlobalAlgBlk *l1uGT_bx_minus1        = new GlobalAlgBlk();
  GlobalAlgBlk *l1uGT_bx_central        = new GlobalAlgBlk();
  GlobalAlgBlk *l1uGT_bx_plus1        = new GlobalAlgBlk();
  GlobalAlgBlk *l1uGT_bx_plus2        = new GlobalAlgBlk();

  tree->SetBranchAddress("L1uGT_bx_minus2", &l1uGT_bx_minus2 );
  tree->SetBranchAddress("L1uGT_bx_minus1", &l1uGT_bx_minus1 );
  tree->SetBranchAddress("L1uGT_bx_central", &l1uGT_bx_central );
  tree->SetBranchAddress("L1uGT_bx_plus1", &l1uGT_bx_plus1 );
  tree->SetBranchAddress("L1uGT_bx_plus2", &l1uGT_bx_plus2 );

  //unsigned long long entries = tree->GetEntries();
  Long64_t entries = tree->GetEntries();
  if (entries == 0)
    return;

  if (maxEvents > 0 && entries > maxEvents) entries=maxEvents;
  for (Long64_t jentry=0; jentry<entries;jentry++)
  {
    tree->GetEntry(jentry);
    hTH1Fs["NEV"]->Fill(0.);

    for (auto it = algoBitMap.begin(); it != algoBitMap.end(); ++it) {
      //std::cout << it->first << ", " << it->second << '\n';
      const char* trigName =  it->first.c_str();
      if (l1uGT_bx_minus2->getAlgoDecisionInitial(it->second)>0) hTH1Fs[trigName]->Fill(-2.,1.);
      if (l1uGT_bx_minus1->getAlgoDecisionInitial(it->second)>0) hTH1Fs[trigName]->Fill(-1.,1.);
      if (l1uGT_bx_central->getAlgoDecisionInitial(it->second)>0) hTH1Fs[trigName]->Fill(0.,1.);
      if (l1uGT_bx_plus1->getAlgoDecisionInitial(it->second)>0) hTH1Fs[trigName]->Fill(1.,1.);
      if (l1uGT_bx_plus2->getAlgoDecisionInitial(it->second)>0) hTH1Fs[trigName]->Fill(2.,1.);

    }

  }// end of loop over entries

  std::cout << "Closing input file" << std::endl;
  ifile->Close();
}


int main(int argc, char** argv) {
  if (argc < 3){
    std::cout << "\n\tPlease supply input and output root file names" << std::endl;
    return 0;
  }

  std::string inFile=argv[1];
  std::string outFile=argv[2];

  //std::string uGTTreeName = "l1uGTTree/L1uGTTree";
  std::string uGTTreeName = "l1uGTEmuTree/L1uGTTree";
  //bool verbose(false);
  bool verbose(true);
  BXPlots bxPlots(inFile,outFile,uGTTreeName,verbose);


  bxPlots.bookHistograms();

  int maxEvt=-100;
  bxPlots.loop(maxEvt);

}
