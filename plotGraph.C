#include <fstream>
#include <TGraph.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <iostream>

void plotGraph() {
    gStyle->SetOptFit(1011);

    std::vector<std::string> filenames = {
        //"3x3NTD.txt", 
        "3x1NTD.txt"
        //, "19T20.txt"
        };
    TFile *file = new TFile("testFit.root", "RECREATE");

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

        // Define the function and fit it to the graph
        TF1 *f1 = new TF1("f1", "[0]*x + [1]*x*sin([2]*x+[3]) + [4]", 0, 2e-9); // adjust the range as needed
        f1->SetParLimits(0,1e5,1e7);//R_bol
        f1->SetParLimits(1,1e4,1e6);//Amplitude
        f1->SetParLimits(2,1e8,1e11);//omega
        f1->SetParLimits(3,0,7);//initial phi
        //f1->SetParameters(2e6,3e5,2e10,1,0);
        f1->SetParameters(1.4e6,3e5,2.219e10,1.38,0);
        f1->SetParNames("R_bol","Amp","omega","phi","Const");
        graph->Fit(f1,"RES+"); // 
        graph->Fit(f1,"RES+"); // 
                graph->Fit(f1,"RES+"); // 
        graph->Fit(f1,"RES+"); // 


        TCanvas *c1 = new TCanvas(("c" + filenames[i]).c_str(), ("TGraph Example " + std::to_string(i+1)).c_str(), 200, 10, 700, 500);
        graph->Draw("APL");

        // Save the canvas to the ROOT file
        c1->Write();
    }

    file->Close();
}
