#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>

void draw(const char *rootfile, const char *h1, const char *leg1, const char *h2, const char *leg2, int Islog, int Isscale) {

	gStyle->SetOptStat(0);	
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");

    // 打开 ROOT 文件
    TFile *file = TFile::Open(rootfile, "READ");

    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open file!" << std::endl;
        return;
    }

    // 读取两个直方图
    TH1D *hist1 = (TH1D*)file->Get(h1);
    TH1D *hist2 = (TH1D*)file->Get(h2);

    if (!hist1 || !hist2) {
        std::cerr << "Failed to retrieve histograms!" << std::endl;
        return;
    }

    // 获取最大值并归一化
    double max1 = hist1->GetMaximum();
    double max2 = hist2->GetMaximum();

    if(Isscale){
    if (max1 > 0) hist1->Scale(1.0 / max1);
    if (max2 > 0) hist2->Scale(1.0 / max2);
    }

    // 创建画布并绘图
    TCanvas *c1 = new TCanvas("c1", "", 1200, 600);
    hist1->SetTitle("");
    //hist1->SetYTitle("Amplitude");
    hist1->SetTitle("");
    hist1->SetLineColor(kRed);
    hist1->SetLineWidth(2);
    hist1->Draw("HIST");

    hist2->SetTitle("");
    hist2->SetLineColor(kBlue);
    hist2->SetLineWidth(2);
    hist2->Draw("HIST SAME");

    // 添加图例
    TLegend *legend = new TLegend(0.7, 0.8, 0.9, 0.9);
    legend->AddEntry(hist1, Form("%s",leg1), "l");
    legend->AddEntry(hist2, Form("%s",leg2), "l");
    legend->Draw();

    // 保存图像或显示
    if(Islog){
        c1->SetLogy();
        c1->SetLogx();
    }
    c1->SaveAs(Form("rootplot/normalized_histograms_%s.png",h1));
    file->Close();
}

void octopus_shape() {
	gStyle->SetOptStat(0);	
	gROOT->SetStyle("Modern");
	
    
	const char *file1="../rootfile/Processed_20240820T010604_000001_1.root";
    const char *hist1="optimumfilter_heat_AP_power";
    const char *leg1="AP";
    const char *hist2="optimumfilter_heat_OF_power";
    const char *leg2="OF";

    int Is_xylog = 1;
    int Is_scale = 0;


	draw(file1,hist1,leg1,hist2,leg2,Is_xylog,Is_scale);
}
