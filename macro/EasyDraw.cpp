#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <iostream>
#include <string>


void EasyDraw(){
    // Open the ROOT file
    std::string dataPath = "../data/";
    std::string outputPath = "../output/";
    std::string runName = "2023Dec/";
    //std::string temperature = "";

    dataPath   = dataPath   + runName;
    outputPath = outputPath + runName;

    TFile* inFile = TFile::Open((dataPath + "Resistance_measurement.root").c_str(),"READ");

    // Access the "channelTree" tree
    TTree* tree2 = dynamic_cast<TTree*>(inFile->Get("MeasurementData"));
    if (!tree2) {
        std::cerr << "Error: Tree not found!" << std::endl;
        return;
    }

    TFile* outFile = TFile::Open((outputPath+"testJYCanvas.root").c_str(),"RECREATE");

    Int_t beforeGraph9_12 = tree2->Draw("ResistanceCh9:ResistanceCh12","ResistanceCh10<1e8&&ResistanceCh10>0&&ResistanceCh9<1e8&&ResistanceCh9>0","goff"); 
    TGraph *graph9_12 = new TGraph(beforeGraph9_12, tree2->GetV2(), tree2->GetV1());
    graph9_12->SetNameTitle("ResistanceCh9:ResistanceCh12","ResistanceCh9:ResistanceCh12");
    graph9_12->SetMarkerStyle(22); 
    graph9_12->Draw("AP");
    graph9_12->Write();

    Int_t beforeGraph7_11 = tree2->Draw("ResistanceCh7:ResistanceCh11","ResistanceCh7<1e10&&ResistanceCh7>0&&ResistanceCh11<1e10&&ResistanceCh11>0","goff"); 
    TGraph *graph7_11 = new TGraph(beforeGraph7_11, tree2->GetV2(), tree2->GetV1()); 
    graph7_11->SetNameTitle("ResistanceCh7:ResistanceCh11","ResistanceCh7:ResistanceCh11");
    graph7_11->SetMarkerStyle(22); 
    graph7_11->Draw("AP");
    graph7_11->Write();
    //c1->SaveAs((outputPath+"testJYCanvas.root").c_str());
    outFile->Close();
}     

