from ROOT import *
import sys,string,math,os

# import myPyRootSettings
sys.path.append(os.path.join(os.environ.get("HOME"),'rootmacros'))
from myPyRootMacros import prepPlot,SetHistColorAndMarker,PrepLegend,DrawText

###################################################################

plotEmu=False
PrintPlot=True

###################################################################

def FindMaxContents(h):

    maxC=0.
    nbins=h.GetNbinsX();    
    for i in range(nbins):
        cont=h.GetBinContent(i+1)
        if cont>maxC: maxC=cont

    return maxC

def CompGT(Triggers,TriggerSet):


    runid="240923"
    HistFile1="Histograms_CompStage1_Any_"+runid+".root"

    f1 = TFile(HistFile1)
    f1.ls()

    hname="compStage1/hL1_1"
    hLegOrg    = f1.Get(hname)
    hname="compStage1/hL1_2"
    hStage1Org = f1.Get(hname)

    nlumi=129
    hLegOrg.Scale((nlumi*23.3));
    hStage1Org.Scale((nlumi*23.3));

    hLegOrg.Sumw2()
    hStage1Org.Sumw2()

    nchan=len(Triggers)
    
    hLegacy    = TH1F( 'hLegacy','EG Trigger Rates -- Legacy', nchan, 0, float(nchan) )
    hS1Emu     = TH1F( 'hS1Emu','EG Trigger Rates -- Stage1 Emulator', nchan, 0, float(nchan) )
    hStage1    = TH1F( 'hStage1','EG Trigger Rates -- Stage1', nchan, 0, float(nchan) )
    hLegEmu    = TH1F( 'hLegEmu','EG Trigger Rates -- Legacy Emulator', nchan, 0, float(nchan) )
    hLegacy.Sumw2()
    hS1Emu.Sumw2()
    hStage1.Sumw2()
    hLegEmu.Sumw2()
    
    ibin=-1
    for trig in Triggers:
        print trig
        ibin+=1
        hLegacy.GetXaxis().SetBinLabel(ibin+1,trig)
        ## hS1Emu.GetXaxis().SetBinLabel(ibin+1,trig)
        hStage1.GetXaxis().SetBinLabel(ibin+1,trig)
        ## hLegEmu.GetXaxis().SetBinLabel(ibin+1,trig)

        theBin=hLegOrg.GetXaxis().FindBin(trig)
        rate=hLegOrg.GetBinContent(theBin)
        err=hLegOrg.GetBinError(theBin)

        print "Legacy:  ",trig,theBin,rate,err
        # print rate,err
        hLegacy.SetBinContent(ibin+1,rate)
        hLegacy.SetBinError(ibin+1,err)

        ## theBin=hS1EmuOrg.GetXaxis().FindBin(trig)
        ## rate=hS1EmuOrg.GetBinContent(theBin)
        ## err=hS1EmuOrg.GetBinError(theBin)
        ## print "S1 Emu:  ",trig,theBin,rate,err
        ## # if trigName == "L1_SingleJet16":
        ## #    rate=rate*10
        ## #    err=err*10
        ## # print rate,err
        ## hS1Emu.SetBinContent(ibin+1,rate)
        ## hS1Emu.SetBinError(ibin+1,err)

        theBin=hStage1Org.GetXaxis().FindBin(trig)
        rate=hStage1Org.GetBinContent(theBin)
        err=hStage1Org.GetBinError(theBin)
        print "Stage1:  ",trig,theBin,rate,err

        hStage1.SetBinContent(ibin+1,rate)
        hStage1.SetBinError(ibin+1,err)

        ## theBin=hLegEmuOrg.GetXaxis().FindBin(trig)
        ## rate=hLegEmuOrg.GetBinContent(theBin)
        ## err=hLegEmuOrg.GetBinError(theBin)
        ## print "Leg Emu: ",trig,theBin,rate,err,"\n"
        ## 
        ## hLegEmu.SetBinContent(ibin+1,rate)
        ## hLegEmu.SetBinError(ibin+1,err)


    hRat= hLegacy.Clone()
    hRat.SetName("Leg/Stage1")
    hRat.Divide(hLegacy,hStage1,1.,1.,"");

    hRat1= hS1Emu.Clone()
    hRat1.SetName("S1Emu/Stage1")
    hRat1.Divide(hS1Emu,hStage1,1.,1.,"");

    hRat2= hS1Emu.Clone()
    hRat2.SetName("LegEmu/Legacy")
    hRat2.Divide(hLegEmu,hLegacy,1.,1.,"");

    Hlist=TObjArray()
    Hlist.Add(hRat);
    Hlist.Add(hRat1);
    Hlist.Add(hRat2);
     
    cname="Rates"
    c1 = prepPlot("c1",cname,100,20,850,500)
    c1.SetLogy(1);    

    lmargin=0.08;    c1.SetLeftMargin(lmargin);
    rmargin=0.08;    c1.SetRightMargin(rmargin);
    bmargin=0.15;     c1.SetBottomMargin(bmargin);

    # hLegacy.Scale(0.1)
    # hStage1.Scale(0.9)
    # hLegEmu.Scale(0.9)
    SetHistColorAndMarker(hStage1,kRed,20); hStage1.SetFillColor(kRed)
    SetHistColorAndMarker(hS1Emu,kGreen+3,24)
    SetHistColorAndMarker(hLegacy,kBlue,20); hLegacy.SetFillColor(kBlue)
    SetHistColorAndMarker(hLegEmu,kMagenta,24)
    # hNum.Draw()
    hLegacy.SetMinimum(0.1)
    hStage1.SetMinimum(0.1)
    hS1Emu.SetMinimum(0.1)
    hLegEmu.SetMinimum(0.1)

    if plotEmu:
        print "JEE"
        # maxContents=FindMaxContents(hS1Emu)
        # hS1Emu.SetMaximum(maxContents*2.)
        # hS1Emu.Draw("e")
        hStage1.Draw("hist")
        hLegacy.Draw("e,same")
        hLegEmu.Draw("e,same")
        hS1Emu.Draw("e,same")
        hS1Emu.Draw("axis,same")
    else:
        #maxContents=FindMaxContents(hLegacy)
        #hStage1.SetMaximum(maxContents*2.)
        #hLegacy.Draw("hist")
        maxContents=FindMaxContents(hStage1)
        hStage1.SetMaximum(maxContents*2.)
        hStage1.Draw("hist")
        hLegacy.Draw("e,same")
        hLegacy.Draw("axis,same")


    
    x1=0.4; y1=0.2; x2=x1+0.22; y2=y1+.2
    leg=PrepLegend(x1,y1,x2,y2,0.038)
    leg.SetHeader("Run " + runid)
    leg.AddEntry( hStage1, 'Stage1 (Fed809)'  , "l")
    leg.AddEntry( hLegacy, 'Legacy (Fed813)'  , "pl")
    if plotEmu:
        leg.AddEntry( hS1Emu,  'Stage1 Emulator', "pl")
        leg.AddEntry( hLegEmu, 'Legacy Emulator'  , "pl")
    leg.Draw()
    ## DrawText(0.4,0.2,"Run " + runid + " Splashes",0.04)

    c1.Update()
    if (PrintPlot):
        psname=TriggerSet + "_Run" + str(runid)
        c1.Print(psname + ".gif")

    
    cname="Ratio"
    c2 = prepPlot("c2",cname,900,40,850,500)
    c2.SetLogy(0);    

    
    min=0.; max=1.24
    
    hRat.SetMaximum(1.24)
    hRat.SetMinimum(0.0)
    hRat.Draw()
    ## hRat2.Draw("same")

    c2.Update()
    if (PrintPlot):
        psname="ratio_" + TriggerSet + "_Run" + str(runid)
        c2.Print(psname + ".gif")

    # f2=TFile(OutFile,"RECREATE")
    # f2.cd()
    # hRat.Write()
    # f2.Close()

