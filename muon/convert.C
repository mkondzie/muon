#include <TFile.h>
#include <TTree.h>
#include <fstream>
// #include <iostream>

void convert() {
  TString filename = "MK/event (1).txt";
  Ssiz_t openParenthesisPos = filename.First('(');
  Ssiz_t closeParenthesisPos = filename.First(')');

  TString eventNumberString = filename(
      openParenthesisPos + 1, closeParenthesisPos - openParenthesisPos - 1);
  Int_t eventNumber = eventNumberString.Atoi();

  TString rootFilename = TString::Format("rootMK/event%d.root", eventNumber);

  std::ifstream file(filename.Data());

  TString line;
  TString format("%lf %lf %lf %lf");
  ///////
  TFile *rootFile = new TFile(rootFilename, "RECREATE");
  TTree *tree = new TTree("event", "event" + eventNumberString);
  Double_t time, channelA, channelB, channelC;
  // Set branch addresses
  tree->Branch("time", &time);
  tree->Branch("channelA", &channelA);
  tree->Branch("channelB", &channelB);
  tree->Branch("channelC", &channelC);

  ///////////////
  while (line.ReadLine(file)) {

    if (sscanf(line.Data(), format.Data(), &time, &channelA, &channelB,
               &channelC) != 4) {
      continue;
    }
    tree->Fill();

    // std::cout << time << " " << channelA << " " << channelB << " " <<
    // channelC
    //           << std::endl;
  }

  file.close();
  tree->Write();
  rootFile->Close();
}
