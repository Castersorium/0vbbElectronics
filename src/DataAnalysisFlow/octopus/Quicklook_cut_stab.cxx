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
        fillname2 = Form("(20 * OF_amplitude / ((baseline - %f)/%f) ) * %f ", stab_of_b, stab_of_k, calib_of_k);
        // fillname2 = Form("(OF_amplitude * %f )", calib_of_k);
    else if(name == "amplitude")
        fillname2 = Form("(3 * amplitude / ((baseline - %f)/%f)* %f + %f  )", stab_amp_b, stab_amp_k,calib_amp_k,calib_amp_b );
    else if (name == "RMS")
        // fillname2 = Form("RMS * (20 /(baseline -%f ) / %f ) * %f ", stab_of_b, stab_of_k, fabs(calib_of_k));
        fillname2 = Form("RMS * (3 /(baseline -%f ) / %f ) * %f ", stab_amp_b, stab_amp_k, fabs(calib_amp_k));
    else if (name == "baseline")
        // fillname2 = Form("RMS * (20 /(baseline -%f ) / %f ) * %f ", stab_of_b, stab_of_k, fabs(calib_of_k));
        fillname2 = Form("baseline * (3 /(baseline -%f ) / %f ) * %f + %f", stab_amp_b, stab_amp_k, calib_amp_k, calib_amp_b);
#endif

    // 创建直方图
    TH1D *h_all = new TH1D("h_all", "", nbin, nmin, nmax);
    TH1D *h_cut = new TH1D("h_cut", "", nbin, nmin, nmax);
    TH1D *h_stab = new TH1D("h_stab", "", nbin, nmin, nmax);

    TCut mycut(cut);
    tree1->Draw(Form("%s>>h_all", fillname.Data()));
    tree1->Draw(Form("%s>>h_cut", fillname.Data()), mycut.GetTitle());
    tree1->Draw(Form("%s>>h_stab", fillname2.Data()), mycut.GetTitle());

    // 输出计数
    std::cout << "Before cut: " << h_all->GetEntries() << std::endl;
    std::cout << "After cut : " << h_cut->GetEntries() << std::endl;

    // 绘制
    TString canvas_name = TString::Format("canvas_%d", canvas_count++);
    TCanvas *c = new TCanvas(canvas_name,"",1200,600);
    h_cut->SetLineColor(kRed);
    h_all->SetLineColor(kGreen);
    h_stab->SetLineColor(kBlue);

    h_cut->SetStats(0);
    h_all->SetStats(0);
    h_stab->SetStats(0);

    // h_all->Draw();
    // h_cut->Draw("same");
    h_stab->Draw("same");

    h_all->SetXTitle(Form("%s", fillname.Data()));
    h_all->SetYTitle("Events");
    h_stab->SetXTitle(Form("%s", fillname.Data()));
    h_stab->SetYTitle("Events");

    TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg->AddEntry(h_all, Form("Before = %d", int(h_all->GetEntries())), "l");
    leg->AddEntry(h_cut, Form("After = %d", int(h_cut->GetEntries())), "l");
    leg->AddEntry(h_stab, Form("Stab"), "l");
    // leg->Draw();

    // c->SetLogy();
    c->SaveAs(Form("rootplot/Cut%d_%s.png", canvas_count, name.Data()));
}

//--------------------------------------------
// 快速浏览示例
//--------------------------------------------
void Quicklook_cut_stab() {
    gROOT->SetStyle("Modern");

    const char *file1 = "./Processed_20251121T122223_000033_1.root";
    int nbin = 20*100;
    double min = -0.1;
    double max = 6000;

    // const char *cut = "issignal == 1 && numberoftriggers == 1 && slope > -0.004 && slope < 0.004 && RMS < 0.002 && risetime > 0.0005 && risetime < 0.007 && decaytime > 0.0005 && decaytime < 0.012";
	const char *cut0 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 ";
	const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 ";
	const char *cut2 = "issignal == 0 ";
	const char *cut3 = "issignal == 0 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 ";
    // const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 && risetime>0.003 && risetime<0.007 && decaytime > 0.008 && decaytime < 0.012";

    // draw(file1, "amplitude", nbin, min, max, cut);
    // draw(file1, "baseline", 200, -860, -760, cut1);

	// draw(file1,"numberoftriggers",5,0,5,cut0);
	// draw(file1,"RMS",200,0,1.2,cut0);
	// draw(file1,"slope",200,-0.02,0.02,cut0);
	
	// draw(file1,"risetime",200,-0.005,0.015,cut1);
	// draw(file1,"decaytime",200,-0.005,0.015,cut1);
	// draw(file1,"starttime1",200,0.2,0.4,cut1);
	// draw(file1,"starttime2",200,0.2,0.4,cut1);
	// draw(file1,"stoptime1",200,0.2,0.4,cut1);
	// draw(file1,"stoptime2",200,0.2,0.4,cut1);

	// draw(file1,"amplitude",5.1*1000,-0.1,5,cut);
	// draw(file1,"OF_amplitude",5000,-0.1,6000,cut1);
	draw(file1,"OF_amplitude",10*100,-5,5,cut3);
	// draw(file1,"OF_Chi2",1.01*100,-0.1,10,cut3);
    
}
