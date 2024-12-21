#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <iostream>
#include <cmath>

void draw(const char *rootfile, const char *h1, const char *leg1, double rb, double rs, double db, double ds ) {
    // 打开 ROOT 文件
    TFile *file = TFile::Open(rootfile, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open file!" << std::endl;
        return;
    }

    // 读取直方图
    TH1D *hist = (TH1D*)file->Get(h1);
    if (!hist) {
        std::cerr << "Failed to retrieve histogram!" << std::endl;
        return;
    }

    // 获取最大值以及最大值所在 bin 的索引
    double maxVal = hist->GetMaximum();
    int maxBin = hist->GetMaximumBin();
    double maxX = hist->GetBinCenter(maxBin);  // 最大值的 x 坐标

    // 计算左右两侧 90% 和 10% 最大值的点
    double left90 = hist->GetXaxis()->GetXmin();
    double right90 = hist->GetXaxis()->GetXmax();
    double left10 = hist->GetXaxis()->GetXmin();
    double right10 = hist->GetXaxis()->GetXmax();

    // 左侧 90% 和 10% 最大值的查找
    for (int bin = maxBin; bin > 0; bin--) {
        if (hist->GetBinContent(bin) <= rb * maxVal) {
            left90 = hist->GetBinCenter(bin);
            break;
        }
    }

    for (int bin = maxBin; bin <= hist->GetNbinsX(); bin++) {
        if (hist->GetBinContent(bin) <= db * maxVal) {
            right90 = hist->GetBinCenter(bin);
            break;
        }
    }

    // 右侧 10% 最大值的查找
    for (int bin = maxBin; bin > 0; bin--) {
        if (hist->GetBinContent(bin) <= rs * maxVal) {
            left10 = hist->GetBinCenter(bin);
            break;
        }
    }

    for (int bin = maxBin; bin <= hist->GetNbinsX(); bin++) {
        if (hist->GetBinContent(bin) <= ds * maxVal) {
            right10 = hist->GetBinCenter(bin);
            break;
        }
    }

    // 计算左右 x 差值
    // double risetime = 100./maxBin*(left90 - left10) ;
    // double decaytime = 100./maxBin*(right10 - right90) ;
    double risetime = (left90 - left10) ;
    double decaytime = (right10 - right90) ;

    // 创建画布并绘图
    TCanvas *c1 = new TCanvas("c1", "Histogram with 90% and 10% Markers", 1200, 600);
    hist->SetYTitle("Amplitude[V]");
    hist->SetTitle("");
    hist->SetLineColor(kRed);
    hist->SetLineWidth(2);
    hist->Draw("HIST");

    // 标注最大值、90%和10%值的位置
    TLatex *latex = new TLatex();
    latex->SetTextSize(0.03);
    latex->SetTextColor(kBlue);

    //latex->DrawText(maxX, maxVal, "Max Value");
    // latex->DrawText(left90, 0.8 * maxVal, Form("Left 90%%: %.2f", left90));
    // latex->DrawText(right90, 0.8 * maxVal, Form("Right 90%%: %.2f", right90));
    // latex->DrawText(left10, 0.6 * maxVal, Form("Left 10%%: %.2f", left10));
    // latex->DrawText(right10, 0.6 * maxVal, Form("Right 10%%: %.2f", right10));
    latex->DrawText(0.1 * (left90 + right90), 0.5 * maxVal, Form("Rise  time: %.1f ms", risetime*1000));
    latex->DrawText(0.1 * (left90 + right90), 0.4 * maxVal, Form("Decay time: %.1f ms", decaytime*1000));


    TLegend *legend = new TLegend(0.7, 0.8, 0.9, 0.9);
    legend->AddEntry(hist, Form("%s",leg1), "l");
    legend->Draw();

    // 保存图像或显示
    c1->SaveAs(Form("fang/%s.png",h1));
    file->Close();
}

void octopus_rdtime() {
	gStyle->SetOptStat(0);	
	gROOT->SetStyle("Modern");
	int max=0.9;
    int min=0.2;
    
	const char *file1="../rootfile/Processed_20240820T010604_000001_1.root";
    const char *hist1="averagepulse_heat_am241_AP";
    const char *leg1="AP of Am241";


	draw(file1,hist1,leg1,0.9,0.1,0.9,0.2);
	draw(file1,"averagepulse_heat_1mev_AP","AP of around 1MeV",0.9,0.1,0.9,0.2);

}
