#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <iostream>

#define OF
#define STAB
#define CALIB

//for amp
#define stab_amp_k -2.0292
#define stab_amp_b 3.9504
#define calib_amp_k 1639.56
#define calib_amp_b 3.5056
//for OF_amp
#define stab_of_k -0.3058
#define stab_of_b 4.0376
// #define calib_of_b -12.8976
// #define calib_of_k 251.244
#define calib_of_b -24.0872
#define calib_of_k 255.6602

int canvas_count = 0;

//--------------------------------------------
// 主绘图函数
//--------------------------------------------
void draw(const char *rootfile, TString fillname, int nbin, double nmin, double nmax, const char *cut) {

    TString name = fillname;

    // 打开 ROOT 文件
    TFile *file = new TFile(rootfile, "READ");
    if(!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << rootfile << std::endl;
        return;
    }

    // 获取各个树
    TTree *tree0 = (TTree*)file->Get("module");
    TTree *tree1 = (TTree*)file->Get("baseline");
    TTree *tree2 = (TTree*)file->Get("baselineslope");
    TTree *tree3 = (TTree*)file->Get("maxminusbaseline");
    TTree *tree4 = (TTree*)file->Get("risetime");
    TTree *tree5 = (TTree*)file->Get("decaytime");
    TTree *tree6 = (TTree*)file->Get("numberoftriggers");
#ifdef OF
    TTree *tree7 = (TTree*)file->Get("optimumfilter");
    TTree *tree8 = (TTree*)file->Get("chi2");
#endif

    // 定义变量
    double RMS, baseline, offset, slope, amplitude, maxtime, risetime, decaytime;
    double starttime1, stoptime1, starttime2, stoptime2;
    int numberoftriggers;
    bool issignal;
    double OF_amplitude, OF_Chi2;
    TString fillname2;

    // 设置分支地址
    tree0->SetBranchAddress("issignal", &issignal);
    tree1->SetBranchAddress("RMS", &RMS);
    tree1->SetBranchAddress("baseline", &baseline);
    tree2->SetBranchAddress("offset", &offset);
    tree2->SetBranchAddress("slope", &slope);
    tree3->SetBranchAddress("amplitude", &amplitude);
    tree3->SetBranchAddress("maxtime", &maxtime);
    tree4->SetBranchAddress("risetime", &risetime);
    tree4->SetBranchAddress("starttime", &starttime1);
    tree4->SetBranchAddress("stoptime", &stoptime1);
    tree5->SetBranchAddress("decaytime", &decaytime);
    tree5->SetBranchAddress("starttime", &starttime2);
    tree5->SetBranchAddress("stoptime", &stoptime2);
    tree6->SetBranchAddress("numberoftriggers", &numberoftriggers);
#ifdef OF
    tree7->SetBranchAddress("amplitude", &OF_amplitude);
    tree8->SetBranchAddress("Chi2", &OF_Chi2);
#endif

    // 设置朋友树
    tree1->AddFriend(tree0);
    tree1->AddFriend(tree2);
    tree1->AddFriend(tree3);
    tree1->AddFriend(tree4, "friend4");
    tree1->AddFriend(tree5, "friend5");
    tree1->AddFriend(tree6);
#ifdef OF
    tree1->AddFriend(tree7, "friend7");
    tree1->AddFriend(tree8, "friend8");
#endif

    // 设置别名
    tree1->SetAlias("starttime1","friend4.starttime");
    tree1->SetAlias("stoptime1","friend4.stoptime");
    tree1->SetAlias("starttime2","friend5.starttime");
    tree1->SetAlias("stoptime2","friend5.stoptime");
#ifdef OF
    tree1->SetAlias("OF_amplitude","friend7.amplitude");
    tree1->SetAlias("OF_Chi2","friend8.Chi2");
#endif

#ifdef STAB 
    if(name == "OF_amplitude")
            fillname2 = Form("(20 * OF_amplitude / ((baseline - %f)/%f) )", stab_of_b, stab_of_k);
    else if(name == "amplitude")
            fillname2 = Form("(3 * amplitude / ((baseline - %f)/%f) )", stab_amp_b, stab_amp_k);
#endif

#ifdef CALIB
    if(name == "OF_amplitude")
        fillname2 = Form("(20 * OF_amplitude / ((baseline - %f)/%f) ) * %f + %f ", stab_of_b, stab_of_k, calib_of_k, calib_of_b);
    else if(name == "amplitude")
        fillname2 = Form("(3 * amplitude / ((baseline - %f)/%f)* %f + %f  )", stab_amp_b, stab_amp_k,calib_amp_k,calib_amp_b );
    else if (name == "RMS")
        // fillname2 = Form("RMS * (20 /(baseline -%f ) / %f ) * %f ", stab_of_b, stab_of_k, fabs(calib_of_k));
        fillname2 = Form("RMS * (3 /(baseline -%f ) / %f ) * %f ", stab_amp_b, stab_amp_k, fabs(calib_amp_k));
#endif

    TH1D *h1 = new TH1D("h1", "", nbin, nmin, nmax);
    TH1D *h2 = new TH1D("h2", "", nbin, nmin, nmax);

    TCut mycut(cut);
    tree1->Draw(Form("%s>>h1", fillname2.Data()), mycut.GetTitle());


        // ==== 输出 ROOT 文件 ====
    TFile *fout = new TFile("./calibration_FDU.root", "RECREATE");
    // 输出 TTree
    TTree *outtree = new TTree("tree", "tree");
    double out_value,value;
    outtree->Branch(fillname.Data(), &out_value, (fillname + "/D").Data());

    // ==== 遍历 tree 并手动应用 cut ====
    Long64_t nentries = tree1->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree1->GetEntry(i);
        // === 手动 cut ===
        if (!(issignal == 1)) continue;
        // if (!(numberoftriggers == 1)) continue;
        // if (!(slope > -0.004 && slope < 0.004)) continue;
        // if (!(RMS < 0.002)) continue;
        // 保存值
    
        #ifdef STAB 
            if(name == "OF_amplitude")
                    value = (20 * OF_amplitude / ((baseline - stab_of_b)/stab_of_k));
            else if(name == "amplitude")
                    value = (3 * amplitude / ((baseline - stab_amp_b)/stab_amp_k));
        #endif

        #ifdef CALIB
            if(name == "OF_amplitude")
                value = (20 * OF_amplitude / ((baseline - stab_of_b)/stab_of_k) ) * calib_of_k + calib_of_b;
            else if(name == "amplitude")
                value = (3 * amplitude / ((baseline - stab_amp_b)/stab_amp_k)* calib_amp_k + calib_amp_b  );
            else if (name == "RMS")
                // fillname2 = Form("RMS * (20 /(baseline -%f ) / %f ) * %f ", stab_of_b, stab_of_k, fabs(calib_of_k));
                value = RMS * (3 /(baseline -stab_amp_b ) / stab_amp_k ) * calib_amp_k ;
        #endif


        out_value = value;
        outtree->Fill();
        h2->Fill(value);
    }


    TString canvas_name = TString::Format("canvas_%d", canvas_count++);
    TCanvas *c = new TCanvas(canvas_name,"",1200,600);
    c->cd();
    h1->SetLineColor(kRed);
    h2->SetLineColor(kGreen);
    h1->SetStats(0);
    h2->SetStats(0);
    h1->Draw();
    // h2->Draw("same");
    c->Update();

    // ==== 写入输出文件 ====
    outtree->Write();
    fout->Close();

}


