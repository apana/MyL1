#ifndef MYROOTIO_H
#define MYROOTIO_H

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TNamed.h"
#include <vector>
#include <map>


// use the map function to access the histograms
std::map<TString, TH1*> m_HistNames;
std::map<TString, TH1*>::iterator hid;

std::map<TString, TH2*> m_HistNames2D;
std::map<TString, TH2*>::iterator hid2D;


TH1F* Book1dHist(TFileDirectory dir,const char* name, const char* title, Int_t nbins, Double_t xmin, Double_t xmax,bool DoSumw2=true){


  TH1F *h= dir.make<TH1F>(name, title, nbins, xmin, xmax); 
  if (DoSumw2) h->Sumw2();

  return h;
}

void fillHist(const TString& histName, const Double_t& value, const Double_t& wt=1.) {

  hid=m_HistNames.find(histName);
  if (hid==m_HistNames.end())
    std::cout << "%fillHist -- Could not find histogram with name: " << histName << std::endl;
  else
    hid->second->Fill(value,wt); 

}

void sclHist(const TString& histName, const Double_t& wt) {

  hid=m_HistNames.find(histName);
  if (hid==m_HistNames.end())
    std::cout << "%fillHist -- Could not find histogram with name: " << histName << std::endl;
  else
    hid->second->Scale(wt); 

}

#endif
