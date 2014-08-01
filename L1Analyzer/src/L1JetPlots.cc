// JetPlotsExample.cc
// Description:  Example of simple EDAnalyzer for jets.
// Author: Robert M. Harris
// Date:  28 - August - 2006
//

#include "MyL1/L1Analyzer/interface/L1JetPlots.h"

// Get the algorithm of the jet collections we will read from the .cfg file
// which defines the value of the strings CaloJetAlgorithm and GenJetAlgorithm.
L1JetPlots::L1JetPlots( const ParameterSet & cfg ) {
  cout << " Beginning L1Jet Analysis " << endl;
  CaloJetAlgorithm = cfg.getParameter<InputTag>( "CaloJetAlgorithm" );
  PFJetAlgorithm   = cfg.getParameter<InputTag>( "PFJetAlgorithm"  );
  GenJetAlgorithm  = cfg.getParameter<InputTag>( "GenJetAlgorithm" );
  recmet_          = cfg.getParameter< InputTag > ("recmet");
  genmet_          = cfg.getParameter< InputTag > ("genmet");
  l1CollectionsTag_= cfg.getParameter< InputTag > ("l1collections");

  errCnt=0;
  nL1Jet8=0;
  nL1Jet12=0;
  nL1Jet16=0;
  nL1Jet36=0;
  nL1Jet52=0;
  nL1Jet68=0;
  nL1Jet92=0;
  nL1Jet128=0;

  nL1TauJet20=0;

}