void plot_histograms(const char *rootfile1, const char *rootfile2) {

    
    TFile *file1 = new TFile(rootfile1, "READ");
    TFile *file2 = new TFile(rootfile2, "READ");

    // 3. 读取TTree
    TTree* tree1 = (TTree*)file1->Get("calibTree");
    TTree* tree2 = (TTree*)file2->Get("tree");
    
    
    std::cout << "成功读取tree1和tree2" << std::endl;
    // int entry1 = tree1->GetEntries();
    // int entry2 = tree2->GetEntries();
    std::cout << "tree1 条目数: " << tree1->GetEntries() << std::endl;
    std::cout << "tree2 条目数: " << tree2->GetEntries() << std::endl;
    
    // 4. 创建直方图
    // 定义直方图范围和bin数量
    int nbins = 5000;
    double xmin = 0.0;  // 根据你的数据范围调整
    double xmax = 10.0;  // 根据你的数据范围调整
    
    // 创建直方图对象
    TH1F* h1 = new TH1F("h1", "", nbins, xmin, xmax);
    TH1F* h2 = new TH1F("h2", "", nbins, xmin, xmax);
    
    // 设置直方图属性
    h1->SetLineColor(kBlue);
    h1->SetLineWidth(2);
    // h1->SetFillColorAlpha(kBlue, 0.3);
    
    h2->SetLineColor(kRed);
    h2->SetLineWidth(1);
    // h2->SetLineStyle(2);
    // h2->SetFillColorAlpha(kRed, 0.3);
    
    // 方法1: 使用Draw函数填充
    // tree1->Draw("E >> h1", "", "goff");  // "goff" 表示不显示图形
    // tree2->Draw("OF_amplitude/1000. >> h2", "", "goff");   
    tree1->Draw("E >> h1","E>0");  // "goff" 表示不显示图形
    tree2->Draw("OF_amplitude/1000. >> h2","OF_amplitude>0");
    
    int entry1 = h1->GetEntries();
    int entry2 = h2->GetEntries();


    // 6. 创建画布并绘制直方图
    TCanvas* canvas = new TCanvas("canvas", "Comparison between USTC and FDU", 800, 600);
    
    // 设置画布样式
    canvas->SetGrid();
    
    // 绘制第一个直方图
    h2->SetStats(0);  // 关闭统计信息框
    h2->SetXTitle("Energy (MeV)");
    h2->SetYTitle("Counts");
    h2->Draw();
    
    // 绘制第二个直方图在同一个画布上
    h1->Draw("same");
    
    // 7. 添加图例
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(h1, Form("USTC=%d",entry1), "l");
    legend->AddEntry(h2, Form("FDU=%d",entry2), "l");
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->Draw();
    
    // 8. 添加标题
    // h1->SetTitle("Tree1:E1 和 Tree2:E2 分布比较");
    // 9. 更新画布
    canvas->Update();
    // 10. 保存图像
    canvas->SaveAs("rootplot/comparison.png");
    
    // 11. 显示统计信息
    std::cout << "\n直方图统计信息:" << std::endl;
    std::cout << "h1 (E1): 均值 = " << h1->GetMean() 
              << ", RMS = " << h1->GetRMS() 
              << ", 积分 = " << h1->Integral() << std::endl;
    std::cout << "h2 (E2): 均值 = " << h2->GetMean() 
              << ", RMS = " << h2->GetRMS() 
              << ", 积分 = " << h2->Integral() << std::endl;

}


//--------------------------------------------
// 快速浏览示例
//--------------------------------------------
void Quicklook_save() {
    gROOT->SetStyle("Modern");
    gStyle->SetOptStat(0);
    
    const char *file1 = "./Processed_20251121T122223_000033_1.root";
	const char *cut0 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 ";
	const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 ";
	const char *cut2 = "issignal == 1 && numberoftriggers == 1 ";

	draw(file1,"OF_amplitude",5000,-0.1,6000,cut2);
 
    const char *file2 = "./calibration.root";  // 请替换为实际路径
    const char *file3 = "./calibration_FDU.root";    // 请替换为实际路径
    plot_histograms(file2,file3);

}
