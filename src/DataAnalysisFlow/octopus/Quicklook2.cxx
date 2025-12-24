#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>

#define OF
int canvas_count;

void draw(const char *rootfile, TString fillname1, TString fillname2,int nbin, double nmin, double nmax, int nbin2, double nmin2, double nmax2, const char *cut) {
	
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
		tree7->SetBranchAddress("Chi2", &OF_Chi2);
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
	#endif

	tree1->SetAlias("starttime1","friend4.starttime");
	tree1->SetAlias("stoptime1","friend4.stoptime");
	tree1->SetAlias("starttime2","friend5.starttime");
	tree1->SetAlias("stoptime2","friend5.stoptime");
	#ifdef OF
		tree1->SetAlias("OF_amplitude","friend7.amplitude");
		tree1->SetAlias("OF_Chi2","friend7.Chi2");
	#endif

	// =========================
	//  Scatter plot version
	// =========================

	// 先申请动态数组来放数据
	std::vector<double> xs, ys;

	// 用 tree->Draw 把数据读出来
	tree1->Draw(Form("%s:%s", fillname2.Data(), fillname1.Data()), cut, "goff");

	int n = tree1->GetSelectedRows();   // 行数
	xs.reserve(n);
	ys.reserve(n);

	double *vx = tree1->GetV2();   // 第一个变量（x）
	double *vy = tree1->GetV1();   // 第二个变量（y）

	for (int i = 0; i < n; i++) {
		xs.push_back(vx[i]);
		ys.push_back(vy[i]);
	}

	// 用这些点生成 scatter plot
	TGraph *gr = new TGraph(n, xs.data(), ys.data());
	gr->SetTitle(Form("%s vs %s", fillname1.Data(), fillname2.Data()));
	gr->SetMarkerStyle(20);
	gr->SetMarkerSize(0.5);
	gr->SetMarkerColor(kBlue);

	TString canvas_name = TString::Format("canvas_%d", canvas_count++);
	TCanvas *c1 = new TCanvas(canvas_name, "", 800, 600);	
	c1->cd();
	gr->GetXaxis()->SetTitle(fillname1);
	gr->GetYaxis()->SetTitle(fillname2);
	gr->Draw("AP");   // A: axis, P: points

	TF1 *fitfunc = new TF1("fitfunc", "[0] + [1]*x", 2.9, 3.0);
	fitfunc->SetLineColor(kRed);
	fitfunc->SetLineWidth(2);
	gr->Fit(fitfunc, "R");  // "R" 遵循范围

	c1->SaveAs(Form("rootplot/Scatter_%svs%s.png", fillname1.Data(), fillname2.Data()));
	// c1->SaveAs(Form("rootplot/Scatter_%svs%s.root", fillname1.Data(), fillname2.Data()));

	double p0 = fitfunc->GetParameter(0);
	double p1 = fitfunc->GetParameter(1);

	// 新的点集
	std::vector<double> xs2, ys2;
	xs2.reserve(n);
	ys2.reserve(n);

	for (int i = 0; i < n; i++) {
		double x = xs[i];
		double y = ys[i];

		double denom = (y - p0) / p1;

		// 防止除零
		if (fabs(denom) < 1e-9) continue;

		double x_new = 3 * x / denom;
		double y_new = y;   // y' = y

		xs2.push_back(x_new);
		ys2.push_back(y_new);
	}

	// 生成新的 scatter 
	TGraph *gr2 = new TGraph(xs2.size(), xs2.data(), ys2.data());
	gr2->SetMarkerStyle(20);
	gr2->SetMarkerSize(0.5);
	gr2->SetMarkerColor(kRed);
	gr2->SetTitle("Rescaled Scatter Plot");

	TCanvas *c2 = new TCanvas("canvas_transformed", "", 800, 600);
	c2->cd();
	gr2->GetXaxis()->SetTitle("x' = a * x / ((y - p0)/p1)");
	gr2->GetYaxis()->SetTitle("y' = y");
	gr2->Draw("AP");

	c2->SaveAs("rootplot/Scatter_transformed.png");
					


}

void Quicklook2(){
	
	gStyle->SetOptStat(0);	
	gROOT->SetStyle("Modern");
	int canvas_count = 0; 

	const char *file1="/mnt/c/Users/sky/Desktop/temp/USTC/Processed_20251121T122223_000033_1.root";
	const char *cut0 = "decaytime < 99999 ";
	const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 ";

	int nbin1=5.1*200, nbin2=200, nbin3=200,nbin4=200;
	double min1=0.1,min2=0,min3=0,min4=0; 
	double max1=5,max2=0.015,max3=0.015,max4=5E6;
	// draw(file1,"amplitude","risetime",nbin1,min1,max1,nbin2,min2,max2,cut1);
	// draw(file1,"amplitude","decaytime",nbin1,min1,max1,nbin3,min3,max3,cut1);
	// draw(file1,"risetime","decaytime",nbin2,min2,max2,nbin3,min3,max3,cut1);
	draw(file1,"amplitude","baseline",nbin1,min1,max1,200,-10,10,cut1);

}
