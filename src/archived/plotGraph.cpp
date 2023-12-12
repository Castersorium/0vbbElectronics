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

    std::string dataPath = "../../data/";
    std::string outputPath = "../../output/";
    std::string runName = "2023Nov/";
    std::string temperature = "29p9mK/";

    std::vector<std::string> filenames = {
        "3x3NTD.txt", 
        "3x1NTD.txt",
        "19T20.txt"
    };

    dataPath = dataPath+runName+temperature;
    outputPath = outputPath+runName+temperature;

    TFile *file = new TFile((outputPath+"outputFit.root").c_str(), "RECREATE");
    
    // Create a new canvas for the PDF
    TCanvas *cAll = new TCanvas("cAll", "All Graphs", 200, 10, 700, 500);
    cAll->Print((outputPath+"allGraphs.pdf[").c_str()); // Open the PDF file

    for (int i = 0; i < filenames.size(); i++) {
        std::ifstream dataFile(dataPath+filenames[i]);
        if (!dataFile.is_open()) {
            std::cout << "Error: File " << dataPath+filenames[i] << " could not be opened." << std::endl;
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
        graph->SetTitle((filenames[i] + "@" + temperature + ";I_Bol(A);V_Bol(V)").c_str());
        graph->SetMarkerStyle(20);
        graph->GetYaxis()->SetTitleOffset(1.30);

        // Define the function and fit it to the graph
        TF1 *f1 = new TF1(("f" + filenames[i]).c_str(), "[0]*x + [1]", 0, 3e-9); // adjust the range as needed

///FIXME: before fitting

        switch (i)
        {
        case 0: //3x3NTD
            //f1->SetRange(0,1.8e-10);//20.0mK
            //f1->SetRange(0,3.34e-10);//22.2mK
            break;
        case 1: //3x1NTD
            //f1->SetRange(0,1.55e-10);//20.0mK
            //f1->SetRange(0,2.16e-10);//22.2mK
            f1->SetRange(0,1.5e-9);//29.9mK
            break;
        case 2: //19T20
            //f1->SetRange(0,2.15e-10);//20.0mK
            //f1->SetRange(0,3.83e-10);//22.2mK
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
        c1->Print((outputPath+"allGraphs.pdf").c_str()); // Add the canvas to the PDF file
    }

    cAll->Print((outputPath+"allGraphs.pdf]").c_str()); // Close the PDF file

    file->Close();
}
