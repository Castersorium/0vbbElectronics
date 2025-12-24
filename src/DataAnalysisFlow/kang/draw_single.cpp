#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>

// 绘制直方图函数，支持数据筛选
void plot(const char* file1, const char* tree, const char* branch, int nbin, double min, double max, const char* cut = "") {
   
    TFile *f1 = TFile::Open(file1);
    if (!f1 || f1->IsZombie()) {
        std::cout << "错误：无法打开文件 " << file1 << std::endl;
        return;
    }

    // 从文件中获取树
    TTree *tree1 = (TTree*)f1->Get(tree);
    if (!tree1) {
        std::cout << "错误：无法找到树 " << tree << std::endl;
        f1->Close();
        return;
    }

    // 获取筛选前后的entries数量
    Long64_t totalEntries = tree1->GetEntries();
    Long64_t selectedEntries = cut[0] ? tree1->GetEntries(cut) : totalEntries;
    
    std::cout << "总entries: " << totalEntries << std::endl;
    if (cut[0]) {
        std::cout << "应用筛选条件: " << cut << std::endl;
        std::cout << "筛选后entries: " << selectedEntries << std::endl;
    }

    // 创建画布
    TCanvas *canvas = new TCanvas("canvas", "Branch Histogram", 2000, 1200);

    // 创建直方图
    TH1F *hist1 = new TH1F("hist1", "Branch Distribution", nbin, min, max);
    hist1->SetLineColor(kRed);
    hist1->SetLineWidth(2);
    hist1->SetTitle("");
    
    // 关闭统计框
    gStyle->SetOptStat(0);

    // 绘制直方图（应用筛选条件）
    if (cut[0]) {
        tree1->Draw(Form("%s >> hist1", branch), cut);
    } else {
        tree1->Draw(Form("%s >> hist1", branch));
    }

    // 自定义统计信息显示
    TPaveText *stats = new TPaveText(0.7, 0.75, 0.9, 0.85, "NDC");
    stats->SetFillColor(0);
    stats->SetBorderSize(1);
    stats->AddText(Form("总Entries: %lld", totalEntries));
    if (cut[0]) {
        stats->AddText(Form("筛选后: %lld", selectedEntries));
        stats->AddText(Form("筛选条件: %s", cut));
    }
    stats->Draw();

    hist1->Draw("HIST");
    canvas->SetLogy();
    canvas->Update();

    // 保存图片（可选）
    // canvas->SaveAs(Form("%s.png", branch));
}

// 示例绘制函数
void draw_single(){
    gROOT->SetStyle("Modern");
    
    // 不应用筛选条件
    const char *cut1 = "RiseTime > 25 && RiseTime < 68  && DecayTime > 130 && DecayTime < 190 && Amp_raw > 1.41E5 && Amp_raw < 2.93E6";//kang
    const char *cut2 = "RiseTime > 25 && RiseTime < 68  && DecayTime > 130 && DecayTime < 190 && Amp_raw > 1.41E5 && Amp_raw < 2.93E6";//kang
    // const char *cut = "RiseTime > 2 && RiseTime < 10  && DecayTime > 20 && DecayTime < 150";//mouli
    //&& Amp_raw > 0 && Amp_raw < 1E5

    plot("/mnt/c/Users/jiaow/OneDrive/Desktop/TEMP/TriggerEvent_calib.root", "tree1", "Amp_raw", 500, -0.1E5, 2E5,cut1);    
    // plot("/mnt/c/Users/jiaow/OneDrive/Desktop/TEMP/TriggerEvent_bkg.root", "tree1", "Amp_raw", 500, -0.1E5, 2E5,cut2);

}