void L1JetPlots::beginJob() {

  ptCal =  fs->make<TH1F>( "ptCal",  "p_{T} of leading CaloJets", 500, 0, 500 );
  etaCal = fs->make<TH1F>( "etaCal", "#eta of leading CaloJets", 52, -5.2, 5.2 );
  phiCal = fs->make<TH1F>( "phiCal", "#phi of leading CaloJets", 50, -M_PI, M_PI );
  ptGen =  fs->make<TH1F>( "ptGen",  "p_{T} of leading GenJets", 500, 0, 500 );
  etaGen = fs->make<TH1F>( "etaGen", "#eta of leading GenJets", 52, -5.2, 5.2 );
  phiGen = fs->make<TH1F>( "phiGen", "#phi of leading GenJets", 50, -M_PI, M_PI );

  ptCalL    =  fs->make<TH1F>( "ptCalL",    "p_{T} of leading CaloJets", 500, 0, 500 );
  ptCalL8   =  fs->make<TH1F>( "ptCalL8",   "p_{T} of leading CaloJets -- L1Jet8", 500, 0, 500 );
  ptCalL12  =  fs->make<TH1F>( "ptCalL12",  "p_{T} of leading CaloJets -- L1Jet12", 500, 0, 500 );
  ptCalL16  =  fs->make<TH1F>( "ptCalL16",  "p_{T} of leading CaloJets -- L1Jet16", 500, 0, 500 );
  ptCalL36  =  fs->make<TH1F>( "ptCalL36",  "p_{T} of leading CaloJets -- L1Jet36", 500, 0, 500 );
  ptCalL52  =  fs->make<TH1F>( "ptCalL52",  "p_{T} of leading CaloJets -- L1Jet52", 500, 0, 500 );
  ptCalL68  =  fs->make<TH1F>( "ptCalL68",  "p_{T} of leading CaloJets -- L1Jet68", 500, 0, 500 );
  ptCalL92  =  fs->make<TH1F>( "ptCalL92",  "p_{T} of leading CaloJets -- L1Jet92", 500, 0, 500 );
  ptCalL128 =  fs->make<TH1F>( "ptCalL128",  "p_{T} of leading CaloJets -- L1Jet128", 500, 0, 500 );

  ptPFL    =  fs->make<TH1F>( "ptPFL",    "p_{T} of leading PFJets", 500, 0, 500 );
  ptPFL8   =  fs->make<TH1F>( "ptPFL8",   "p_{T} of leading PFJets -- L1Jet8", 500, 0, 500 );
  ptPFL12  =  fs->make<TH1F>( "ptPFL12",  "p_{T} of leading PFJets -- L1Jet12", 500, 0, 500 );
  ptPFL16  =  fs->make<TH1F>( "ptPFL16",  "p_{T} of leading PFJets -- L1Jet16", 500, 0, 500 );
  ptPFL36  =  fs->make<TH1F>( "ptPFL36",  "p_{T} of leading PFJets -- L1Jet36", 500, 0, 500 );
  ptPFL52  =  fs->make<TH1F>( "ptPFL52",  "p_{T} of leading PFJets -- L1Jet52", 500, 0, 500 );
  ptPFL68  =  fs->make<TH1F>( "ptPFL68",  "p_{T} of leading PFJets -- L1Jet68", 500, 0, 500 );
  ptPFL92  =  fs->make<TH1F>( "ptPFL92",  "p_{T} of leading PFJets -- L1Jet92", 500, 0, 500 );
  ptPFL128 =  fs->make<TH1F>( "ptPFL128",  "p_{T} of leading PFJets -- L1Jet128", 500, 0, 500 );

  int nbins=50;
  Double_t min=0.,max=150.;

  MetPt = fs->make<TH1F>( "MetPt", "Reconstructed MET ", nbins, min, max );
  genMetPt = fs->make<TH1F>( "genMetPt", "Generated MET ", nbins, min, max );

  TString hname,htitle;

  hname="L1JetCollSize"; htitle="L1 Jet Collection Size";
  m_HistNames[hname] = fs->make<TH1F>( hname , htitle  , 10, -0.5, 9.5 );

  Int_t nptbins=200;
  Double_t ptmin=0,ptmax=200.;
  hname="L1JetPt_Cen"; htitle="L1 Jet p_{T} -- Central";
  m_HistNames[hname] = fs->make<TH1F>( hname , htitle  , nptbins, ptmin, ptmax );
  m_HistNames[hname]->Sumw2();
  hname="L1JetPt_Tau"; htitle="L1 Jet p_{T} -- Tau";
  m_HistNames[hname] = fs->make<TH1F>( hname , htitle  , nptbins, ptmin, ptmax );
  m_HistNames[hname]->Sumw2();
  hname="L1JetPt_For"; htitle="L1 Jet p_{T} -- Forward";
  m_HistNames[hname] = fs->make<TH1F>( hname , htitle  , nptbins, ptmin, ptmax );
  m_HistNames[hname]->Sumw2();


  Int_t nphibins=20;
  Double_t phimin=-M_PI,phimax=M_PI;
  hname="L1JetPhi_Cen"; htitle="L1 Jet #phi -- Central";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , nphibins, phimin, phimax );
  m_HistNames[hname]->Sumw2();
  hname="L1JetPhi_Tau"; htitle="L1 Jet #phi -- Tau";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , nphibins, phimin, phimax );
  m_HistNames[hname]->Sumw2();
  hname="L1JetPhi_For"; htitle="L1 Jet #phi --Forward";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , nphibins, phimin, phimax );
  m_HistNames[hname]->Sumw2();


  Int_t netabins=52;
  Double_t etamin=-5.2,etamax=5.2;
  hname="L1JetEta_Cen"; htitle="L1 Jet #eta -- Central";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , netabins, etamin, etamax );  m_HistNames[hname]->Sumw2();
  hname="L1JetEta_Tau"; htitle="L1 Jet #eta -- Tau";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , netabins, etamin, etamax );  m_HistNames[hname]->Sumw2();
  hname="L1JetEta_For"; htitle="L1 Jet #eta -- Forward";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , netabins, etamin, etamax );  m_HistNames[hname]->Sumw2();


  hname="L1DeltaR_Calo"; htitle="Delta R -- L1 and CaloJets";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , 100, 0., 10. );
  m_HistNames[hname]->Sumw2();

  hname="L1DeltaR_Gen"; htitle="Delta R -- L1 and GenJets";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , 100, 0., 10. );
  m_HistNames[hname]->Sumw2();

  hname="L1PtOverCaloPt_Calo";htitle=" L1 Jet p_{T} over CaloJet p_{T}";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , 40, 0., 2.0 );
  m_HistNames[hname]->Sumw2();

  hname="L1PtOverCaloPt_Gen";htitle=" L1 Jet p_{T} over GenJet p_{T}";
  m_HistNames[hname] =  fs->make<TH1F>( hname , htitle  , 40, 0., 2.0 );
  m_HistNames[hname]->Sumw2();

  Int_t n2dbins=50;
  hname="L1etaVSJeteta_Calo"; htitle="L1 Jet #eta vs CaloJet #eta -- Central";
  m_HistNames2D[hname] = fs->make<TH2F>( hname , htitle  , n2dbins, etamin, etamax, n2dbins, etamin, etamax );
  hname="L1etaVSJeteta_Gen"; htitle="L1 Jet #eta vs GenJet #eta -- Central";
  m_HistNames2D[hname] = fs->make<TH2F>( hname , htitle  , n2dbins, etamin, etamax, n2dbins, etamin, etamax );


  hname="L1phiVSJetphi_Calo"; htitle="L1 Jet #phi vs CaloJet #phi -- Central";
  m_HistNames2D[hname] = fs->make<TH2F>( hname , htitle  , n2dbins, phimin, phimax, n2dbins, phimin, phimax );
  hname="L1phiVSJetphi_Gen"; htitle="L1 Jet #phi vs GenJet #phi -- Central";
  m_HistNames2D[hname] = fs->make<TH2F>( hname , htitle  , n2dbins, phimin, phimax, n2dbins, phimin, phimax );

  hname="L1ptVSJetpt_Calo"; htitle="L1 Jet p_{T} vs CaloJet p_{T} -- Central";
  m_HistNames2D[hname] = fs->make<TH2F>( hname , htitle  , n2dbins, ptmin, ptmax, n2dbins, ptmin, ptmax );
  hname="L1ptVSJetpt_Gen"; htitle="L1 Jet p_{T} vs GenJet p_{T} -- Central";
  m_HistNames2D[hname] = fs->make<TH2F>( hname , htitle  , n2dbins, ptmin, ptmax, n2dbins, ptmin, ptmax );

}

