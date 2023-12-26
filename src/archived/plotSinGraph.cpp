#include <fstream>
#include <TGraph.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TStyle.h>
#include <iostream>
#include <TAxis.h>
#include <TPaveStats.h>

void plotSinGraph() {
    gStyle->SetOptFit(111);

    src/plotGraph.C
    std::vector<std::string> filenames = {
        "../../data/2023Nov/39p9mK_PIDbad/3x3NTD.txt", 
        "../../data/2023Nov/39p9mK_PIDbad/3x1NTD.txt",
        "../../data/2023Nov/39p9mK_PIDbad/19T20.txt"
    };
    TFile *file = new TFile("../../output/2023Nov/39p9mK_PIDbad/outputFit.root", "RECREATE");

    // Create a new canvas for the PDF
    TCanvas *cAll = new TCanvas("cAll", "All Graphs", 200, 10, 700, 500);
    cAll->Print("../../output/2023Nov/39p9mK_PIDbad/allGraphs.pdf["); // Open the PDF file

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
        graph->SetTitle((filenames[i] + "@38.01-41.78 mK;I_Bol(A);V_Bol(V)").c_str());
        graph->SetMarkerStyle(20);
        graph->GetYaxis()->SetTitleOffset(1.30);

        // Define the function and fit it to the graph
        TF1 *f1 = new TF1(("f" + filenames[i]).c_str(), "[0]*x + [1]*x*sin([2]*x+[3]) + [4]", 0, 2e-9); // adjust the range as needed

///FIXME: before fitting

        f1->SetParLimits(0,1e5,1e7);//R_bol
        f1->SetParLimits(1,1e4,1e6);//Amplitude
        f1->SetParLimits(2,1e10,1e11);//omega
        f1->SetParLimits(3,0,7);//initial phi

        switch (i)
        {
        case 0: //3x3NTD
            f1->SetParameters(1.4e6,3e5,2.219e10,1.38,0);//3x3NTD
            break;
        case 1: //3x1NTD
            f1->SetParLimits(4,-1e-3,1e-3);//Const
            f1->SetParameters(1.4e6,3e5,2.219e10,1.38,0);//3x1NTD
            break;
        case 2: //19T20
            f1->SetParLimits(4,-1e-3,1e-3);//Const
            f1->SetParameters(4.3e5,1e5,2.2e10,1.83,0);//19T20
            break;
        
        default:
            break;
        }

        f1->SetParNames("R_bol","Amp","omega","phi","Const");
        graph->Fit(f1,"RES+"); // 

        // Print the sum of the parameters "R_bol" and "Amp"
        std::cout << "+-------------------------------------------------------------------------------------------------+" << std::endl;
        std::cout << std::scientific;
        std::cout << "|  Sum of R_bol and Amp        for " << filenames[i] << ": " << f1->GetParameter("R_bol") + f1->GetParameter("Amp") << std::endl;
        std::cout << "|  R_bol                       for " << filenames[i] << ": " << f1->GetParameter("R_bol") << std::endl;
        std::cout << "|  Difference of R_bol and Amp for " << filenames[i] << ": " << f1->GetParameter("R_bol") - f1->GetParameter("Amp") << std::endl;
        std::cout << std::defaultfloat;
        std::cout << "+-------------------------------------------------------------------------------------------------+" << std::endl;

        TCanvas *c1 = new TCanvas(("c" + filenames[i]).c_str(), ("TGraph Example " + std::to_string(i+1)).c_str(), 200, 10, 700, 500);

        graph->Draw("APL");
        gPad->Update();

        TPaveStats *st = (TPaveStats*)graph->FindObject("stats");
        st->SetX1NDC(0.15);//new x start position
        st->SetX2NDC(0.45);//new x end position
        st->SetY1NDC(0.5);//new y start position
        st->SetY2NDC(0.85);//new y end position

        c1->SetGrid();
        c1->Modified();

        // Save the canvas to the ROOT file
        c1->Write();
        // Save the canvas to the PDF file
        c1->Print("../../output/2023Nov/39p9mK_PIDbad/allGraphs.pdf"); // Add the canvas to the PDF file
    }

    cAll->Print("../../output/2023Nov/39p9mK_PIDbad/allGraphs.pdf]"); // Close the PDF file

    file->Close();
}
