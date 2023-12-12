#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <iostream>

void draw_graphs() {
    // Open the ROOT file

    std::string dataPath = "../../data/";
    std::string outputPath = "../../output/";
    std::string runName = "2023Nov/";
    std::string temperature = "29p9mK/";

    dataPath   =   dataPath + runName + temperature;
    outputPath = outputPath + runName + temperature;

    TFile* file = TFile::Open((outputPath+"ColumnInfo.root").c_str());

    // Access the "channelTree" tree
    TTree* tree = dynamic_cast<TTree*>(file->Get("channelTree"));
    if (!tree) {
        std::cerr << "Error: Tree not found!" << std::endl;
        return;
    }

    // Create arrays to store the data
    double I_Bol, V_Bol;
    Long64_t DAQCH;  // Correct the type to Int_t

    // Set branch addresses
    tree->SetBranchAddress("I_Bol", &I_Bol);
    tree->SetBranchAddress("V_Bol", &V_Bol);
    tree->SetBranchAddress("DAQCH", &DAQCH);

    // Create TGraphs for DAQCH 1, 2, and 3
    TGraph* graph_1 = new TGraph();
    TGraph* graph_2 = new TGraph();
    TGraph* graph_3 = new TGraph();

    // Fill the TGraphs with data
    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i);
        int pointIndex;
        if (DAQCH == 1) {
            pointIndex = graph_1->GetN();
            graph_1->SetPoint(pointIndex, I_Bol, V_Bol);
        } else if (DAQCH == 2) {
            pointIndex = graph_2->GetN();
            graph_2->SetPoint(pointIndex, I_Bol, V_Bol);
        } else if (DAQCH == 4) {
            pointIndex = graph_3->GetN();
            graph_3->SetPoint(pointIndex, I_Bol, V_Bol);
        }
    }

    // Create a canvas and draw the TGraphs
    TCanvas* canvas = new TCanvas("canvas", "Canvas", 800, 600);
    graph_1->SetMarkerStyle(20);  // Set marker style for DAQCH 1
    graph_2->SetMarkerStyle(21);  // Set marker style for DAQCH 2
    graph_3->SetMarkerStyle(22);  // Set marker style for DAQCH 3

    graph_1->SetMarkerColor(kRed);    // Set marker color for DAQCH 1
    graph_2->SetMarkerColor(kBlue);   // Set marker color for DAQCH 2
    graph_3->SetMarkerColor(kGreen);  // Set marker color for DAQCH 3

    graph_1->Draw("AP");
    graph_2->Draw("P same");
    graph_3->Draw("P same");

    // Optionally save the plot as an image file
    //canvas->SaveAs("./graphs.root");
    canvas->Print((outputPath+"VAgraphs.root").c_str());
    // Close the file
    file->Close();
}

int main() {
    // Run the drawing function
    draw_graphs();

    // Keep the program open to view the plot
    std::cout << "Press enter to exit..." << std::endl;
    std::cin.ignore();

    return 0;
}
