#include <RtypesCore.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>

#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TPolyMarker.h>
#include <TSpectrum.h>
#include <TLatex.h>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

const bool VERBOSE = false;

Double_t customExp(Double_t *x, Double_t *par) {
  Double_t t = x[0];
  Double_t N0 = par[0];
  Double_t T = par[1];
  Double_t N1 = par[2];
  return N0 * exp(-t / T) + N1;
}

void setPlottingOptions(TH1D *hist, TString title = "; ; events") {

  hist->GetXaxis()->CenterTitle(true);
  hist->GetYaxis()->CenterTitle(true);
  hist->SetTitle(title);
  hist->SetTitleSize(0.08);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetOptStat(0);

  gStyle->SetTitleFont(132);
  hist->SetTitleFont(132);
  hist->GetXaxis()->SetTitleFont(132);
  hist->GetXaxis()->SetLabelFont(132);
  hist->GetYaxis()->SetTitleFont(132);
  hist->GetYaxis()->SetLabelFont(132);

  hist->GetYaxis()->SetNdivisions(505, 4);
  hist->GetXaxis()->SetNdivisions(505, 4);
  hist->GetXaxis()->SetLabelSize(0.035);
  hist->GetYaxis()->SetLabelSize(0.035);
  hist->GetYaxis()->SetLabelOffset(0.006);
  hist->GetXaxis()->SetLabelOffset(0.006);
  hist->GetXaxis()->SetTitleOffset(1.5);
  hist->GetYaxis()->SetTitleOffset(1.4);
  hist->GetXaxis()->SetTitleSize(0.035);
  hist->GetYaxis()->SetTitleSize(0.035);

}

