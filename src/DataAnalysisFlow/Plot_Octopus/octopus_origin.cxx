#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>

#define CALIB
#define calib_k 0.77049725
#define calib_b 3.8658775

void draw(const char *rootfile, const char *fillname, int nbin, double nmin, double nmax) {
	// 打开 ROOT 文件
	TFile *file = new TFile(rootfile, "READ");

	// 获取各个树
	TTree *tree1 = (TTree*)file->Get("baseline");
	TTree *tree2 = (TTree*)file->Get("baselineslope");
	TTree *tree3 = (TTree*)file->Get("maxminusbaseline");
	TTree *tree4 = (TTree*)file->Get("risetime");
	TTree *tree5 = (TTree*)file->Get("decaytime");


	// 将所有树作为 tree1 的朋友树
	tree1->AddFriend(tree2);
	tree1->AddFriend(tree3);
	tree1->AddFriend(tree4);
	tree1->AddFriend(tree5);

	// 定义变量，用于存储树中的数据
	double RMS, baseline;
	double offset, slope;
	double amplitude, maxtime;
	double risetime, decaytime;
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

	// 初始化计数器
	int count0 = 0, count1 = 0;
	TH1D *h1 = new TH1D("h1", "", nbin, nmin, nmax);  // 定义直方图
	TH1D *h2 = new TH1D("h2", "", nbin, nmin, nmax);  // 定义直方图
	TH2D *hist = new TH2D("hist", "amplitude vs risetime", 100, 0, 8E3, 100, 0., .07);
	// 设置 X 和 Y 轴的标题
	hist->SetXTitle("amplitude");
	hist->SetYTitle("");

	// 遍历所有条目
	for (int i = 0; i < tree1->GetEntries(); i++) {
		// 获取树的数据
		tree1->GetEntry(i);
		tree2->GetEntry(i);
		tree3->GetEntry(i);
		tree4->GetEntry(i);
		tree5->GetEntry(i);

		count0++;
		if (strcmp(fillname, "RMS") == 0) {
			h2->Fill(RMS);  // 填充 RMS
		} else if (strcmp(fillname, "baseline") == 0) {
			h2->Fill(baseline);  // 填充 baseline
		} else if (strcmp(fillname, "offset") == 0) {
			h2->Fill(offset);  // 填充 offset
		} else if (strcmp(fillname, "slope") == 0) {
			h2->Fill(slope);  // 填充 slope
		} else if (strcmp(fillname, "maxtime") == 0) {
			h2->Fill(maxtime);  // 填充 slope
		} else if (strcmp(fillname, "risetime") == 0) {
			h2->Fill(risetime);  // 填充 slope
		} else if (strcmp(fillname, "decaytime") == 0) {
			h2->Fill(decaytime);  // 填充 slope
		} else if (strcmp(fillname, "amplitude") == 0) {
#ifdef CALIB
			h2->Fill(amplitude* calib_k + calib_b);  // 填充 Am241 calib。。。 energy
#else
			h2->Fill(amplitude);  // 填充 amplitude
#endif
		}

		// 判断条件
		if (decaytime > 0.001 && decaytime < 0.04 && risetime < 0.035 && risetime > 0.0015 &&  slope > -8000 && slope < 4000 && maxtime < 45E-3 && starttime1 < stoptime1 && starttime2 < stoptime2 && starttime1 > 0.01 && stoptime1 > 0.028 && starttime2 > 0.03 && stoptime2 > 0.03)
		//if ( 0 )
		{
			count1++;
			//h1->Fill(Form("%s",fillname));  // 将 RMS 填充到直方图
			if (strcmp(fillname, "RMS") == 0) {
				h1->Fill(RMS);  // 填充 RMS
			} else if (strcmp(fillname, "baseline") == 0) {
				h1->Fill(baseline);  // 填充 baseline
			} else if (strcmp(fillname, "offset") == 0) {
				h1->Fill(offset);  // 填充 offset
			} else if (strcmp(fillname, "slope") == 0) {
				h1->Fill(slope);  // 填充 slope
			} else if (strcmp(fillname, "maxtime") == 0) {
				h1->Fill(maxtime);  // 填充 slope
			} else if (strcmp(fillname, "risetime") == 0) {
				h1->Fill(risetime);  // 填充 slope
			} else if (strcmp(fillname, "decaytime") == 0) {
				h1->Fill(decaytime);  // 填充 slope
			} else if (strcmp(fillname, "amplitude") == 0) {
#ifdef CALIB
				h1->Fill(amplitude* calib_k + calib_b);  // 填充 Am241 calib。。。 energy
#else
				h1->Fill(amplitude);  // 填充 amplitude
#endif
			}
			//hist->Fill(maxtime,decaytime);
			//hist->Fill(decaytime,maxtime);
			hist->Fill(amplitude,decaytime);

		}

	}
	// 输出计数
	std::cout << "Beforecut: " << count0 << std::endl;
	std::cout << "Aftercut : " << count1 << std::endl;

	//TCanvas *c = new TCanvas("c","",800,600);
	TCanvas *c = new TCanvas("c","",1200,600);
	c->cd();
	// 绘制直方图
	h1->SetLineColor(kRed);
	h2->SetLineColor(kBlue);
	h1->SetFillColor(kRed);
	h2->SetFillColor(kBlue);
	h1->SetFillStyle(3005);
	h2->SetFillStyle(3004);
	h2->Draw();
	h1->Draw("same");
	//h2->SetXTitle(Form("%s",condition));
	h2->SetTitle(" decaytime > 0.001 && decaytime < 999 && risetime < 0.035 && risetime > 0.0015 &&  slope > -8000 && slope < 4000 && maxtime < 45E-3");
	//h2->SetTitleSize(5);  // 设置直方图总标题字体大小
	#ifdef CALIB
		h2->SetXTitle("Energy(keV)");
		//h2->SetXTitle("ADC Voltage(mV)");
	#else
		h2->SetXTitle("ADC (ch)");
	#endif
	h2->SetYTitle("Events");
	TLegend* leg = new TLegend(0.6, 0.7, .9, .9);
	//leg->AddEntry(h2, "Nocut", "lp");
	//leg->AddEntry(h1, "Cut", "lp");
	//leg->AddEntry(h2, "Nocut", "f");
	//leg->AddEntry(h1, "Cut", "f");
	leg->AddEntry(h2, Form("Before=%d",count0), "f");
	leg->AddEntry(h1, Form("After =%d",count1), "f");
	leg->Draw();

	TPaveText *border = new TPaveText(0.15, 0.7, 0.4, 0.89, "NDC");
	border->SetFillColor(0); // 设置边框填充颜色，0表示透明
	border->SetLineColor(0); // 设置边框颜色
	border->SetLineWidth(2); // 设置边框宽度
	border->SetShadowColor(0); // 设置阴影颜色，0表示透明
	border->SetTextAlign(12); // 12 表示左对齐 22居中 32右对齐
	border->AddText(Form("N_{nocut}=%d",count0)); // 向 TPaveText 中添加文本
	border->AddText(Form("N_{cut}  =%d",count1)); // 向 TPaveText 中添加文本
						      //border->Draw();


	c->SetLogy();
	c->SaveAs(Form("rootplot/%s.png",fillname));

	TCanvas *c1 = new TCanvas("c1","",1200,600);
	c1->cd();
	// 绘制直方图
	hist->Draw();
	c->SaveAs("rootplot/2D.png");

}

void octopus(){
	gStyle->SetOptStat(0);	
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");
	//const char *condition = "decaytime > 0.001 && decaytime < 0.04 && risetime < 0.035 && risetime > 0.0015 &&  slope > -8000 && slope < 4000 && maxtime < 45E-3 && starttime1 < stoptime1 && starttime2 < stoptime2 && starttime1 > 0.01 && stoptime1 > 0.028 && starttime2 > 0.03 && stoptime2 > 0.03";
	//const char *condition = "decaytime < 999 && starttime1 > 0.029";
	draw("../rootfile/SB13_source.root","amplitude",71*4,-1E2,7E3);
}