void L1JetPlots::analyze( const Event& evt, const EventSetup& es ) {

  string errMsg("");
  doCaloJets=true; doGenJets=true; doCaloMET=true; doGenMET=true, doL1Jets=true; doPFJets=true;

  //Get the collections
  Handle<CaloJetCollection> caloJets, caloJetsDummy;
  Handle<PFJetCollection> pfJets, pfJetsDummy;
  Handle<GenJetCollection> genJets, genJetsDummy;
  Handle<CaloMETCollection> recmet, recmetDummy;
  Handle<GenMETCollection>  genmet, genmetDummy;
  Handle<l1extra::L1JetParticleCollection> l1CenJets,l1ForJets,l1TauJets,l1jetsDummy;

  evt.getByLabel( CaloJetAlgorithm, caloJets );
  evt.getByLabel( PFJetAlgorithm,  pfJets );
  evt.getByLabel( GenJetAlgorithm, genJets );
  evt.getByLabel( recmet_,recmet );
  evt.getByLabel( genmet_,genmet );

  if (!caloJets.isValid()) { errMsg=errMsg + "  -- No CaloJets"; caloJets = caloJetsDummy; doCaloJets =false;}
  if (!pfJets.isValid())   { errMsg=errMsg + "  -- No PFJets";  pfJets   = pfJetsDummy  ; doPFJets   =false;}
  if (!genJets.isValid())  { errMsg=errMsg + "  -- No GenJets"; genJets  = genJetsDummy ; doGenJets  =false;}
  if (!recmet.isValid())   { errMsg=errMsg + "  -- No RecMET" ; recmet   = recmetDummy  ; doCaloMET  =false;}
  if (!genmet.isValid())   { errMsg=errMsg + "  -- No GenMET" ; genmet   = genmetDummy  ; doGenMET   =false;}

  InputTag L1CenJetTag(edm::InputTag(l1CollectionsTag_.label(),"Central"));
  //InputTag L1CenJetTag(edm::InputTag(l1CollectionsTag_.label(),"cenJets"));
  evt.getByLabel(L1CenJetTag,l1CenJets);
  if (! l1CenJets.isValid()) { errMsg=errMsg + "  -- No L1Jets with name: " + L1CenJetTag.label() ;
    l1CenJets = l1jetsDummy; doL1Jets=false;}

  InputTag L1ForJetTag(edm::InputTag(l1CollectionsTag_.label(),"Forward"));
  //InputTag L1ForJetTag(edm::InputTag(l1CollectionsTag_.label(),"forJets"));
  evt.getByLabel(L1ForJetTag,l1ForJets);
  if (! l1ForJets.isValid()) { errMsg=errMsg + "  -- No L1Jets with name: " + L1ForJetTag.label() ;
    l1ForJets = l1jetsDummy; doL1Jets=false;}

  InputTag L1TauJetTag(edm::InputTag(l1CollectionsTag_.label(),"Tau"));
  //InputTag L1TauJetTag(edm::InputTag(l1CollectionsTag_.label(),"tauJets"));
  evt.getByLabel(L1TauJetTag,l1TauJets);
  if (! l1TauJets.isValid()) { errMsg=errMsg + "  -- No L1Jets with name: " + L1TauJetTag.label() ;
    l1TauJets = l1jetsDummy; doL1Jets=false;}


  if (doCaloJets){
    //Loop over the two leading CaloJets and fill some histograms
    int jetInd = 0;
    for( CaloJetCollection::const_iterator cal = caloJets->begin(); cal != caloJets->end() && jetInd<2; ++ cal ) {
      // std::cout << "CALO JET #" << jetInd << std::endl << cal->print() << std::endl;
      ptCal->Fill( cal->pt() );
      etaCal->Fill( cal->eta() );
      phiCal->Fill( cal->phi() );
      jetInd++;
    }
  }

  if (doGenJets){
    //Loop over the two leading GenJets and fill some histograms
    int jetInd = 0;
    for( GenJetCollection::const_iterator gen = genJets->begin(); gen != genJets->end() && jetInd<2; ++ gen ) {
      // std::cout << "GEN JET #" << jetInd << std::endl << gen->print() << std::endl;
      ptGen->Fill( gen->pt() );
      etaGen->Fill( gen->eta() );
      phiGen->Fill( gen->phi() );
      jetInd++;
    }
  }

  if (doCaloMET){
    for( CaloMETCollection::const_iterator met = recmet->begin(); met != recmet->end() ; ++ met ) {
      MetPt->Fill(met->pt());
    }
  }

  if (doGenMET){
    for( GenMETCollection::const_iterator met = genmet->begin(); met != genmet->end() ; ++ met ) {
      genMetPt->Fill(met->pt());
    }
  }

  if (doL1Jets) L1Analysis(*caloJets,*pfJets,*genJets,*l1CenJets,*l1ForJets,*l1TauJets);

  if ((errMsg != "") && (errCnt < errMax())){
    errCnt=errCnt+1;
    errMsg=errMsg + ".";
    cout << "%L1JetPlots-Warning" << errMsg << endl;
    if (errCnt == errMax()){
      errMsg="%L1JetPLots-Warning -- Maximum error count reached -- No more messages will be printed.";
      cout << errMsg << endl;
    }
  }

}

