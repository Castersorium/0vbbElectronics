#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>

int canvas_count;
//#define CALIB
//#define calib_k 0.77049725
//#define calib_b 3.8658775

void draw(const char *rootfile, TString fillname1, TString fillname2,int nbin, double nmin, double nmax, int nbin2, double nmin2, double nmax2, const char *cut) {
	
	// 打开 ROOT 文件
	TFile *file = new TFile(rootfile, "READ");
	// 获取各个树
	TTree *tree1 = (TTree*)file->Get("baseline");
	TTree *tree2 = (TTree*)file->Get("baselineslope");
	TTree *tree3 = (TTree*)file->Get("maxminusbaseline");
	TTree *tree4 = (TTree*)file->Get("risetime");
	TTree *tree5 = (TTree*)file->Get("decaytime");

	// 定义变量，用于存储树中的数据
	double RMS, baseline;
	double offset, slope;
	double amplitude, maxtime;
	double risetime, decaytime;
	double starttime,stoptime;
	double starttime1,starttime2;
	double stoptime1,stoptime2;

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

	// 将所有树作为 tree1 的朋友树
	tree1->AddFriend(tree2);
	tree1->AddFriend(tree3);
	tree1->AddFriend(tree4,"friend4");
	tree1->AddFriend(tree5,"friend5");

	tree1->SetAlias("starttime1","friend4.starttime");
	tree1->SetAlias("stoptime1","friend4.stoptime");
	tree1->SetAlias("starttime2","friend5.starttime");
	tree1->SetAlias("stoptime2","friend5.stoptime");

	TH2D *hist = new TH2D("hist", Form("%s vs %s",fillname1.Data(),fillname2.Data()), nbin, nmin, nmax, nbin2, nmin2, nmax2);
	// 设置 X 和 Y 轴的标题
	hist->SetXTitle(Form("%s",fillname1.Data()));
	hist->SetYTitle(Form("%s",fillname2.Data()));

	// if (fillname == "amplitude") {
	// 	#ifdef CALIB
	// 		fillname = Form("amplitude*%f + %f", calib_k, calib_b);
	// 	#endif
	// }

	TCut mycut(cut);
	tree1->Draw(Form("%s:%s>>hist",fillname2.Data(),fillname1.Data()),mycut.GetTitle());

    TString canvas_name = TString::Format("canvas_%d", canvas_count++);
    //TString canvas_name = TString::Format("canvas_%s", fillname2.Data());
	TCanvas *c1 = new TCanvas(canvas_name,"",800,600);
	//TCanvas *c1 = new TCanvas("c1","",800,600);
	c1->cd();
	// 绘制直方图
	hist->Draw();
	c1->SaveAs(Form("rootplot/2Dhist_%svs%s.png",fillname1.Data(),fillname2.Data()));
}

void octopus_2D(){
	
	gStyle->SetOptStat(0);	
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");
	int canvas_count = 0; 

	int nbin = 110*4;
	double min=-1;
	//double max=12E3;
	double max=10;

	//const char *cut = "decaytime > 0.001 && decaytime < 0.04 && risetime < 0.035 && risetime > 0.0015 &&  slope > -8000 && slope < 4000 && maxtime < 45E-3 && starttime1 < stoptime1 && starttime2 < stoptime2 && starttime1 > 0.01 && stoptime1 > 0.028 && starttime2 > 0.03 && stoptime2 > 0.03";
	const char *cut = "decaytime < 999";
	//const char *cut = "!(decaytime > 0.005 && decaytime < 0.06 && risetime > 0.002 && risetime < 0.03   && starttime1 > 0.03 && starttime1 < 0.1 && stoptime1 > 0.03 && stoptime1 < 0.1 && starttime2 > 0.03 && stoptime2 > 0.03)";


	//const char *file1="../rootfile/Processed_20240820T010600_000012_1.root";
	//const char *file1="../rootfile/Processed_20240824T130500_000013_1.root";
	//const char *file1="../rootfile/Processed_20241207T161500_000014_1.root";
	//const char *file1="../rootfile/Processed_20241208T213600_000001_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/DLMO/Processed_20250420T1028_042001_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/Processed_20250420T1028_042001_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/DLMO/Processed_20250420T1445_042002_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/DLMO/Processed_20250420T1854_042003_1.root";
	//const char *file1="/mnt/c/Users/sky/Desktop/temp/DLMO/Processed_20250421T0953_042101_1.root";

	//const char *file1="/mnt/c/Users/sky/Downloads/Reconstructed/Processed_20250511T000112_100000_1.root";
	//const char *file1="/mnt/c/Users/sky/Downloads/Reconstructed/Processed_20250510T000112_000000_1.root";//USTC-LD
	//const char *file1="/mnt/c/Users/sky/Downloads/Reconstructed/Processed_20250510T000175_000001_1.root";//USTC-LMO
	//const char *file1="/mnt/c/Users/sky/Downloads/Reconstructed/Processed_20250511T000112_100000_1.root";//FDU-LD
	const char *file1="/mnt/c/Users/sky/Downloads/Reconstructed/Processed_20250511T000260_100002_1.root";//FDU-LMO
	



	 //draw(file1,"amplitude","risetime",nbin,min,max,200,0,0.2,cut);
	 //draw(file1,"amplitude","decaytime",nbin,min,max,200,0,0.2,cut);
	// draw(file1,"amplitude","starttime1",nbin,min,max,100,0,0.1,cut);
	// draw(file1,"amplitude","stoptime1",nbin,min,max,100,0,0.1,cut);
	// draw(file1,"amplitude","starttime2",nbin,min,max,100,0,0.1,cut);
	// draw(file1,"amplitude","stoptime2",nbin,min,max,100,0,0.1,cut);
	//draw(file1,"risetime","decaytime",200,0,0.02,200,0,0.2,cut);
	draw(file1,"amplitude","RMS",nbin,min,max,200,0,0.02,cut);

	//draw(file1,"amplitude","baseline",200,0,1E8,100,-20000,-100000,cut);
	//draw(file1,"amplitude","baseline",200,0.05,0.35,100,-17,-15,cut);



}
