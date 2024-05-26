#include <TCanvas.h>
#include <TH1D.h>
#include <TPolyMarker.h>
#include <TSpectrum.h>
#include <TString.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

void read(int eventNumber = 101) {

  TString filename = Form("MK/event (%d).txt", eventNumber);
  std::ifstream file(filename.Data());

  TString line;
  TString format("%lf %lf %lf %lf");

  TH1D *channelAhist =
      new TH1D("channelAhist", "channelAhist", 1000, -45.015, 5.003);
  TH1D *channelBhist =
      new TH1D("channelBhist", "channelBhist", 1000, -45.015, 5.003);
  TH1D *channelChist =
      new TH1D("channelChist", "channelChist", 1000, -45.015, 5.003);
  //-45.015, 5.003);
  // time resolution: 4 ns (0.004 us)
  while (line.ReadLine(file)) {
    Double_t time, channelA, channelB, channelC;

    if (sscanf(line.Data(), format.Data(), &time, &channelA, &channelB,
               &channelC) != 4) {
      continue;
    }
    channelAhist->Fill(time, channelA);
    channelBhist->Fill(time, channelB);
    channelChist->Fill(time, channelC);

    // std::cout << time << " " << channelA << " " << channelB << " " <<
    // channelC
    //           << std::endl;
  }

  file.close();

  TSpectrum spectrum = TSpectrum();
  spectrum.Search(channelAhist, 1, "nodraw", 0.6);
  TList *functionsA = channelAhist->GetListOfFunctions();
  TPolyMarker *polyMarkerA =
      (TPolyMarker *)functionsA->FindObject("TPolyMarker");
  std::cout << "Channel A: " << spectrum.GetNPeaks() << " found at "
            << std::endl;
  std::vector<Double_t> peakPositionXChannelA;
  for (int i = 0; i < spectrum.GetNPeaks(); i++) {
    peakPositionXChannelA.push_back(spectrum.GetPositionX()[i]);
  }
  std::sort(peakPositionXChannelA.begin(), peakPositionXChannelA.end());
  for (auto &peakPositon : peakPositionXChannelA) {
    std::cout << peakPositon << std::endl;
  }

  spectrum.Search(channelBhist, 1, "nodraw", 0.7);
  TList *functionsB = channelBhist->GetListOfFunctions();
  TPolyMarker *polyMarkerB =
      (TPolyMarker *)functionsB->FindObject("TPolyMarker");
  std::cout << "Channel B: " << spectrum.GetNPeaks() << " found at "
            << std::endl;
  std::vector<Double_t> peakPositionXChannelB;
  for (int i = 0; i < spectrum.GetNPeaks(); i++) {
    peakPositionXChannelB.push_back(spectrum.GetPositionX()[i]);
  }
  std::sort(peakPositionXChannelB.begin(), peakPositionXChannelB.end());
  for (auto &peakPositon : peakPositionXChannelB) {
    std::cout << peakPositon << std::endl;
  }

  spectrum.Search(channelChist, 1, "nodraw", 0.7);
  TList *functionsC = channelChist->GetListOfFunctions();
  TPolyMarker *polyMarkerC =
      (TPolyMarker *)functionsC->FindObject("TPolyMarker");
  std::cout << "Channel C: " << spectrum.GetNPeaks() << " found at "
            << std::endl;
  std::vector<Double_t> peakPositionXChannelC;
  for (int i = 0; i < spectrum.GetNPeaks(); i++) {
    peakPositionXChannelC.push_back(spectrum.GetPositionX()[i]);
  }
  std::sort(peakPositionXChannelC.begin(), peakPositionXChannelC.end());
  for (auto &peakPositon : peakPositionXChannelC) {
    std::cout << peakPositon << std::endl;
  }

  TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 800);
  canvas->Divide(1, 3);
  canvas->cd(1);
  channelAhist->Draw("HIST");
  if (polyMarkerA)
    polyMarkerA->Draw();

  canvas->cd(3);
  channelBhist->SetLineColor(kRed);
  channelBhist->Draw("Hist");
  if (polyMarkerB) {
    polyMarkerB->Draw();
  }

  canvas->cd(2);
  channelChist->SetLineColor(kGreen);
  channelChist->Draw("Hist");
  if (polyMarkerC)
    polyMarkerC->Draw();

  canvas->Print(Form("event%d.pdf", eventNumber));
  delete channelAhist, channelBhist, channelChist;
  delete canvas;
}