void L1JetPlots::L1Analysis(const reco::CaloJetCollection& caloJets,
			    const reco::PFJetCollection& pfJets,
			    const reco::GenJetCollection& genJets,
			    const l1extra::L1JetParticleCollection& l1CenJets,
			    const l1extra::L1JetParticleCollection& l1ForJets,
			    const l1extra::L1JetParticleCollection& l1TauJets
			    ) {

  //CalJetIter cIter;
  //GenJetIter gIter;

  // cout << "%doL1Analysis -- Number of l1jets:   " << l1ForJets.size() << endl;
  //cout << "%doL1Analysis -- Number of calojets: " << caloJets.size() << endl;
  //cout << "%doL1Analysis -- Number of pfjets: " << pfJets.size() << endl;


  fillHist("L1JetCollSize",l1ForJets.size());

  for(l1extra::L1JetParticleCollection::const_iterator l1 = l1ForJets.begin(); l1 != l1ForJets.end(); ++l1) {
    
    Double_t pt_l1=l1->pt();
    Double_t eta_l1=l1->eta();
    Double_t phi_l1=l1->phi();
    
    fillHist("L1JetPt_For",pt_l1);
    fillHist("L1JetPhi_For",phi_l1);
    fillHist("L1JetEta_For",eta_l1);
    
    // match L1 Jets with Jets
    if (doCaloJets) mtchL1(eta_l1,phi_l1,pt_l1,caloJets,"_Calo");
    if (doGenJets)  mtchL1(eta_l1,phi_l1,pt_l1,genJets,"_Gen");
  }

  for(l1extra::L1JetParticleCollection::const_iterator l1 = l1TauJets.begin(); l1 != l1TauJets.end(); ++l1) {
    
    Double_t pt_l1=l1->pt();
    Double_t eta_l1=l1->eta();
    Double_t phi_l1=l1->phi();
    
    fillHist("L1JetPt_Tau",pt_l1);
    fillHist("L1JetPhi_Tau",phi_l1);
    fillHist("L1JetEta_Tau",eta_l1);
    
  }

  for(l1extra::L1JetParticleCollection::const_iterator l1 = l1CenJets.begin(); l1 != l1CenJets.end(); ++l1) {
    
    Double_t pt_l1=l1->pt();
    Double_t eta_l1=l1->eta();
    Double_t phi_l1=l1->phi();
    
    fillHist("L1JetPt_Cen",pt_l1);
    fillHist("L1JetPhi_Cen",phi_l1);
    fillHist("L1JetEta_Cen",eta_l1);
    
  }

  // try to recreate the jet bits
  double maxL1Cen=0., maxL1For=0., maxL1Tau=0.;
  if (l1CenJets.size()>0) maxL1Cen=l1CenJets.at(0).pt();
  if (l1ForJets.size()>0) maxL1For=l1ForJets.at(0).pt();
  if (l1TauJets.size()>0) maxL1Tau=l1TauJets.at(0).pt();

  double maxL1=maxL1Cen;
  if (maxL1For>maxL1) maxL1=maxL1For;
  if (maxL1Tau>maxL1) maxL1=maxL1Tau;

  if (maxL1>=16.) nL1Jet16++;
  if (maxL1>=36.) nL1Jet36++;
  if (maxL1>=52.) nL1Jet52++;
  if (maxL1>=68.) nL1Jet68++;
  if (maxL1>=92.) nL1Jet92++;
  if (maxL1>=128.) nL1Jet128++;

  if (maxL1Tau>=20.) nL1TauJet20++;

  if (doCaloJets && caloJets.size()>0){
    CaloJetCollection::const_iterator cal = caloJets.begin();

    if (checkCaloJetID(cal)){
      ptCalL->Fill( cal->pt() );
      if (maxL1>=8. )ptCalL8 ->Fill( cal->pt() );
      if (maxL1>=12.)ptCalL12->Fill( cal->pt() );
      if (maxL1>=16.)ptCalL16->Fill( cal->pt() );
      if (maxL1>=36.)ptCalL36->Fill( cal->pt() );
      if (maxL1>=52.)ptCalL52->Fill( cal->pt() );
      if (maxL1>=68.)ptCalL68->Fill( cal->pt() );
      if (maxL1>=92.)ptCalL92->Fill( cal->pt() );
      if (maxL1>=128.)ptCalL128->Fill( cal->pt() );
    }
  }

  if (doPFJets && pfJets.size()>0){
    PFJetCollection::const_iterator pf = pfJets.begin();
    if (checkPFJetID(pf)){
      ptPFL->Fill( pf->pt() );
      if (maxL1>=8. )ptPFL8 ->Fill( pf->pt() );
      if (maxL1>=12.)ptPFL12->Fill( pf->pt() );
      if (maxL1>=16.)ptPFL16->Fill( pf->pt() );
      if (maxL1>=36.)ptPFL36->Fill( pf->pt() );
      if (maxL1>=52.)ptPFL52->Fill( pf->pt() );
      if (maxL1>=68.)ptPFL68->Fill( pf->pt() );
      if (maxL1>=92.)ptPFL92->Fill( pf->pt() );
      if (maxL1>=128.)ptPFL128->Fill( pf->pt() );
    }
  }
}

