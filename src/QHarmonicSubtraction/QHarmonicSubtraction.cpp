//
//  main.cpp
//  QHarmonicSubtraction
//
//  Created by Shihong Fu on 2022/11/21.
//

/*
c++ -std=c++11 -o ./qFilewithFlag/QHarmonicSubtraction.exe QHarmonicSubtraction.cpp `root-config --glibs --cflags` `diana-config --libs --cflags`
*/

/*
./QHarmonicSubtraction.exe --directory ./ --run 302525 --channel 7
*/

#include <iostream> // C++ header
#include <fstream>
#include <vector>

#include <TApplication.h> // ROOT header
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TFrame.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMath.h>
#include <TPaveStats.h>

int searchResult(std::vector<double> arr, double k);
Double_t BaselineRMS(std::vector<double> x_vec, std::vector<double> y_vec, double DTTime);
int findBestBaselineRMS(std::vector<double> x_vec, std::vector<double> y_vec, double DTTime, double &BestRMS);

int main(int argc, const char* argv[])
{
    TApplication* myApp = new TApplication("myApp", NULL, NULL);
    gStyle->SetOptStat(1111);
    
    std::string FILEPATH = "";
    int RUN = 0;
    int CHANNEL = 0;
    bool AMDEBUGGING = false;
    
    for (unsigned int i = 1; i < static_cast<unsigned int>(argc); i++)
    {
        std::string argu = argv[i];
        // index file path
        if (argu == "--directory" || argu == "-d")
        {
            i++;
            FILEPATH = argv[i];
        }
        // index run
        else if (argu == "--run" || argu == "-r")
        {
            i++;
            RUN = std::stoi(argv[i]);
        }
        // index channel
        else if (argu == "--channel" || argu == "-c")
        {
            i++;
            CHANNEL = std::stoi(argv[i]);
        }
        // set debug status
        else if (argu == "--debug")
        {
            AMDEBUGGING = true;
        }
    }
    
    int DTFlagOrder = 0;
    
    TString myFileOutput = FILEPATH;
    myFileOutput += Form("output/Baseline_from_run%d_of_ch%d.root", RUN, CHANNEL);
    TFile* outFile( TFile::Open(myFileOutput, "RECREATE") ); // output file handle
    if (!outFile || outFile->IsZombie())
    {
       std::cerr << "Error opening file" << std::endl;
       exit(-1);
    }
    
    std::string myFileInput = FILEPATH;
    myFileInput += Form("samples_run%d_ch%d.txt", RUN, CHANNEL);
    std::string myFileInputforDT = FILEPATH;
    myFileInputforDT += Form("DTFlags_run%d_ch%d.txt", RUN, CHANNEL);
    std::ifstream infile; // input file handle
    std::ifstream infileforDT;
    // read file
    infile.open(myFileInput, std::ios::in); // opening input data file
    infileforDT.open(myFileInputforDT, std::ios::in);
    // Determine whether the read is successful
    if (!infile.is_open())
    {
        std::cout << "The file [" << myFileInput << "] could not be opened correctly." << std::endl;
        return -1;
    }
    if (!infileforDT.is_open())
    {
        std::cout << "The file [" << myFileInputforDT << "] could not be opened correctly." << std::endl;
        return -2;
    }
    
    int line = 0;                      // counter for index of data point
    double x, y;                       // variable for storing data points while reading
    std::vector<double> x_vec, y_vec; // vectors for storing data points x and y
    std::vector<double> x_vec_DT, y_vec_DT;
    
    TCanvas* myC0 = new TCanvas(Form("c0_run%d_ch%d", RUN, CHANNEL), Form("ch%d", CHANNEL), 200, 10, 700, 500);
    myC0->SetGrid();
    
    while ((infile >> x >> y) && (line < 3000000 || !AMDEBUGGING)) // reading data point in while loop
    {
        // std::cout << x << "\t" << y << std::endl;
        x_vec.push_back(x);        // adding data point x to vector
        y_vec.push_back(y);        // adding data point y to vector
        line++;
    }
    
    double myHistoAmp = 50;
    double myBaselineLower = 500;
    double myBaselineUpper = 3500;
    
    // draw a frame to define the range
    TH1F* hr = myC0->DrawFrame(0, y_vec.at(DTFlagOrder) - myBaselineLower, 3000, y_vec.at(DTFlagOrder) + myBaselineUpper);
    hr->SetXTitle("time [s]");
    hr->SetYTitle("signal [mV]");
    hr->SetTitle(Form("Data from run %d ch %d", RUN, CHANNEL));
    myC0->GetFrame()->SetBorderSize(12);
    
    // create graph (declaring TGraph pointer using vector)
    TGraph* myGraph = new TGraph(static_cast<int>(x_vec.size()), x_vec.data(), y_vec.data());
    myGraph->SetLineColor(kRed);
    myGraph->SetMarkerColor(kRed);
    myGraph->SetName("SignalGraph");
    myGraph->Draw("L");
    
    line = 0;
    x = 0.0;
    y = 0.0;
    while ((infileforDT >> x >> y) && (line < 3000000 || !AMDEBUGGING)) // reading data point in while loop
    {
        // std::cout << x << "\t" << y << std::endl;
        x_vec_DT.push_back(x);        // adding data point x to vector
        y_vec_DT.push_back(y);        // adding data point y to vector
        line++;
    }
    
    // create graph for DT
    TGraph* myGraphforDT = new TGraph(static_cast<int>(x_vec_DT.size()), x_vec_DT.data(), y_vec_DT.data());
    myGraphforDT->SetMarkerStyle(23);
    myGraphforDT->SetMarkerColor(kGreen);
    myGraphforDT->SetMarkerSize(1.5);
    myGraphforDT->SetTitle("Derivative trigger");
    myGraphforDT->SetName("DTGraph");
    myGraphforDT->Draw("PSAME");
    std::cout << "I have " << x_vec_DT.size() << " flags of DT." << std::endl;
    
    // define the Legend
    TLegend * myLegend0 = new TLegend(0.6, 0.8, 0.95, 0.9);
    // draw the set of Legend
    myLegend0->AddEntry(myGraph->DrawClone(), Form("Waveform from run %d ch %d", RUN, CHANNEL), "l");
    myLegend0->AddEntry(myGraphforDT->DrawClone("same"), "Derivative trigger flags","p");
    myLegend0->SetFillStyle(0);
    myLegend0->SetBorderSize(0);
    myLegend0->SetTextSize(0.018);
    myLegend0->Draw();
    
    myC0->Modified();
    myC0->Update();
    
    myC0->Write();
    myFileOutput = FILEPATH;
    myFileOutput += Form("output/c0_run%d_ch%d.pdf", RUN, CHANNEL);
    myC0->Print(myFileOutput);     // Save the canvas in a .pdf file
    
    // Initialization for arrays.
    unsigned long CANVASNUM = 0;
    if (AMDEBUGGING)
        CANVASNUM = 6;
    else
        CANVASNUM = x_vec_DT.size();
    std::vector<TCanvas*> myC1(CANVASNUM, nullptr);
    std::vector<TCanvas*> myC2(CANVASNUM, nullptr);
    std::vector<TH1F*> h0(CANVASNUM, nullptr);
    std::vector<TH1F*> h1(CANVASNUM, nullptr);
    std::vector<TH1F*> hrPulse(CANVASNUM, nullptr);
    std::vector<TGraph*> myGraphforSignal(CANVASNUM, nullptr);
    std::vector<int> BestShiftIter(CANVASNUM, 0);
    std::vector<TGraph*> myGraphforShiftSignal(CANVASNUM, nullptr);
    std::vector<TLegend *> myLegend1(CANVASNUM, nullptr);
    std::vector<TPaveStats*> ps1(CANVASNUM, nullptr);
    
    for (DTFlagOrder = 0; DTFlagOrder < static_cast<int>(CANVASNUM); DTFlagOrder++)
    {
        myC1[DTFlagOrder] = new TCanvas(Form("c1_Sign%d", DTFlagOrder), Form("Signal_of_ch%d_Sign%d", CHANNEL, DTFlagOrder), 200 + 40 * (2 * DTFlagOrder + 1), 10 + 40 * (2 * DTFlagOrder + 1), 700, 500);
        myC1[DTFlagOrder]->SetGrid();
        myC2[DTFlagOrder] = new TCanvas(Form("c2_Sign%d", DTFlagOrder), Form("Baseline_of_ch%d_Sign%d", CHANNEL, DTFlagOrder), 200 + 40 * (2 * DTFlagOrder + 2), 10 + 40 * (2 * DTFlagOrder + 2), 700, 500);
        myC2[DTFlagOrder]->SetGrid();
        
        h0[DTFlagOrder] = new TH1F (Form("h0_Sign%d", DTFlagOrder), Form("Baseline of ch%d for Sign%d", CHANNEL, DTFlagOrder), 100, y_vec_DT.at(DTFlagOrder) - myHistoAmp, y_vec_DT.at(DTFlagOrder) + myHistoAmp);
        h1[DTFlagOrder] = new TH1F (Form("h1_Sign%d", DTFlagOrder), Form("NEWBaseline of ch%d for Sign%d", CHANNEL, DTFlagOrder), 100, y_vec_DT.at(DTFlagOrder) - myHistoAmp, y_vec_DT.at(DTFlagOrder) + myHistoAmp);
        h1[DTFlagOrder]->GetXaxis()->SetTitle("signal [mV]");
        h1[DTFlagOrder]->GetYaxis()->SetTitle("counts / 0.7 [mV^{-1}]");
        
        myC1[DTFlagOrder]->cd();
        
        // draw a frame to define the range
        hrPulse[DTFlagOrder] = myC1[DTFlagOrder]->DrawFrame(x_vec_DT.at(DTFlagOrder) - 3.5, y_vec_DT.at(DTFlagOrder) - myBaselineLower, x_vec_DT.at(DTFlagOrder) + 7.5, y_vec_DT.at(DTFlagOrder) + myBaselineUpper);
        hrPulse[DTFlagOrder]->SetXTitle("time [s]");
        hrPulse[DTFlagOrder]->SetYTitle("signal [mV]");
        hrPulse[DTFlagOrder]->SetTitle(Form("#%d pulse from run %d ch %d", DTFlagOrder, RUN, CHANNEL));
        myC1[DTFlagOrder]->GetFrame()->SetBorderSize(12);
        
        myGraphforSignal[DTFlagOrder] = new TGraph(); // declaring TGraph pointer
        int i = -1;                              // counter for index of data point
        int myTimeWindowBeginIter = searchResult(x_vec, x_vec_DT.at(DTFlagOrder) - 3); // - 3 s
        int myTimeWindowEndIter = myTimeWindowBeginIter + 1000 * 10; // 10 s
        for (int tmpIter = myTimeWindowBeginIter; tmpIter < myTimeWindowEndIter; tmpIter++)
        {
            myGraphforSignal[DTFlagOrder]->SetPoint(++i, x_vec.at(tmpIter), y_vec.at(tmpIter)); // Adding data point to graph
            if (tmpIter - myTimeWindowBeginIter < 1000 * 3)
                h0[DTFlagOrder]->Fill(y_vec.at(tmpIter));
        }
        
        double myBestRMS = 0.0;
        BestShiftIter[DTFlagOrder] = findBestBaselineRMS(x_vec, y_vec, x_vec_DT.at(DTFlagOrder), myBestRMS);
        
        myGraphforShiftSignal[DTFlagOrder] = new TGraph(); // declaring TGraph pointer
        i = -1;                              // counter for index of data point
        for (int tmpIter = myTimeWindowBeginIter; tmpIter < myTimeWindowEndIter; tmpIter++)
        {
            myGraphforShiftSignal[DTFlagOrder]->SetPoint(++i, x_vec.at(tmpIter), y_vec.at(tmpIter) - y_vec.at(BestShiftIter[DTFlagOrder] + tmpIter - myTimeWindowBeginIter) + y_vec_DT.at(0)); // Adding data point to graph
            if (tmpIter - myTimeWindowBeginIter < 1000 * 3)
                h1[DTFlagOrder]->Fill(y_vec.at(tmpIter) - y_vec.at(BestShiftIter[DTFlagOrder] + tmpIter - myTimeWindowBeginIter) + y_vec_DT.at(0));
        }
        
        myGraphforSignal[DTFlagOrder]->SetLineColor(kBlue);
        myGraphforSignal[DTFlagOrder]->SetMarkerColor(kBlue);
        myGraphforSignal[DTFlagOrder]->SetName("SignalWaveform");
        myGraphforSignal[DTFlagOrder]->Draw("L");
        myGraphforShiftSignal[DTFlagOrder]->SetLineColor(kRed);
        myGraphforShiftSignal[DTFlagOrder]->SetMarkerColor(kRed);
        myGraphforShiftSignal[DTFlagOrder]->SetName("ShiftSignalWaveform");
        myGraphforShiftSignal[DTFlagOrder]->Draw("LSAME");
        myGraphforDT->Draw("PSAME");
        
        // define the Legend
        myLegend1[DTFlagOrder] = new TLegend(0.6, 0.8, 0.95, 0.9);
        // draw the set of Legend
        myLegend1[DTFlagOrder]->AddEntry(myGraphforSignal[DTFlagOrder]->DrawClone(), Form("OLD RMS = %f [mV]", BaselineRMS(x_vec, y_vec, x_vec_DT.at(DTFlagOrder))), "l");
        myLegend1[DTFlagOrder]->AddEntry(myGraphforShiftSignal[DTFlagOrder]->DrawClone("same"), Form("Best RMS = %f [mV]", myBestRMS), "l");
        myLegend1[DTFlagOrder]->AddEntry(myGraphforDT->DrawClone("same"), "Derivative trigger flags", "p");
        myLegend1[DTFlagOrder]->SetFillStyle(0);
        myLegend1[DTFlagOrder]->SetBorderSize(0);
        myLegend1[DTFlagOrder]->SetTextSize(0.026);
        myLegend1[DTFlagOrder]->Draw();
        
        myC1[DTFlagOrder]->Modified();
        myC1[DTFlagOrder]->Update();
        
        myC1[DTFlagOrder]->Write();
        myFileOutput = FILEPATH;
        myFileOutput += Form("output/c1_run%d_ch%d_Sign%d.pdf", RUN, CHANNEL, DTFlagOrder);
        myC1[DTFlagOrder]->Print(myFileOutput);     // Save the canvas in a .pdf file
        
        myC2[DTFlagOrder]->cd();
        h0[DTFlagOrder]->SetLineColor(kBlue);
        h1[DTFlagOrder]->SetLineColor(kRed);
        h1[DTFlagOrder]->Draw();
        h0[DTFlagOrder]->Draw("SAMES");
        
        myC2[DTFlagOrder]->Update(); //this will force the generation of the "stats" box
        ps1[DTFlagOrder] = (TPaveStats*)h1[DTFlagOrder]->GetListOfFunctions()->FindObject("stats");
        ps1[DTFlagOrder]->SetY2NDC(0.6);
        ps1[DTFlagOrder]->SetTextColor(kRed);
        
        myC2[DTFlagOrder]->Modified();
        myC2[DTFlagOrder]->Update();
        
        myC2[DTFlagOrder]->Write();
        myFileOutput = FILEPATH;
        myFileOutput += Form("output/c2_run%d_ch%d_Sign%d.pdf", RUN, CHANNEL, DTFlagOrder);
        myC2[DTFlagOrder]->Print(myFileOutput);     // Save the canvas in a .pdf file
    }
    
    // std::cout << "My IterIndex is: " << searchResult(x_vec, x_vec_DT.at(DTFlagOrder)) << std::endl;
    // std::cout << "My time [s] is: " << x_vec.at(searchResult(x_vec, x_vec_DT.at(DTFlagOrder))) << std::endl;
    // std::cout << "My RMS is: " << BaselineRMS(x_vec, y_vec, x_vec_DT.at(DTFlagOrder)) << " [mV]" << std::endl;
    
    // Make my application run.
    if (AMDEBUGGING)
        myApp->Run();
    
    infile.close();
    
    outFile->Close();
    return 0;
}

