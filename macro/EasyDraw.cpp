#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <string>


void EasyDraw(){
    gStyle->SetOptFit(111);

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

    // Fit a linear function to the data
    TF1* linearFit = new TF1("linearFit", "[0]*x+[1]", 10e1, 10e4);
    linearFit->SetLineColor(kRed);
    TF1* linearFit2 = new TF1("linearFit2", "[0]*x+[1]", 10e1, 10e4);
    linearFit2->SetLineColor(kRed);

    TFile* outFile = TFile::Open((outputPath+"testJYCanvas.root").c_str(),"RECREATE");

    TCanvas *c1=new TCanvas("c1","c1",800,800);
    Int_t beforeGraph9_12 = tree2->Draw("ResistanceCh9:ResistanceCh12","ResistanceCh12<1e10 && ResistanceCh12>0 && ResistanceCh9<1e10 &&    ResistanceCh9>0","goff"); 
    TGraph *graph9_12 = new TGraph(beforeGraph9_12, tree2->GetV2(), tree2->GetV1());
    graph9_12->SetNameTitle("ResistanceCh9:ResistanceCh12","ResistanceCh9:ResistanceCh12");
    graph9_12->SetMarkerStyle(20); 
    graph9_12->Draw("AP");
    graph9_12->Fit(linearFit,"RES+","",1, 10e4);
    graph9_12->Write();
    gPad->SetLogx();
    gPad->SetLogy();
    c1->Draw();

    TCanvas *c2=new TCanvas("c2","c2",800,800);
    Int_t beforeGraph7_11 = tree2->Draw("ResistanceCh7:ResistanceCh11","ResistanceCh7<1e10 && ResistanceCh7>0 && ResistanceCh11<1e10 && ResistanceCh11>0","goff"); 
    TGraph *graph7_11 = new TGraph(beforeGraph7_11, tree2->GetV2(), tree2->GetV1()); 
    graph7_11->SetNameTitle("ResistanceCh7:ResistanceCh11","ResistanceCh7:ResistanceCh11");
    graph7_11->SetMarkerStyle(20); 
    graph7_11->Draw("AP");
    graph7_11->Fit(linearFit2,"RES+","",1, 10e4);
    graph7_11->Write();
    gPad->SetLogx();
    gPad->SetLogy();
    c2->Draw();

    // TCanvas *c3=new TCanvas("ResistanceCh7:ResistanceCh11","ResistanceCh7:ResistanceCh11",800,800);
    // Int_t beforeGraph7_11 = tree2->Draw("ResistanceCh7:ResistanceCh11","ResistanceCh7<1e10 && ResistanceCh7>0 && ResistanceCh11<1e10 && ResistanceCh11>0","goff"); 
    // TGraph *graph7_11 = new TGraph(beforeGraph7_11, tree2->GetV2(), tree2->GetV1()); 
    // graph7_11->SetNameTitle("ResistanceCh7:ResistanceCh11","ResistanceCh7:ResistanceCh11");
    // graph7_11->SetMarkerStyle(20); 
    // graph7_11->Draw("AP");
    // graph7_11->Write();
    // gPad->SetLogx();
    // gPad->SetLogy();
    // c3->Draw();

    outFile->Close();
}     