template <typename T> void L1JetPlots::mtchL1(const Double_t& eta_l1, 
						  const Double_t& phi_l1, 
						  const Double_t& pt_l1, 
						  const T& jets,
						  const TString& WhichJets) {

  // kick out if Jet collection does not exist
  if (! &jets) return;

  double etaCut=2.5;
  double drCut=0.5;


  typedef typename T::const_iterator iter;
  iter mIter;

  float rmin(99.);
  for ( iter jiter=jets.begin(); jiter!=jets.end(); jiter++) {
    
    if (fabs(jiter->eta()) < etaCut){

      Double_t pt_jet=jiter->pt();
      Double_t eta_jet=jiter->eta();
      Double_t phi_jet=jiter->phi();
      
      float dr=deltaR(eta_jet,phi_jet,eta_l1,phi_l1);
      if (pt_l1>20 && pt_jet>10) fillHist("L1DeltaR" + WhichJets,dr);
      
      if(dr<rmin){rmin=dr;mIter=jiter;}
      
    }  
  }

  if (rmin < drCut){
    fill2DHist("L1etaVSJeteta" + WhichJets,mIter->eta(),eta_l1,1.);
    fill2DHist("L1phiVSJetphi" + WhichJets,mIter->phi(),phi_l1,1.);
    fill2DHist("L1ptVSJetpt" + WhichJets,mIter->pt(),pt_l1,1.);
    
    if (mIter->pt()>75.) {
      fillHist("L1PtOverCaloPt" + WhichJets,pt_l1/(mIter->pt()));
    }
  }
}