#===============================================================

    ans=raw_input('\npress return to continue, \"e\" to exit...')
    if len(ans)>0:
        return 1

    return 0

if __name__ == '__main__':

    gROOT.Reset()
    gROOT.SetStyle("MyStyle");    
    # gStyle.SetOptLogy(0);
    gStyle.SetPalette(1);
    gStyle.SetOptTitle(0);
    gStyle.SetOptStat(0);
    gStyle.SetPadTopMargin(0.02);
    gStyle.SetPadTickX(1);

    gStyle.SetLabelSize(0.045, "XYZ");
    gStyle.SetLabelSize(0.04, "Y");
    gStyle.SetTitleSize(0.05, "XYZ");

    RootDir="."


    NULLMAP={}
    EGMAP={
        "L1_SingleEG2":2,"L1_SingleEG35er":32,"L1_SingleIsoEG25er":33,"L1_SingleIsoEG25":34,"L1_SingleIsoEG28er":35,"L1_SingleIsoEG30er":36,
        "L1_SingleEG10":37,"L1_SingleIsoEG22er":38,"L1_SingleEG5":47,"L1_SingleEG25":48,"L1_SingleEG40":49,"L1_SingleIsoEG18":50,
        "L1_SingleIsoEG20er":51,"L1_SingleEG20":52,"L1_SingleEG30":53,"L1_SingleEG35":54,"L1_SingleEG15":57,"L1_DoubleEG_15_10":101,
        "L1_DoubleEG_22_10":102,"L1_DoubleEG_20_10_1LegIso":103
    }

    JETMAP={
        "L1_SingleJet16":3,"L1_SingleJet36":23,"L1_DoubleTauJet36er":24,"L1_DoubleTauJet44er":25,"L1_SingleJet52":17,"L1_SingleJet68":18,
        "L1_SingleJet92":19,"L1_SingleJet128":20,"L1_SingleJet176":21,"L1_SingleJet200":22,"L1_QuadJetC84":14,"L1_DoubleJetC72":15
        }

    SUMMAP={
        "L1_ETM30":66,"L1_ETM50":67,"L1_ETM70":68,"L1_ETM100":69,"L1_HTT125":70,"L1_HTT150":71,"L1_HTT175":72,"L1_HTT200":73,
        "L1_ETM40":79,"L1_HTT250":80
        }

    
    ##for L1Obj in [EGMAP,JETMAP,SUMMAP]:
    for TriggerSet in ["egamma","jet","sums"]:
        if TriggerSet == "egamma":
            L1Obj=EGMAP
        elif TriggerSet == "jet":
            L1Obj=JETMAP
        elif TriggerSet == "sums":
            L1Obj=SUMMAP
        else:
            L1Obj=NULLMAP

        Trigs= L1Obj.keys()
        print Trigs
        go=CompGT(Trigs,TriggerSet)
        if go == 1:
            break

    
