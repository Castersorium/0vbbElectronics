#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>

int canvas_count = 0; 

#define CALIB
//#define calib_k 0.483618264*1E3
//#define calib_b 2.426496603*1E3
#define calib_k -0.913354
#define calib_b -0.76026

//void draw(const char *rootfile, const char *fillname, int nbin, double nmin, double nmax, const char *cut) {
void draw(const char *rootfile, TString fillname, int nbin, double nmin, double nmax, const char *cut) {
	
	// 打开 ROOT 文件
	TFile *file = new TFile(rootfile, "READ");
	// 获取各个树
	TTree *tree1 = (TTree*)file->Get("baseline");
	TTree *tree2 = (TTree*)file->Get("baselineslope");
	TTree *tree3 = (TTree*)file->Get("maxminusbaseline");
	TTree *tree4 = (TTree*)file->Get("risetime");
	TTree *tree5 = (TTree*)file->Get("decaytime");
	TTree *tree6 = (TTree*)file->Get("numberoftriggers");

	// 定义变量，用于存储树中的数据
	double RMS, baseline;
	double offset, slope;
	double amplitude, maxtime;
	double risetime, decaytime;
	double starttime,stoptime;
	double starttime1,starttime2;
	double stoptime1,stoptime2;
	int numberoftriggers;

	// 设置树的分支地址
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

	// 将所有树作为 tree1 的朋友树
	tree1->AddFriend(tree2);
	tree1->AddFriend(tree3);
	tree1->AddFriend(tree4,"friend4");
	tree1->AddFriend(tree5,"friend5");
	tree1->AddFriend(tree6);

	tree1->SetAlias("starttime1","friend4.starttime");
	tree1->SetAlias("stoptime1","friend4.stoptime");
	tree1->SetAlias("starttime2","friend5.starttime");
	tree1->SetAlias("stoptime2","friend5.stoptime");


	// 初始化计数器
	int entry1 = 0, entry2 = 0;
	TH1D *h1 = new TH1D("h1", "", nbin, nmin, nmax);  // 定义直方图
	TH1D *h2 = new TH1D("h2", "", nbin, nmin, nmax);  // 定义直方图	

	if (fillname == "amplitude") {
		#ifdef CALIB
			fillname = Form("amplitude - (baseline + 9)*%f", calib_k);
		#endif
	}

	if (fillname == "RMS") {
		#ifdef CALIB
			fillname = Form("1000*(RMS*%f + %f)", calib_k, calib_b);
		#endif
	}

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
	//TCanvas *c = new TCanvas("c","",1200,600);
	c->cd();
	// 绘制直方图
	h1->SetLineColor(kRed);
	h2->SetLineColor(kBlue);
	h1->SetFillColor(kRed);
	h2->SetFillColor(kBlue);
	h1->SetFillStyle(3005);
	h2->SetFillStyle(3004);
	//h2->Draw();
	//h1->Draw("same");
	h1->Draw();

	h2->SetXTitle(Form("%s",cut));
	//h2->SetTitleSize(5);  // 设置直方图总标题字体大小
	#ifdef CALIB
		//h2->SetXTitle("Energy(keV)");
		h2->SetXTitle("BaselineRMS(eV)");
	#else
		//h2->SetXTitle("ADC (ch)");
		h2->SetXTitle("ADC Voltage(V)");
	#endif
	h2->SetYTitle("Events");
	TLegend* leg = new TLegend(0.6, 0.7, .9, .9);
	//leg->AddEntry(h2, "Nocut", "lp");
	//leg->AddEntry(h1, "Cut", "lp");
	leg->AddEntry(h2, Form("Before=%d",entry2), "f");
	leg->AddEntry(h1, Form("After =%d",entry1), "f");
	//leg->Draw();
	//c->SetLogy();
	c->SaveAs(Form("rootplot/Cut%d_%s.png",canvas_count,fillname.Data()));

}


void octopus_pro(){
	//gStyle->SetOptStat(0);	
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");

	int nbin = 20*100;
	double min=-10;
	double max=10;

	// int nbin = 160*4;
	// double min=-1;
	// double max=15;
	
	//const char *file1="../rootfile/Processed_20240820T010600_000012_1.root";
	//const char *file1="../rootfile/Processed_20240824T130500_000013_1.root";
	//const char *file2="../rootfile/Processed_20241207T161500_000014_1.root";
	const char *file2="../rootfile/Processed_20241208T213600_000001_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/DLMO/Processed_20250420T1028_042001_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/DLMO/Processed_20250420T1445_042002_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/DLMO/Processed_20250420T1854_042003_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/Processed_20250421T0953_042101_1.root";
	// const char *file1="/mnt/c/Users/sky/Desktop/temp/Processed_20250420T1028_042001_1.root";
	const char *file1="/mnt/c/Users/sky/Downloads/Heater/heater3.root";

	//const char *cut = "decaytime > 0.001 && decaytime < 0.04 && risetime < 0.035 && risetime > 0.0015 &&  slope > -8000 && slope < 4000 && maxtime < 45E-3 && starttime1 < stoptime1 && starttime2 < stoptime2 && starttime1 > 0.01 && stoptime1 > 0.028 && starttime2 > 0.03 && stoptime2 > 0.03";
	//const char *cut1 = "decaytime > 0.005 && decaytime < 0.015 && risetime > 0.002 && risetime < 0.015   && starttime1 > 0.035 && starttime1 < 0.1 && stoptime1 > 0.04 && starttime2 > 0.03 && stoptime2 > 0.03";
	//const char *cut2 = "decaytime > 0.02 && decaytime < 0.03 && risetime > 0.002 && risetime < 0.015   && starttime1 > 0.03 && starttime1 < 0.1 && stoptime1 > 0.03 && stoptime1 < 0.1 && starttime2 > 0.03 && stoptime2 > 0.03";
	//const char *cut = "decaytime > 0.04 && decaytime < 0.06  && risetime > 0.005  && risetime < 0.015";
	const char *cut = "decaytime < 999 && numberoftriggers ==1 ";
	// const char *cut = "decaytime < 999 ";
	//const char *cut2 = "decaytime > 0.02 ";

	draw(file1,"amplitude",nbin,min,max,cut);
	//draw(file1,"amplitude",nbin,min,max,cut2);
	//draw(file2,"amplitude",60*4,0,4E-2,cut1);
	//draw(file1,"RMS",151*2,-1E3,15E4,cut);
	//draw(file2,"RMS",60*4,0,6E3,cut1);

}
