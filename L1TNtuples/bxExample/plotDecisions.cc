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

unsigned int ParseAlias(std::string alias){
  std::smatch base_match;
  std::regex integer("L1uGT\\.m_algoDecisionInitial\\[([0-9]+)\\]");
  unsigned int nbit = 0;

  if (std::regex_match(alias, base_match, integer))
  {
    nbit = std::stoi(base_match[1].str(), nullptr);
  }

  return nbit;
}       // -----  end of function L1uGT::ParseAlias  -----


void plot_rates(std::string infile, std::string outfile) {


  std::cout << "Infile: " << infile <<std::endl;
  std::cout << "Outfile: " << outfile <<std::endl;
  std::map<std::string,TH1F*> hTH1Fs;
  TFile* outrootfile =  new TFile( outfile.c_str(), "RECREATE");
  hTH1Fs["NEV"] = new TH1F("NEV","NEV; Number of events processed; ",1,-.5,.5);


  TFile * file = TFile::Open(infile.c_str());
  TTree * tree = (TTree *) file->Get("l1uGTTree/L1uGTTree");

  // extract the map between algo name  and bit number
  std::map<std::string, unsigned int> algoBitMap;
  std::vector<std::string> names;
  TList * aliases = tree->GetListOfAliases();
  TIter iter(aliases);
  std::for_each(iter.Begin(), TIter::End(), [&](TObject* alias){
      std::cout << alias->GetName() << "\t" << tree->GetAlias(alias->GetName()) << "\t" << ParseAlias(tree->GetAlias(alias->GetName()))<< std::endl;
      algoBitMap[alias->GetName()] = ParseAlias(tree->GetAlias(alias->GetName()));
      names.push_back(alias->GetName());
    } );

  //create output hists
  outrootfile->cd();
  // hTH1Fs["NEV2"] = new TH1F("NEV2","NEV; Number of events processed; ",1,-.5,.5);
  int numberTriggerBits = algoBitMap.size();
  hTH1Fs["L1ResultsInit"] = new TH1F("L1ResultsInit", "L1 Algo Trigger Results (Initial)", numberTriggerBits, 0, numberTriggerBits );
  int ibin=0;
  for (auto it = algoBitMap.begin(); it != algoBitMap.end(); ++it) {
    std::cout << it->first << ", " << it->second << '\n';
    const char* trigName =  it->first.c_str();
    hTH1Fs["L1ResultsInit"]->GetXaxis()->SetBinLabel(ibin+1,trigName);
    ibin++;
  }
  file->cd();
  //

  GlobalAlgBlk *l1uGT_        = new GlobalAlgBlk();
  tree->SetBranchAddress("L1uGT", &l1uGT_ );

  //unsigned long long entries = tree->GetEntries();
  Long64_t entries = tree->GetEntries();
  if (entries == 0)
    return;

  for (Long64_t jentry=0; jentry<entries;jentry++)
  {
    tree->GetEntry(jentry);
    hTH1Fs["NEV"]->Fill(0.);
    int ibin=0;
    for (auto it = algoBitMap.begin(); it != algoBitMap.end(); ++it) {
      //std::cout << it->first << ", " << it->second << '\n';
      if (l1uGT_->getAlgoDecisionInitial(it->second)>0)
	hTH1Fs["L1ResultsInit"]->Fill(ibin,1.);
      ibin++;
    }
    //std::cout <<"\t" <<l1uGT_->getAlgoDecisionInitial(458) << "\t" <<l1uGT_->getAlgoDecisionInitial(123) << std::endl;

  }// end of loop over entries

  int digits = std::log(entries) / std::log(10) + 1;
  for (auto const & name: names) {
    unsigned long long counts = tree->GetEntries(name.c_str());
    std::cout << std::setw(digits) << counts << " / " << std::setw(digits) << entries << "  " << name << std::endl;
  }

  file->Close();

  std::cout << "Closing input file" << std::endl;
  outrootfile->cd();
  for(auto h : hTH1Fs)
  {
     h.second->Write();
  }
  outrootfile->Close();

}


int main(int argc, char** argv) {
  if (argc < 3){
    std::cout << "%Please supply input and output root file names" << std::endl;
    return 0;
  }

  std::string infile=argv[1];
  std::string outfile=argv[2];
  plot_rates(infile,outfile);
}