void L1JetPlots::endJob() {

  cout << "Number of L1 SingleJet with pT>8: " << nL1Jet8 << endl;
  cout << "Number of L1 SingleJet with pT>12: " << nL1Jet12 << endl;
  cout << "Number of L1 SingleJet with pT>16: " << nL1Jet16 << endl;
  cout << "Number of L1 SingleJet with pT>36: " << nL1Jet36 << endl;
  cout << "Number of L1 SingleJet with pT>52: " << nL1Jet52 << endl;
  cout << "Number of L1 SingleJet with pT>68: " << nL1Jet68 << endl;
  cout << "Number of L1 SingleJet with pT>92: " << nL1Jet92 << endl;
  cout << "Number of L1 SingleJet with pT>128: " << nL1Jet128 << endl;

  cout << "Number of L1 TauJets with pT>20: " << nL1TauJet20 << endl;
}

void L1JetPlots::fillHist(const TString& histName, const Double_t& value, const Double_t& wt) {

  hid=m_HistNames.find(histName);
  if (hid==m_HistNames.end())
    std::cout << "%fillHist -- Could not find histogram with name: " << histName << std::endl;
  else
    hid->second->Fill(value,wt); 

}

void L1JetPlots::fill2DHist(const TString& histName, const Double_t& x,const Double_t& y,const Double_t& wt) {

  hid2D=m_HistNames2D.find(histName);
  if (hid2D==m_HistNames2D.end())
    std::cout << "%fillHist -- Could not find histogram with name: " << histName << std::endl;
  else
    hid2D->second->Fill(x,y,wt); 

}
 
bool L1JetPlots::checkPFJetID(const std::vector<reco::PFJet>::const_iterator jet){

  // loose id for PFJets

  bool jid=false;

  double chf   = jet->chargedHadronEnergyFraction();
  double nhf   = (jet->neutralHadronEnergy() + jet->HFHadronEnergy())/jet->energy();
  double phf   = jet->photonEnergyFraction();
  double elf   = jet->electronEnergyFraction();
  double chm   = jet->chargedHadronMultiplicity();
  //int nhm   = jet->neutralHadronMultiplicity();
  //int phm   = jet->photonMultiplicity();
  //int elm   = jet->electronMultiplicity();
  int npr   = jet->chargedMultiplicity() + jet->neutralMultiplicity();

  jid  = (npr>1 && phf<0.99 && nhf<0.99 && ((fabs(jet->eta())<=2.4 && elf<0.99 && chf>0 && chm>0) || fabs(jet->eta())>2.4)) ;

  return jid;

}

bool L1JetPlots::checkCaloJetID(const std::vector<reco::CaloJet>::const_iterator i_calojet){

  // loose id for CaloJets

  bool jid=false;

  double emf    = i_calojet->emEnergyFraction();
  // int n90hits   = int((*calojetID)[calojetRef].n90Hits);
  // double fHPD   = (*calojetID)[calojetRef].fHPD;
  // double fRBX   = (*calojetID)[calojetRef].fRBX;
  // int nTrkVtx   = JetExtendedAssociation::tracksAtVertexNumber(*calojetExtender,*i_calojet);
  // int nTrkCalo  = JetExtendedAssociation::tracksAtCaloNumber(*calojetExtender,*i_calojet);		   
  // bool looseID  = ((emf>0.01 || fabs(i_calojet->eta())>2.6) && (n90hits>1) && (fHPD<0.98));
  // bool tightID  = ((emf>0.01 || fabs(i_calojet->eta())>2.6) && (n90hits>1) && ((fHPD<0.98 && i_calojet->pt()<=25) || (fHPD<0.95 && i_calojet->pt()>25)));
  
  jid=emf>0.01;

  return jid;

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L1JetPlots);
