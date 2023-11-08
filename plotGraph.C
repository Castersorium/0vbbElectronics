#include <fstream>
#include <TGraph.h>
#include <TCanvas.h>
#include <TFile.h>
#include <iostream>

void plotGraph() {
    std::vector<std::string> filenames = {"3x3NTD.txt", "3x1NTD.txt", "19T20.txt"};
    TFile *file = new TFile("outputFuAnQuXian.root", "RECREATE");

    for (int i = 0; i < filenames.size(); i++) {
        std::ifstream dataFile(filenames[i]);
        if (!dataFile.is_open()) {
            std::cout << "Error: File " << filenames[i] << " could not be opened." << std::endl;
            continue;
        }

        std::vector<double> x, y;
        double tempX, tempY;

        while (dataFile >> tempX >> tempY) {
            x.push_back(tempX);
            y.push_back(tempY);
        }

        dataFile.close();

        TGraph *graph = new TGraph(x.size(), &y[0], &x[0]);
        graph->SetTitle((filenames[i] + ";I_Bol(A);V_Bol(V)").c_str());
        graph->SetMarkerStyle(20);

        TCanvas *c1 = new TCanvas(("c" + filenames[i]).c_str(), ("TGraph Example " + std::to_string(i+1)).c_str(), 200, 10, 700, 500);
        graph->Draw("APL");

        // Save the canvas to the ROOT file
        c1->Write();
    }

    file->Close();
}
