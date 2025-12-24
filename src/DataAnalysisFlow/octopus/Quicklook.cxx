#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>

#define OF
#define CALIB
#define calib_k 5304/2.95229
#define calib_b 0.0
int canvas_count = 0; 

void draw(const char *rootfile, TString fillname, int nbin, double nmin, double nmax, const char *cut) {
	
	// 打开 ROOT 文件
	TFile *file = new TFile(rootfile, "READ");
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

	
	// 定义变量，用于存储树中的数据
	double RMS, baseline;
	double offset, slope;
	double amplitude, maxtime;
	double risetime, decaytime;
	double starttime,stoptime;
	double starttime1,starttime2;
	double stoptime1,stoptime2;
	int numberoftriggers;
	bool issignal;
	double OF_amplitude, OF_Chi2;

	// 设置树的分支地址
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

	
	// 将所有树作为 tree1 的朋友树
	tree1->AddFriend(tree0);
	tree1->AddFriend(tree2);
	tree1->AddFriend(tree3);
	tree1->AddFriend(tree4,"friend4");
	tree1->AddFriend(tree5,"friend5");
	tree1->AddFriend(tree6);
	#ifdef OF
		tree1->AddFriend(tree7,"friend7");
		tree1->AddFriend(tree8,"friend8");
	#endif

	tree1->SetAlias("starttime1","friend4.starttime");
	tree1->SetAlias("stoptime1","friend4.stoptime");
	tree1->SetAlias("starttime2","friend5.starttime");
	tree1->SetAlias("stoptime2","friend5.stoptime");
	#ifdef OF
		tree1->SetAlias("OF_amplitude","friend7.amplitude");
		tree1->SetAlias("OF_Chi2","friend8.Chi2");
	#endif
	// 初始化计数器
	int entry1 = 0, entry2 = 0;
	TH1D *h1 = new TH1D("h1", "", nbin, nmin, nmax);  // 定义直方图
	TH1D *h2 = new TH1D("h2", "", nbin, nmin, nmax);  // 定义直方图	
	
	#ifdef CALIB
		fillname = Form("(amplitude * %f + %f )", calib_k,calib_b);
	#endif

	TCut mycut(cut);
	tree1->Draw(Form("%s>>h2",fillname.Data()));
	tree1->Draw(Form("%s>>h1",fillname.Data()),mycut.GetTitle());

	entry1 = h1->GetEntries();
	entry2 = h2->GetEntries();
	// 输出计数
	std::cout << "Beforecut: " << entry2 << std::endl;
	std::cout << "Aftercut : " << entry1 << std::endl;

    TString canvas_name = TString::Format("canvas_%d", canvas_count++);
	TCanvas *c = new TCanvas(canvas_name,"",1200,600);
	c->cd();
	// 绘制直方图
	h1->SetLineColor(kRed);
	h2->SetLineColor(kBlue);
	h1->SetStats(0);
	h2->SetStats(0);	
	h1->Draw();
	//h2->Draw("same");

	h1->SetXTitle(Form("%s",fillname.Data()));
	h1->SetYTitle("Events");
	TLegend* leg = new TLegend(0.6, 0.7, .9, .9);
	// leg->AddEntry(h2, Form("Before=%d",entry2), "f");
	// leg->AddEntry(h1, Form("After =%d",entry1), "f");
	//leg->Draw();
	c->SetLogy();
	c->SaveAs(Form("rootplot/Cut%d_%s.png",canvas_count,fillname.Data()));

}


void Quicklook(){
	//gStyle->SetOptStat(0);	
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");

	int nbin = 20*100;
	double min=-10;
	double max=10;
	
	const char *file1="/mnt/c/Users/sky/Desktop/temp/USTC/Processed_20251121T122223_000033_1.root";

	// const char *cut = "decaytime > 0.02 && decaytime < 0.1 && numberoftriggers ==1 && slope > -0.04 && slope < 0.04 && risetime > 0.003 && risetime < 0.01";
	const char *cut0 = "decaytime < 99999 ";
	const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 ";
	const char *cut2 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 && risetime > 0.0005 && risetime < 0.002 && decaytime > 0.0005 && decaytime < 0.002 ";
	const char *cut3 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 && risetime > 0.0005 && risetime < 0.007 && decaytime > 0.0005 && decaytime < 0.012 ";
	// draw(file1,"numberoftriggers",5,0,5,cut0);
	// draw(file1,"RMS",200,0,0.01,cut0);
	// draw(file1,"slope",200,-0.02,0.02,cut0);
	
	// draw(file1,"risetime",200,-0.005,0.015,cut1);
	// draw(file1,"decaytime",200,-0.005,0.015,cut1);
	// draw(file1,"starttime1",200,0.2,0.4,cut1);
	// draw(file1,"starttime2",200,0.2,0.4,cut1);
	// draw(file1,"stoptime1",200,0.2,0.4,cut1);
	// draw(file1,"stoptime2",200,0.2,0.4,cut1);

	draw(file1,"amplitude",5*100,-0.1,6000,cut3);
	// draw(file1,"OF_amplitude",10.1*100,-0.1,10,cut3);
	// draw(file1,"OF_amplitude",10*100,1.5,2.5,cut3);
	// draw(file1,"OF_Chi2",1.01*100,-0.1,10,cut3);

}
