#include <fstream>
#include <TGraph.h>
#include <TCanvas.h>
#include <iostream>

void plotGraph() {
    std::ifstream file("3x3NTD.txt");
    if (!file.is_open()) {
        std::cout << "Error: File could not be opened." << std::endl;
        return;
    }

    std::vector<double> x, y;
    double tempX, tempY;

    while (file >> tempX >> tempY) {
        x.push_back(tempX);
        y.push_back(tempY);
    }

    file.close();

    TGraph *graph = new TGraph(x.size(), &x[0], &y[0]);
    //Temp: 35.86 ~ 39.23 mK
    graph->SetTitle("35.86-39.23 mK, 3x3NTD;V_Bol(V);I_Bol(A)");
    graph->SetMarkerStyle(20);

    TCanvas *c1 = new TCanvas("c1", "TGraph Example", 200, 10, 700, 500);
    graph->Draw("APL");

    // Save the canvas to a ROOT file
    c1->SaveAs("output.root");

}