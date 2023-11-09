#include <fstream>
#include <TGraph.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <iostream>
#include <TAxis.h>
#include <TPaveStats.h>

void plotGraph() {
    gStyle->SetOptFit(111);

    std::vector<std::string> filenames = {
        "../data/2023Nov/20p0mK/3x3NTD.txt", 
        "../data/2023Nov/20p0mK/3x1NTD.txt",
        "../data/2023Nov/20p0mK/19T20.txt"
        };
    TFile *file = new TFile("../output/2023Nov/20p0mK/outputFit.root", "RECREATE");
    
    // Create a new canvas for the PDF
    TCanvas *cAll = new TCanvas("cAll", "All Graphs", 200, 10, 700, 500);
    cAll->Print("../output/2023Nov/20p0mK/allGraphs.pdf["); // Open the PDF file

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
        graph->SetTitle((filenames[i] + "@20.0 mK;I_Bol(A);V_Bol(V)").c_str());
        graph->SetMarkerStyle(20);
        graph->GetYaxis()->SetTitleOffset(1.30);

        // Define the function and fit it to the graph
        TF1 *f1 = new TF1(("f" + filenames[i]).c_str(), "[0]*x + [1]", 0, 3e-10); // adjust the range as needed

///FIXME: before fitting

        switch (i)
        {
        case 0: //3x3NTD
            f1->SetRange(0,1.8e-10);
            break;
        case 1: //3x1NTD
            f1->SetRange(0,1.55e-10);
            break;
        case 2: //19T20
            f1->SetRange(0,2.15e-10);
            break;
        
        default:
            break;
        }

        f1->SetParNames("R_bol","Const");
        graph->Fit(f1,"RES+"); // 

        TCanvas *c1 = new TCanvas(("c" + filenames[i]).c_str(), ("TGraph Example " + std::to_string(i+1)).c_str(), 200, 10, 700, 500);

        graph->Draw("APL");
        gPad->Update();

        TPaveStats *st = (TPaveStats*)graph->FindObject("stats");
        st->SetX1NDC(0.55);//new x start position
        st->SetX2NDC(0.85);//new x end position
        st->SetY1NDC(0.15);//new y start position
        st->SetY2NDC(0.4);//new y end position

        c1->SetGrid();
        c1->Modified();

        // Save the canvas to the ROOT file
        c1->Write();
        // Save the canvas to the PDF file
        c1->Print("../output/2023Nov/20p0mK/allGraphs.pdf"); // Add the canvas to the PDF file
    }

    cAll->Print("../output/2023Nov/20p0mK/allGraphs.pdf]"); // Close the PDF file

    file->Close();
}
