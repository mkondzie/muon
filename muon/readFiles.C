#include <RtypesCore.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>

#include <TCanvas.h>
#include <TF1.h>
#include <TH1D.h>
#include <TSpectrum.h>

#include <algorithm>
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

void readFiles() {
  if (!VERBOSE) {
    // disconnect the underlying output stream to disable logging
    std::clog.rdbuf(nullptr);
  }

  TH1D *histMu = new TH1D("histMu", "histMu", 53, 0, 20);
  for (int eventNumber = 1; eventNumber <= 24498; eventNumber++) {
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
    spectrum->Search(&histChannelA, 1, "nodraw", 0.6);

    std::vector<Double_t> peakPositionXChannelA;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelA.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelA.begin(), peakPositionXChannelA.end());
    std::clog << "____________________" << std::endl;
    for (auto &peakPositon : peakPositionXChannelA) {

      std::clog << "peakPositionXChannelA " << peakPositon << std::endl;
    }

    spectrum->Search(&histChannelB, 1, "nodraw", 0.7);
    std::vector<Double_t> peakPositionXChannelB;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelB.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelB.begin(), peakPositionXChannelB.end());
    for (auto &peakPositon : peakPositionXChannelB) {
      std::clog << "peakPositionXChannelB " << peakPositon << std::endl;
    }
    spectrum->Search(&histChannelC, 1, "nodraw", 0.7);
    std::vector<Double_t> peakPositionXChannelC;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelC.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelC.begin(), peakPositionXChannelC.end());
    for (auto &peakPositon : peakPositionXChannelC) {
      std::clog << "peakPositionXChannelC " << peakPositon << std::endl;
    }
    std::clog << "____________________" << std::endl;

    Double_t timeFrameAC = 15.5; // us
    if (!peakPositionXChannelA.empty() && !peakPositionXChannelC.empty()) {

      if ((std::abs(peakPositionXChannelA.front() -
                    peakPositionXChannelC.front()) < timeFrameAC) &&
          (std::abs(peakPositionXChannelA.front() -
                    peakPositionXChannelC.back()) < timeFrameAC) &&
          (peakPositionXChannelB.empty() || (
           (std::abs(peakPositionXChannelB.front() -
                     peakPositionXChannelC.back()) < timeFrameAC) &&
           (std::abs(peakPositionXChannelB.back() -
                     peakPositionXChannelC.back()) < timeFrameAC)))

      ) {
        Double_t timeDifferenceAC =
            peakPositionXChannelC.back() - peakPositionXChannelC.front();
        histMu->Fill(timeDifferenceAC);
      }
    }
  }
 
TF1 *fitFunc = new TF1("fitFunc", customExp, 0.5, 20, 3);
TF1 *fullFunc = new TF1("fullFunc", customExp, 0, 20, 3);

    fitFunc->SetParameters(1400, 2.2, 10); // Initial N0, T, N1 values

    histMu->Fit("fitFunc", "MERL");
       gStyle->SetOptFit(1111);
    fullFunc->SetParameters(fitFunc->GetParameter(0), fitFunc->GetParameter(1), fitFunc->GetParameter(2)); 
    double chi2 = fitFunc->GetChisquare();
    int ndf = fitFunc->GetNDF();
    double prob = fitFunc->GetProb();

    std::cout << "Fitted lifetime (T): " << fitFunc->GetParameter(1) << " +/- "
              << fitFunc->GetParError(1) << " us" << std::endl;
    std::cout << "Chi-square: " << chi2 << std::endl;
    std::cout << "Degrees of freedom: " << ndf << std::endl;
    std::cout << "Chi-square/ndf: " << chi2 / ndf << std::endl;
    std::cout << "Fit probability: " << prob << std::endl;
    std::cout << "ROOT TH1::Chisquare/ndf: " << histMu->Chisquare(fitFunc, "L") / ndf << std::endl;

  // canvas and histMu have to outlive the end of main
  TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 800);
  // canvas->SetLogy();
  histMu->SetLineColor(kBlack);
  histMu->SetMarkerColor(kBlack);
  histMu->SetFillColor(kAzure + 4);
  histMu->Draw("HE1");//HIST
    fitFunc->SetLineColor(kAzure -9);
    fitFunc->Draw("same");
       fullFunc->SetLineColor(kAzure - 9);
    fullFunc->Draw("same");
  std::cout << "Mean lifetime: " << histMu->GetMean() << " +/- "
            << histMu->GetStdDev() << " us" << std::endl;
  canvas->Print("histMu.pdf");
  delete histMu, fitFunc, fullFunc;
  delete canvas;
}