int searchResult(std::vector<double> arr, double k)
{
    std::vector<double>::iterator it;
    it = std::find(arr.begin(), arr.end(), k);
    if(it != arr.end())
        return static_cast<int>(it - arr.begin());
    else
        return -1;
}

Double_t BaselineRMS(std::vector<double> x_vec, std::vector<double> y_vec, double DTTime)
{
    std::vector<double> Baseline; // vector for storing Baseline
    int myTimeWindowBeginIter = searchResult(x_vec, DTTime - 3); // - 3 s
    int myDTTimeIter = searchResult(x_vec, DTTime);
    if(myDTTimeIter != -1)
    {
        for (int tmpIter = myTimeWindowBeginIter; tmpIter < myDTTimeIter; tmpIter++)
        {
            Baseline.push_back(y_vec.at(tmpIter));        // adding data point y to vector
        }
        return TMath::RMS(Baseline.size(), Baseline.data());
    }
    else
        return -1;
}

int findBestBaselineRMS(std::vector<double> x_vec, std::vector<double> y_vec, double DTTime, double &BestRMS)
{
    std::vector<double> Baseline; // vector for storing Baseline
    int myTimeWindowBeginIter = searchResult(x_vec, DTTime - 3); // - 3 s
    int myDTTimeIter = searchResult(x_vec, DTTime);
    if(myDTTimeIter != -1)
    {
        for (int tmpIter = myTimeWindowBeginIter; tmpIter < myDTTimeIter; tmpIter++)
        {
            Baseline.push_back(y_vec.at(tmpIter));        // adding data point y to vector
        }
        std::cout << "My OLD  RMS is: " << TMath::RMS(Baseline.size(), Baseline.data()) << " [mV]" << std::endl;
        
        int myNoiseTimeWindowBeginIter = searchResult(x_vec, DTTime - 23); // - 23 s
        
        std::vector<double> NewBaseline; // vector for storing new Baseline
        for (int tmpIter = 0; tmpIter != static_cast<int>(Baseline.size()); tmpIter++)
        {
            NewBaseline.push_back(Baseline.at(tmpIter) - y_vec.at(myNoiseTimeWindowBeginIter + tmpIter));
        }
        double tmpRMS = TMath::RMS(NewBaseline.size(), NewBaseline.data());
        // std::cout << "My initial RMS is: " << tmpRMS << " [mV]" << std::endl;
        
        int BestShiftIter = 0;
        for (int ShiftIter = 0; ShiftIter < 1000 * 5; ShiftIter++) // no longer than 5 s
        {
            NewBaseline.clear();
            for (int tmpIter = 0; tmpIter != static_cast<int>(Baseline.size()); tmpIter++)
            {
                NewBaseline.push_back(Baseline.at(tmpIter) - y_vec.at(myNoiseTimeWindowBeginIter + ShiftIter + tmpIter));
            }
            // std::cout << "My ShiftIter is: " << ShiftIter << std::endl;
            // std::cout << "My current RMS is: " << TMath::RMS(NewBaseline.size(), NewBaseline.data()) << " [mV]" << std::endl;
            if (TMath::RMS(NewBaseline.size(), NewBaseline.data()) < tmpRMS)
            {
                tmpRMS = TMath::RMS(NewBaseline.size(), NewBaseline.data());
                BestShiftIter = ShiftIter;
            }
            else
                continue;
        }
        
        BestRMS = tmpRMS;
        // std::cout << "My NoiseTimeWindowBeginIter is: " << myNoiseTimeWindowBeginIter << std::endl;
        // std::cout << "My Best ShiftIter is: " << BestShiftIter << std::endl;
        std::cout << "My Best RMS is: " << BestRMS << " [mV]" << std::endl;
        std::cout << "------------------------------" << std::endl;
        
        return (myNoiseTimeWindowBeginIter + BestShiftIter);
    }
    else
        return -1;
}