void readSaveFiles() {
  if (!VERBOSE) {
    // disconnect the underlying output stream to disable logging
    std::clog.rdbuf(nullptr);
  }
  //__________randomize saved events__________//
  int lowerBound = 1;
  int upperBound = 24498;
  int sizeChosenEvents = 10;

  std::vector<int> chosenEvents(sizeChosenEvents);
  std::vector<int> acceptedEvents;
  std::vector<int> rejectedEvents;
  srand(time(NULL));
  TCanvas *canvas;

  std::generate(chosenEvents.begin(), chosenEvents.end(),
                [lowerBound, upperBound]() {
                  return rand() % (upperBound - lowerBound + 1) + lowerBound;
                });

  for (const auto &chosenEvent : chosenEvents) {
    std::clog << chosenEvent << std::endl;
  }
  //_________________________________________//
  TH1D *histMu = new TH1D("histMu", "histMu", 67, 0, 20); // 53 bins, max is 20 //69
  for (int eventNumber = lowerBound; eventNumber <= upperBound; eventNumber++) {
    TString filename = Form("rootMK/event%d.root", eventNumber);
    TFile file = TFile(filename);
    TTree *tree = (TTree *)file.Get("event");

    Double_t time, channelA, channelB, channelC;

    tree->SetBranchAddress("time", &time);
    tree->SetBranchAddress("channelA", &channelA);
    tree->SetBranchAddress("channelB", &channelB);
    tree->SetBranchAddress("channelC", &channelC);
    Int_t nEntries = (Int_t)tree->GetEntries();

    TH1D histChannelA =
        TH1D("channelAhist", "channelAhist", 1000, -45.015, 5.003);
    TH1D histChannelB =
        TH1D("channelBhist", "channelBhist", 1000, -45.015, 5.003);
    TH1D histChannelC =
        TH1D("channelChist", "channelChist", 1000, -45.015, 5.003);

    for (Long_t i = 0; i < nEntries; i++) {
      tree->GetEntry(i);
      histChannelA.Fill(time, channelA);
      histChannelB.Fill(time, channelB);
      histChannelC.Fill(time, channelC);
    }

    TSpectrum spectrum_ = TSpectrum();
    auto spectrum = &spectrum_;
    spectrum->Search(&histChannelA, 1, "nodraw", 0.4);//0.6
    ///////////
    TList *functionsA = histChannelA.GetListOfFunctions();
    TPolyMarker *polyMarkerA =
        (TPolyMarker *)functionsA->FindObject("TPolyMarker");
    //////////

    std::vector<Double_t> peakPositionXChannelA;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelA.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelA.begin(), peakPositionXChannelA.end());
    std::clog << "____________________" << std::endl;
    for (auto &peakPositon : peakPositionXChannelA) {

      std::clog << "peakPositionXChannelA " << peakPositon << std::endl;
    }

    spectrum->Search(&histChannelB, 1, "nodraw", 0.4);//0.7
    //////////
    TList *functionsB = histChannelB.GetListOfFunctions();
    TPolyMarker *polyMarkerB =
        (TPolyMarker *)functionsB->FindObject("TPolyMarker");
    //////////

    std::vector<Double_t> peakPositionXChannelB;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelB.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelB.begin(), peakPositionXChannelB.end());
    for (auto &peakPositon : peakPositionXChannelB) {
      std::clog << "peakPositionXChannelB " << peakPositon << std::endl;
    }
    spectrum->Search(&histChannelC, 1, "nodraw", 0.4);//0.7
    /////////
    TList *functionsC = histChannelC.GetListOfFunctions();
    TPolyMarker *polyMarkerC =
        (TPolyMarker *)functionsC->FindObject("TPolyMarker");
    ////////////

    std::vector<Double_t> peakPositionXChannelC;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelC.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelC.begin(), peakPositionXChannelC.end());
    for (auto &peakPositon : peakPositionXChannelC) {
      std::clog << "peakPositionXChannelC " << peakPositon << std::endl;
    }
    std::clog << "____________________" << std::endl;

    bool eventFilledIntoHistMu = false;
    Double_t timeFrameAC = 15.5; // 15.5 us
    if (!peakPositionXChannelA.empty() && !peakPositionXChannelC.empty()) {

      if ((std::abs(peakPositionXChannelA.front() -
                    peakPositionXChannelC.front()) < timeFrameAC - (Double_t) 10.0) &&
          (std::abs(peakPositionXChannelA.front() -
                    peakPositionXChannelC.back()) < timeFrameAC + (Double_t) 4.5) &&
          (peakPositionXChannelB.empty() ||
           ((std::abs(peakPositionXChannelB.front() -
                      peakPositionXChannelC.back()) < timeFrameAC - (Double_t) 2.0) && 
            (std::abs(peakPositionXChannelB.back() -
                      peakPositionXChannelC.back()) < timeFrameAC - (Double_t) 2.0)))

      ) {
        Double_t timeDifferenceAC =
            peakPositionXChannelC.back() - peakPositionXChannelC.front();
        histMu->Fill(timeDifferenceAC);
        eventFilledIntoHistMu = true;
      }
    }

    if (std::find(chosenEvents.begin(), chosenEvents.end(), eventNumber) !=
        chosenEvents.end()) {
      if (eventFilledIntoHistMu) {
        acceptedEvents.push_back(eventNumber);
      } else {
        rejectedEvents.push_back(eventNumber);
      }
    }
    /////////
    if (std::find(chosenEvents.begin(), chosenEvents.end(), eventNumber) !=
        chosenEvents.end()) {
      canvas = new TCanvas("canvas", "canvas", 800, 800);
      canvas->Divide(1, 3);
      canvas->cd(1);
      setPlottingOptions(&histChannelA, "A;#it{t} (#it{#mu}s);#it{U}");
      histChannelA.Draw("HIST");
      if (polyMarkerA)
        histChannelA.Draw();

      canvas->cd(3); 
      setPlottingOptions(&histChannelB, "B;#it{t} (#it{#mu}s);#it{U}");
      histChannelB.SetLineColor(kRed);
      histChannelB.Draw("Hist");
      if (polyMarkerB) {
        histChannelB.Draw();
      }

      canvas->cd(2);
      setPlottingOptions(&histChannelC, "C;#it{t} (#it{#mu}s);#it{U}");
      histChannelC.SetLineColor(kGreen);
      histChannelC.Draw("Hist");
      if (polyMarkerC)
        polyMarkerC->Draw();

      if (std::find(acceptedEvents.begin(), acceptedEvents.end(),
                    eventNumber) != acceptedEvents.end()) {
        canvas->Print(Form("savedEvents/accepted/event%d.pdf", eventNumber));
      }
      if (std::find(rejectedEvents.begin(), rejectedEvents.end(),
                    eventNumber) != rejectedEvents.end()) {
        canvas->Print(Form("savedEvents/rejected/event%d.pdf", eventNumber));
      }
      delete canvas;
    }
    /////////////
  }

  TF1 *fitFunc = new TF1("fitFunc", customExp, 0.5, 20, 3);//0.5, 20
  TF1 *fullFunc = new TF1("fullFunc", customExp, 0, 20, 3);

  fitFunc->SetParameters(1400, 2.2, 10); // Initial N0, T, N1 values

  histMu->Fit("fitFunc", "MERL");
  // gStyle->SetOptFit(1111);
  fullFunc->SetParameters(fitFunc->GetParameter(0), fitFunc->GetParameter(1),
                          fitFunc->GetParameter(2));
  double chi2 = fitFunc->GetChisquare();
  int ndf = fitFunc->GetNDF();
  double prob = fitFunc->GetProb();

  std::cout << "Fitted lifetime (T): " << fitFunc->GetParameter(1) << " +/- "
            << fitFunc->GetParError(1) << " us" << std::endl;
  std::cout << "Chi-square: " << chi2 << std::endl;
  std::cout << "Degrees of freedom: " << ndf << std::endl;
  std::cout << "Chi-square/ndf: " << chi2 / ndf << std::endl;
  std::cout << "Fit probability: " << prob << std::endl;
  std::cout << "ROOT TH1::Chisquare/ndf: "
            << histMu->Chisquare(fitFunc, "L") / ndf << std::endl;

  // canvas and histMu have to outlive the end of main
  canvas = new TCanvas("canvas", "canvas", 800, 800);
  histMu->SetLineColor(kBlack);
  histMu->SetMarkerColor(kBlack);
  histMu->SetFillColor(kAzure + 4);
  /////////
  setPlottingOptions(histMu, ";#it{t} (#it{#mu}s); #it{N}");
  
  ///////////
  histMu->SetFillColor(kAzure + 4);
  histMu->GetXaxis()->SetRangeUser(0, 11);
  histMu->Draw("HE1"); // HIST
  gPad->SetLogy();
  fitFunc->SetLineColor(kAzure - 9);
  fitFunc->Draw("same");
  fullFunc->SetLineColor(kAzure - 9);
  fullFunc->Draw("same");
  std::cout << "Mean lifetime: " << histMu->GetMean() << " +/- "
            << histMu->GetStdDev() << " us" << std::endl;
  std::cout << "Entries: " << histMu->GetEntries() << std::endl;

  ///////////
    TLatex latex ;
    latex.SetNDC ();
    latex.SetTextSize (0.045);
    latex.SetTextColor(kBlue + 4);
    latex.DrawLatex(0.52, 0.85, "#font[132]{#it{N(t) = N_{0} e^{-t /#tau} + N_{1}}}");
    latex.DrawLatex(0.52, 0.77, Form("#font[132]{#it{#tau} = (%.3f #pm %.3f) #it{#mu}s}", fitFunc->GetParameter(1), fitFunc->GetParError(1)));
    latex.DrawLatex(0.52, 0.69, Form("#font[132]{#it{N_{0}} = %.0f #pm %.0f}", fitFunc->GetParameter(0), fitFunc->GetParError(0)));
    latex.DrawLatex(0.52, 0.61, Form("#font[132]{#it{N_{1}} = %.2f #pm %.2f}", fitFunc->GetParameter(2), fitFunc->GetParError(2)));
    latex.DrawLatex(0.52, 0.53, Form("#font[132]{#it{#chi^{2}} / ndf = %.3f}", chi2 / ndf));
  ////////////////
  canvas->Print("histMuSave.pdf");
  delete histMu, fitFunc, fullFunc;
  delete canvas;
}
