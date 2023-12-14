#include <TAxis.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TTree.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

void draw_graphs(const std::string& runName, const std::string& temperature) {
    // Open the ROOT file
    std::string dataPath = "../../data/";
    std::string outputPath = "../../output/";

    dataPath   = dataPath   + runName + temperature;
    outputPath = outputPath + runName + temperature;

    TFile* file = TFile::Open((outputPath + "ColumnInfo.root").c_str());

    // Access the "channelTree" tree
    TTree* tree = dynamic_cast<TTree*>(file->Get("channelTree"));
    if (!tree) {
        std::cerr << "Error: Tree not found!" << std::endl;
        return;
    }

    // Create arrays to store the data
    double I_Bol, V_Bol;
    Long64_t DAQCH;

    // Set branch addresses
    tree->SetBranchAddress("I_Bol", &I_Bol);
    tree->SetBranchAddress("V_Bol", &V_Bol);
    tree->SetBranchAddress("DAQCH", &DAQCH);

    // Read the channels_map.txt file
    std::ifstream channelFile(outputPath + "channels_map.txt");
    if (!channelFile.is_open()) {
        std::cerr << "Error: Unable to open channels_map.txt" << std::endl;
        return;
    }

    // Map to store channel names
    std::map<Long64_t, std::string> channelNames;

    // Skip the header line
    std::string header;
    std::getline(channelFile, header);

    // Read channel names
    Long64_t channel;
    std::string name;
    while (channelFile >> channel >> name) {
        channelNames[channel] = name;
    }

    // Create TGraphs for DAQCH 1, 2, and 3
    TGraph* graph_1 = new TGraph();
    TGraph* graph_2 = new TGraph();
    TGraph* graph_4 = new TGraph();

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
            pointIndex = graph_4->GetN();
            graph_4->SetPoint(pointIndex, I_Bol, V_Bol);
        }
    }

    // Create a canvas and draw the TGraphs
    TCanvas* canvas = new TCanvas("canvas", "Canvas", 800, 600);
    graph_1->SetMarkerStyle(20);  // Set marker style for DAQCH 1
    graph_2->SetMarkerStyle(21);  // Set marker style for DAQCH 2
    graph_4->SetMarkerStyle(22);  // Set marker style for DAQCH 3

    graph_1->SetMarkerColor(kRed);    // Set marker color for DAQCH 1
    graph_2->SetMarkerColor(kBlue);   // Set marker color for DAQCH 2
    graph_4->SetMarkerColor(kGreen);  // Set marker color for DAQCH 3

    graph_2->Draw("AP");
    graph_1->Draw("P same");
    graph_4->Draw("P same");

    graph_1->GetXaxis()->SetTitle("I_Bol(A)");
    graph_1->GetYaxis()->SetTitle("V_Bol(A)");

    //TODO: Add legend

    TLegend* legend = new TLegend(0.12, 0.72, 0.25, 0.88); // Adjust the position as needed
    legend->SetFillColor(0);

    // Add entries to the legend
    legend->AddEntry(graph_1, channelNames[1].c_str(), "p");
    legend->AddEntry(graph_2, channelNames[2].c_str(), "p");
    legend->AddEntry(graph_4, channelNames[4].c_str(), "p");
 
    // Draw the legend
    legend->Draw();

    canvas->Print((outputPath + "VAgraphs.root").c_str());

    file->Close();
}

int main() {
    // List of run_names and temperatures
    std::vector<std::string> run_name_list = {"2023Nov/"};//, "2023Dec/"};
    std::vector<std::string> temperature_list = {"20p0mK/", "22p2mK/", "29p9mK/","39p9mK_PIDbad/"};
    //std::vector<std::string> temperature_list = {"20p0mK/"};

    //TODO: Add selected channels

    // Iterate over combinations of run_name and temperature
    for (const auto& run_name : run_name_list) {
        for (const auto& temperature : temperature_list) {
            draw_graphs(run_name, temperature);
        }
    }

    // Keep the program open to view the plot
    std::cout << "Press enter to exit..." << std::endl;
    std::cin.ignore();

    return 0;
}
