// {
//     std::string temperature = "29p9mK/";
//         TGraph *graph1 = new TGraph();
//     graph1->SetTitle("g1;I_Bol(A);V_Bol(V)");
//     graph1->SetName("g1" );

//         for (Long64_t i = 1; i <= 12; ++i) {

//         graph1->SetPoint(i,i,0.001);
//     }
//         TFile* outputFile = new TFile("test.root", "RECREATE");
//     graph1->Write();
// }
{
        std::string temperature = "29p9mK/";

        std::vector<double> x, y;
        double tempX, tempY;
        for (Long64_t i = 1; i <= 12; ++i) {
            x.push_back(0.001);
            y.push_back(i);

        }
        TCanvas *c1 = new TCanvas();
        TGraph *graph1 = new TGraph(x.size(), &y[0], &x[0]);
        graph1->SetTitle("g1;I_Bol(A);V_Bol(V)");
        graph1->SetName("g1" );
        graph1->SetMarkerStyle(20);
        graph1->Draw("APL");
        TFile* outputFile = new TFile("test.root", "RECREATE");
        graph1->Write();
        c1->Write();
}