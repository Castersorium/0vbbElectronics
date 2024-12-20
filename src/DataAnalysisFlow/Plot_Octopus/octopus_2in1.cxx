#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>

int canvas_count = 0; 
TString x_title="title"; 

#define CALIB
#define calib_k 478.5860639
#define calib_b 2.373786877

//void draw(const char *rootfile, const char *fillname, int nbin, double nmin, double nmax, const char *cut) {
void gettree(const char *rootfile, TString fillname, int nbin, double nmin, double nmax, TH1D *&h1, TH1D *&h2, const char *cut) {
	
	x_title=fillname;
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

	// 初始化计数器
	h1 = new TH1D("h1", "", nbin, nmin, nmax);  // 定义直方图
	h2 = new TH1D("h2", "", nbin, nmin, nmax);  // 定义直方图	

	if (fillname == "amplitude") {
		#ifdef CALIB
			fillname = Form("amplitude*%f + %f", calib_k, calib_b);
		#endif
	}
	TCut mycut(cut);
	tree1->Draw(Form("%s>>h1",fillname.Data()));
	tree1->Draw(Form("%s>>h2",fillname.Data()),mycut.GetTitle());

}

void drawhist(TH1D*& h1, const char *tleg1, double time1, TH1D*& h2,  const char *tleg2, double time2){
	int entry1 = h1->GetEntries();
	int entry2 = h2->GetEntries();

    TString canvas_name = TString::Format("canvas_%d", canvas_count++);
	TCanvas *c = new TCanvas(canvas_name,"",1200,600);
	c->cd();
	// 绘制直方图
	h1->SetLineColor(kRed);
	h2->SetLineColor(kBlue);
	h1->SetFillColor(kRed);
	h2->SetFillColor(kBlue);
	h1->SetFillStyle(3005);
	h2->SetFillStyle(3004);
	h1->SetYTitle("Events");
	h1->SetXTitle(Form("%s",x_title.Data()));
	//h1->GetXaxis()->SetTitleSize(1);  // 设置直方图总标题字体大小
	int Islog = 0;
	if (x_title == "amplitude") {
		Islog = 1;
		#ifdef CALIB
			h1->SetXTitle("Energy(keV)");
			h2->SetXTitle("Energy(keV)");

		#else
			h1->SetXTitle("ADC Voltage(mV)");
			h2->SetXTitle("ADC Voltage(mV)");
		#endif
	}


	if (Islog){
	h2->Scale(1/time2);
	h1->Scale(1/time1);
	}
	else{
	h2->Scale(1./h2->Integral());
	h1->Scale(1./h1->Integral());
	}
	
	if(h1->GetMaximum() > h2->GetMaximum()){
	//h2->GetYaxis()->SetRangeUser(0,1.5*h1->GetMaximum());
	h1->Draw("HIST");
	h2->Draw("HISTSAME");
	}
	else{
	h2->Draw("HIST");
	h1->Draw("HISTSAME");
	}

	TLegend* leg = new TLegend(0.6, 0.7, .9, .9);
	//leg->AddEntry(h2, "Nocut", "lp");
	//leg->AddEntry(h1, "Cut", "lp");
	//leg->AddEntry(h1, Form("%s	=%d",tleg1,entry1), "f");
	//leg->AddEntry(h2, Form("%s	=%d",tleg2,entry2), "f");
	leg->AddEntry(h1, Form("%s",tleg1), "f");
	leg->AddEntry(h2, Form("%s",tleg2), "f");
	leg->Draw();

	if (Islog) c->SetLogy();
	c->SaveAs(Form("rootplot/twohist%d.png",canvas_count));

	}

void octopus_2in1(){
	
	gStyle->SetOptStat(0);	
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");

	//const char *file1="../rootfile/Processed_20240820T010600_000012_1.root";
	const char *file1="../rootfile/Processed_20240824T130500_000013_1.root";
	//const char *file2="../rootfile/Processed_20241207T161500_000014_1.root";
	const char *file2="../rootfile/Processed_20241208T213600_000001_1.root";

	//const char *cut = "decaytime > 0.001 && decaytime < 0.04 && risetime < 0.035 && risetime > 0.0015 &&  slope > -8000 && slope < 4000 && maxtime < 45E-3 && starttime1 < stoptime1 && starttime2 < stoptime2 && starttime1 > 0.01 && stoptime1 > 0.028 && starttime2 > 0.03 && stoptime2 > 0.03";
	const char *cut1 = "decaytime > 0.004 && decaytime < 0.06 && risetime > 0.002 && risetime < 0.04   && starttime1 > 0.03 && stoptime1 > 0.03 && starttime2 > 0.03 && stoptime2 > 0.03";
	const char *cut2 = "decaytime > 0.005 && decaytime < 0.06 && risetime > 0.002 && risetime < 0.03   && starttime1 > 0.03 && starttime1 < 0.1 && stoptime1 > 0.03 && stoptime1 < 0.1 && starttime2 > 0.03 && stoptime2 > 0.03";

	double t1=16802.5192;
	//double t2=16202.4292;
	double t2=32400;

	//TH1D *h1, *h2, *h3, *h4, *h5, *h6 = nullptr;
	// int nbin = 71*4;
	// double min=-1E2;
	// double max=7E3;
	// TString *var = "amplitude ";
	// draw(file1,var,nbin,min,max,h3,h4,cut1);
	// draw(file2,var,nbin,min,max,h5,h6,cut2);
	// drawhist(h4,"With Am",t1,h6,"Without Am",t2);

    std::vector<std::string> variable = {"amplitude","risetime","decaytime"};
	std::vector<int> nbins = {71*4,100,100};
    std::vector<double> mins = {-1E2,0,0};
 	std::vector<double> maxs = {7E3,0.05,0.06};


   for (size_t i = 0; i < variable.size(); ++i) {
		const auto &var = variable[i];
        int nbin = nbins[i];
        double min = mins[i];
        double max = maxs[i];     
		TH1D *h1, *h2, *h3, *h4, *h5, *h6 = nullptr;
		gettree(file1,var,nbin,min,max,h3,h4,cut1);
		gettree(file2,var,nbin,min,max,h5,h6,cut1);
		drawhist(h4,"With Am",t1,h6,"Without Am",t2);
	}

}
