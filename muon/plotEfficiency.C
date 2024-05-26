#include <TCanvas.h>
#include <TGaxis.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH2D.h>
#include <TString.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <valarray>
// U (V)	AC	ACB	counts
// 1100	500	14	8
// 1200	500	212	331
// 1300	508	274	494
// 1400	511	285	639
// 1500	500	316	1519
// 1600	503	313	6493

template <typename T> void setPlottingOptions(T *plot, TString title = "; ;") {

  plot->GetYaxis()->SetLabelOffset(0.0025);
  plot->GetXaxis()->SetLabelOffset(0.004);
  plot->GetXaxis()->SetTitleOffset(1.0);
  plot->GetYaxis()->SetTitleOffset(1.1);
  plot->SetTitle(title);

  plot->GetXaxis()->SetNdivisions(409);
  plot->GetYaxis()->SetNdivisions(409);
  plot->GetXaxis()->CenterTitle(true);
  plot->GetYaxis()->CenterTitle(true);

  plot->GetXaxis()->SetTitleFont(132);
  plot->GetXaxis()->SetLabelFont(132);
  plot->GetYaxis()->SetTitleFont(132);
  plot->GetYaxis()->SetLabelFont(132);
  plot->GetXaxis()->SetTitleSize(0.05);
  plot->GetXaxis()->SetLabelSize(0.045);
  plot->GetYaxis()->SetTitleSize(0.045);
  plot->GetYaxis()->SetLabelSize(0.042);

  plot->SetMarkerSize(0.6);
  plot->SetMarkerStyle(21);

  gStyle->SetCanvasColor(kWhite);
  gStyle->SetTitleFontSize();
  gStyle->SetOptStat(0);
}

void plotEfficiency() {
  gROOT->SetBatch(kTRUE);
  typedef std::valarray<double> myArray;

  myArray voltage({1100, 1200, 1300, 1400, 1500, 1600}); // U(V)

  myArray voltageUncertainty(10 / std::sqrt(3), voltage.size());

  myArray upDownCoincidences({500, 500, 508, 511, 500, 503}); // AC

  myArray upDownCoincidencesUncertainty = std::sqrt(upDownCoincidences);

  myArray upMainDownCoincidences({14, 212, 274, 285, 316, 313}); // ACB

  myArray upMainDownCoincidencesUncertainty = std::sqrt(upMainDownCoincidences);

  myArray backgroundCounts({8, 331, 494, 639, 1519, 6493}); // per 100 s

  myArray backgroundCountsUncertainty = std::sqrt(backgroundCounts);

  /*_____________________________________________________________*/
  TCanvas *canvas = new TCanvas("canvas", "canvas", 10, 50, 1000, 600);
  canvas->Divide(2, 1);
  /*_____________________________________________________________*/
  canvas->cd(1);

  myArray efficiency = upMainDownCoincidences / upDownCoincidences;
  myArray efficiencyUncertainty = std::sqrt(
      std::pow(upMainDownCoincidencesUncertainty / upDownCoincidences, 2) +
      std::pow(upMainDownCoincidences * upDownCoincidencesUncertainty /
                   std::pow(upDownCoincidences, 2), 2));
  TGraph *graphEfficiency =
      new TGraphErrors(voltage.size(), &voltage[0], &efficiency[0],
                       &voltageUncertainty[0], &efficiencyUncertainty[0]);


  setPlottingOptions(graphEfficiency, ";#it{U} (V); #it{#eta} ");
  graphEfficiency->Draw("AP");
  /*_____________________________________________________________*/
  canvas->cd(2);
  TGraphErrors *graphBackground =
      new TGraphErrors(voltage.size(), &voltage[0], &backgroundCounts[0],
                       &voltageUncertainty[0], &backgroundCountsUncertainty[0]);
  setPlottingOptions(graphBackground, ";#it{U} (V); #it{N} / 100 s");
  graphBackground->GetYaxis()->SetMaxDigits(3); // expo notation
  graphBackground->Draw("AP");
  /*_____________________________________________________________*/
  canvas->Print("efficiencyBackground.pdf");
}