#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <iostream>

#define OF

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
void draw(const char *rootfile, TString fillname, TString fillname3, int nbin, double nmin, double nmax, const char *cut) {

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


int entry1 = tree1->GetEntries();
double x[entry1], y[entry1];

int npoints = 0; // 真正有效的点数

for (Long64_t i = 0; i < entry1; i++) {

    tree1->GetEntry(i);

    // cut
    if (issignal != 1) continue;
    if (numberoftriggers != 1) continue;
    if (!(slope > -0.004 && slope < 0.004)) continue;
    if (!(RMS < 0.002)) continue;
    if (risetime < 0.0001 || risetime > 0.01) continue;

    // 正确写法：使用 npoints，而不是 i
    x[npoints] = risetime;
    y[npoints] = amplitude;

    npoints++;  // 记录有效点数
}

TGraph *g = new TGraph(npoints, x, y);
g->Draw("AP");

}




//--------------------------------------------
// 快速浏览示例
//--------------------------------------------
void Quicklook_test() {
    gROOT->SetStyle("Modern");
    gStyle->SetOptStat(0);
    
    // const char *file1 = "/mnt/c/Users/sky/Desktop/temp/USTC/Processed_20251121T122223_000033_1.root";
        const char *file1 = "./Processed_20251121T122223_000033_1.root";
	const char *cut0 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 ";
	const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 ";

	draw(file1,"amplitude","risetime",5000,-0.1,6000,cut1);

}
