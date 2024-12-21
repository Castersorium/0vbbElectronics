#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>

void plot(const char* file1, const char* file2, const char* file3, const char* tree, const char* branch, int nbin, double min, double max) {
   
    //int nbin =100;
    //double min=-100,max=900;   
    // 打开两个 ROOT 文件
    TFile *f1 = TFile::Open(file1);
    TFile *f2 = TFile::Open(file2);
    TFile *f3 = TFile::Open(file3);

    // 从文件中获取树
    //TTree *tree1 = (TTree*)f1->Get("tree_name"); // 将 "tree_name" 替换为实际的树名称
    TTree *tree1 = (TTree*)f1->Get(Form("%s",tree));
    TTree *tree2 = (TTree*)f2->Get(Form("%s",tree));
    TTree *tree3 = (TTree*)f3->Get(Form("%s",tree));
   
    // 创建画布
    TCanvas *canvas = new TCanvas("canvas", "Comparison of Branches", 1600, 1200);
    //canvas->Divide(2, 2); // 分为 2x2 画布

    // 绘制第一个文件的 branch1
    canvas->cd();
    TH1F *hist1 = new TH1F("hist1", "Branch from File 1", nbin, min, max); // 设置合适的 bin 数量和范围
    tree1->Draw(Form("%s >> hist1", branch));
    hist1->SetLineColor(kRed);
    //hist1->SetTitle(Form("Histogram of %s from File 1", branch));
    //canvas->SaveAs("hist1.pdf");

    // 绘制第二个文件的 branch2
    canvas->cd();
    TH1F *hist2 = new TH1F("hist2", "Branch from File 2", nbin, min, max); // 设置合适的 bin 数量和范围
    tree2->Draw(Form("%s >> hist2", branch));
    hist2->SetLineColor(kBlue);
    //hist2->SetTitle(Form("Histogram of %s from File 2", branch));
    //canvas->SaveAs("hist2.pdf");


    TH1F *hist3 = new TH1F("hist3", "Branch from File 3", nbin, min, max); // 设置合适的 bin 数量和范围
    tree3->Draw(Form("%s >> hist3", branch));
    hist3->SetLineColor(kBlack);

    // 关闭直方图的标题和统计框
    hist1->SetTitle("");
    hist2->SetTitle("");
    hist3->SetTitle("");
    gStyle->SetOptStat(0);

    // 确定最大值并设置Y轴范围
    double max1 = hist1->GetMaximum();
    double max2 = hist2->GetMaximum();
    double max3 = hist3->GetMaximum();
    double maxVal = (max1 > max2) ? max1 : max2;
    maxVal = (max1 > max3) ? max1 : max3;
    hist1->SetMaximum(2. * maxVal); // 将Y轴最大值设置为更大的直方图的1.1倍，以确保显示

    // 绘制两个分支的叠加图
    canvas->cd();
    hist1->Draw(); // 先绘制 hist1
    hist2->Draw("SAME"); // 叠加绘制 hist2
    hist3->Draw("SAME"); // 叠加绘制 hist2
    hist1->GetXaxis()->SetTitle(Form("Histogram of %s ", branch));
    hist1->GetYaxis()->SetTitle("Counts");
    // 添加图例
    TLegend *legend = new TLegend(0.7, 0.75, 0.9, 0.9);
    //legend->AddEntry(hist1, file1, "l");
    //legend->AddEntry(hist2, file2, "l");
    legend->AddEntry(hist1, "Co60", "l");
    legend->AddEntry(hist2, "Before", "l");
    legend->AddEntry(hist3, "After", "l");
    legend->Draw();
    canvas->SetLogy();
    canvas->SaveAs(Form("%s.png",branch));

    // 更新画布
    canvas->Update();
}

void draw_multi(){
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");
	plot("test/LMO.root", "test/LMO_bkg.root", "calib/hadd/After_LMO.root", "tree1", "rawamp",200,0,3E6);   
	//plot("calib/hadd/Co60_LD.root", "calib/hadd/Before_LD.root", "calib/hadd/After_LD.root", "tree1", "rawamp",200,0,3E6);   
	//plot("calib/origin/co60/light1ld.root", "calib/origin/bkg_before/light1ld.root", "calib/origin/bkg_after/light1ld.root", "tree1", "rawamp",200,0,3E6);   
}
