#include <RtypesCore.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>

#include <TCanvas.h>
#include <TH1D.h>
#include <TF1.h>
#include <TSpectrum.h>

#include <algorithm>
#include <iostream>
#include <vector>

void readFiles() {
  TH1D *histMu = new TH1D("histMu", "histMu", 100, 0, 10);
  for (int eventNumber = 1; eventNumber <= 1001; eventNumber++) {
    TString filename = Form("rootMK/event%d.root", eventNumber);
    TFile file = TFile(filename);
    TTree *tree = (TTree *)file.Get("event");

    Double_t time, channelA, channelB, channelC;

    tree->SetBranchAddress("time", &time);
    tree->SetBranchAddress("channelA", &channelA);
    tree->SetBranchAddress("channelB", &channelB);
    tree->SetBranchAddress("channelC", &channelC);
    Int_t nEntries = (Int_t)tree->GetEntries();

    TH1D *histChannelA =
        new TH1D("channelAhist", "channelAhist", 1000, -45.015, 5.003);
    TH1D *histChannelB =
        new TH1D("channelBhist", "channelBhist", 1000, -45.015, 5.003);
    TH1D *histChannelC =
        new TH1D("channelChist", "channelChist", 1000, -45.015, 5.003);

    for (Long_t i = 0; i < nEntries; i++) {
      tree->GetEntry(i);
      histChannelA->Fill(time, channelA);
      histChannelB->Fill(time, channelB);
      histChannelC->Fill(time, channelC);
    }

    TSpectrum spectrum_ = TSpectrum();
    auto spectrum = &spectrum_;
    spectrum->Search(histChannelA, 1, "nodraw", 0.6);

    std::vector<Double_t> peakPositionXChannelA;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelA.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelA.begin(), peakPositionXChannelA.end());
    std::cout << "____________________" << std::endl;
    for (auto &peakPositon : peakPositionXChannelA) {

      std::cout << "peakPositionXChannelA " << peakPositon << std::endl;
    }

    spectrum->Search(histChannelB, 1, "nodraw", 0.6);
    std::vector<Double_t> peakPositionXChannelB;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelB.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelB.begin(), peakPositionXChannelB.end());
    for (auto &peakPositon : peakPositionXChannelB) {
      std::cout << "peakPositionXChannelB " << peakPositon << std::endl;
    }
    spectrum->Search(histChannelC, 1, "nodraw", 0.7);
    std::vector<Double_t> peakPositionXChannelC;
    for (int i = 0; i < spectrum->GetNPeaks(); i++) {
      peakPositionXChannelC.push_back(spectrum->GetPositionX()[i]);
    }
    std::sort(peakPositionXChannelC.begin(), peakPositionXChannelC.end());
    for (auto &peakPositon : peakPositionXChannelC) {
      std::cout << "peakPositionXChannelC " << peakPositon << std::endl;
    }
    std::cout << "____________________" << std::endl;
    Double_t timeFrameAC = 15; // us
    if (!peakPositionXChannelA.empty() && !peakPositionXChannelC.empty()) {
      std::cout << "differenceAC "
                << std::abs(peakPositionXChannelA.at(0) -
                            peakPositionXChannelC.at(0))
                << std::endl;
      if ((std::abs(peakPositionXChannelA.at(0) - peakPositionXChannelC.at(0)) <
          timeFrameAC) &&
          (std::abs(peakPositionXChannelA.front() - peakPositionXChannelC.back())) < timeFrameAC
          && (std::abs(peakPositionXChannelA.front() - peakPositionXChannelC.back())) < timeFrameAC){
  //       if (          peakPositionXChannelB.empty() 
  //       || (std::abs(peakPositionXChannelB.front() -
  //                     peakPositionXChannelC.back()) < 5.0 ) 
  // ) {
  //         // std::cout << "differenceBC "
  //         //     << std::abs(peakPositionXChannelB.at(0) -
  //         //                 peakPositionXChannelC.back())
  //         //     << std::endl;

          Double_t timeDifferenceAC =
              peakPositionXChannelC.back() - peakPositionXChannelC.front();
          histMu->Fill(timeDifferenceAC);
  //       }
      }
    }
  }
  // TF1 *expFunction = new TF1("expFunction","[0] + [1] * exp(-x / [2]) ",0,10, 3);
  // expFunction->SetParameter(0, 0);
  // expFunction->SetParameter(1, 0.425);
  // expFunction->SetParameter(2, 2.073);
  // histMu->Fit(expFunction, "NMER");
  // histMu->Fit("expo", "NMER");

  TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 800);
  histMu->Draw("HIST");
  std::cout << "Muon lifetime: " << histMu->GetMean() << " +/- "
            << histMu->GetStdDev() << " us" << std::endl;
}
