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

void save_graphs(const std::string& runName, const std::string& temperature) {
    // Open the ROOT file
    std::string dataPath = "../../data/";
    std::string outputPath = "../../output/";
    std::string inputFileName  = "ColumnInfo.root";
    std::string outputFileName = "allCh_VAgraphs.root";

    dataPath   = dataPath   + runName + temperature;
    outputPath = outputPath + runName + temperature;

    TFile* file = TFile::Open((outputPath + inputFileName).c_str(), "READ");

    // Access the "channelTree" tree
    TTree* tree = dynamic_cast<TTree*>(file->Get("channelTree"));
    if (!tree) {
        std::cerr << "Error: Tree not found!" << std::endl;
        return;
    }

    // Create arrays to store the tree data
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
    Long64_t channelNumberRead;
    std::string nameRead;
    while (channelFile >> channelNumberRead >> nameRead) {
        channelNames[channelNumberRead] = nameRead;
    }

    // Create TGraphs for DAQCH 1 through 12
    // TODO : DAQCH is not necessarily only 12
    std::map<Long64_t, TGraph*> channelGraphs;

    for (Long64_t i = 1; i <= 12; ++i) {
        channelGraphs[i] = new TGraph();
        channelGraphs[i]->SetNameTitle(("g"+channelNames[i]).c_str(),(channelNames[i]).c_str());
        channelGraphs[i]->GetXaxis()->SetTitle("I_Bol(A)");
        channelGraphs[i]->GetYaxis()->SetTitle("V_Bol(V)");
        channelGraphs[i]->SetMarkerStyle(20);
    }

    int pointIndex = 0;
    // Fill the TGraphs with data
    for (Long64_t treeEntryNumber = 0; treeEntryNumber < tree->GetEntries(); ++treeEntryNumber) {
        tree->GetEntry(treeEntryNumber);
                
        // Iterate over all channels
        pointIndex = channelGraphs[DAQCH]->GetN();
        channelGraphs[DAQCH]->SetPoint(pointIndex, I_Bol, V_Bol);
        //std::cout << DAQCH << "\t" << pointIndex << "\t"<< I_Bol<< "\t"<< V_Bol << "\n";

    }

    TFile* outputFile = new TFile((outputPath + outputFileName).c_str(), "RECREATE");

    // Write each TGraph to the TFile
    for (const auto& entry : channelGraphs) {
        Long64_t channel = entry.first;
        channelGraphs[channel]->Write();
        //std::cout << channel << "\t" << channelGraphs[channel]->GetN() << "\t" << "\n";
    }

    // Close the TFile
    outputFile->Close();
    std::cout << "Graphs stored in" << outputPath << "\n";
}

int main() {
   
    // List of run_names and temperatures
    std::vector<std::string> run_name_list = {"2023Nov/"};//, "2023Dec/"};
    std::vector<std::string> temperature_list = {"20p0mK/", "22p2mK/", "29p9mK/","39p9mK_PIDbad/"};
    //std::vector<std::string> temperature_list = {"20p0mK/"};

    //TODO:  Idealy, the list should be input from a certain list outside the code

    //TODO: Add selected channels

    // Iterate over combinations of run_name and temperature
    for (const auto& run_name : run_name_list) {
        for (const auto& temperature : temperature_list) {
            save_graphs(run_name, temperature);
        }
    }

    // Keep the program open to view the plot
    //std::cout << "Press enter to exit..." << std::endl;
    //std::cin.ignore();

    return 0;
}
