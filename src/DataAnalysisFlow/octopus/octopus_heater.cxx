#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>
#include <cmath>
#include "TGraph.h"
#include "TF1.h"

int canvas_count = 0; 

#define CALIB
//#define calib_k 0.483618264*1E3
//#define calib_b 2.426496603*1E3
#define calib_k 478.5860639
#define calib_b 2.373786877

//void draw(const char *rootfile, const char *fillname, int nbin, double nmin, double nmax, const char *cut) {
void draw(const char *rootfile, int nbin, double nmin, double nmax, double length, double tolerence, double bingo) {
	
	// 打开 ROOT 文件
	TFile *file = new TFile(rootfile, "READ");
	// 获取各个树
	TTree *tree1 = (TTree*)file->Get("baseline");
	TTree *tree2 = (TTree*)file->Get("baselineslope");
	TTree *tree3 = (TTree*)file->Get("maxminusbaseline");
	TTree *tree4 = (TTree*)file->Get("risetime");
	TTree *tree5 = (TTree*)file->Get("decaytime");
	TTree *tree6 = (TTree*)file->Get("timestamp");
	TTree *tree7 = (TTree*)file->Get("numberoftriggers");


	// 定义变量，用于存储树中的数据
	double RMS, baseline;
	double offset, slope;
	double amplitude, maxtime;
	double risetime, decaytime;
	double starttime,stoptime;
	double starttime1,starttime2;
	double stoptime1,stoptime2;
	double timestamp;
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
	tree6->SetBranchAddress("timestamp", &timestamp);
	tree7->SetBranchAddress("numberoftriggers", &numberoftriggers);


	// 将所有树作为 tree1 的朋友树
	tree1->AddFriend(tree2);
	tree1->AddFriend(tree3);
	tree1->AddFriend(tree4,"friend4");
	tree1->AddFriend(tree5,"friend5");
	tree1->AddFriend(tree6);
	tree1->AddFriend(tree7);


	tree1->SetAlias("starttime1","friend4.starttime");
	tree1->SetAlias("stoptime1","friend4.stoptime");
	tree1->SetAlias("starttime2","friend5.starttime");
	tree1->SetAlias("stoptime2","friend5.stoptime");

    // 创建直方图
    TH1F *hist  = new TH1F("hist", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数
    TH1F *hist1 = new TH1F("hist1", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数

    TH2F *hist2d  = new TH2F("hist2d", "2D Histogram",  nbin, nmin, nmax, 100, -20, -5);  // 请根据需要调整范围
    TH2F *hist2d1 = new TH2F("hist2d1", "2D Histogram", nbin, nmin, nmax, 100, -10, 0);  // 请根据需要调整范围
    TH2F *hist2d2 = new TH2F("hist2d2", "2D Histogram", nbin, nmin, nmax, 100, 0, 0.4);  // 请根据需要调整范围
    TH2F *hist2d3 = new TH2F("hist2d3", "2D Histogram", nbin, nmin, nmax, 100, 0, 0.4);  // 请根据需要调整范围
    TH2F *hist2d4 = new TH2F("hist2d4", "2D Histogram", nbin, nmin, nmax, 10, 0, 10);  // 请根据需要调整范围
    TGraph *graph = new TGraph();

    TGraph *gAmpVsBsl_before = new TGraph();
    TGraph *gAmpVsBsl_after  = new TGraph();

    // 循环遍历 TTree 中的所有事件
    Long64_t nEntries = tree1->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree1->GetEntry(i);
		tree2->GetEntry(i);
        tree3->GetEntry(i);
        tree4->GetEntry(i);
        tree5->GetEntry(i);
        tree6->GetEntry(i);

		// 计算商
    	int quotient = (timestamp-bingo) / length;
    	// 计算余数 (浮点余数)
    	//double remainder = fmod((timestamp-bingo), length);
    	double remainder = ( timestamp - bingo ) - length * quotient;	
		//去掉setpoint前面的
		if (remainder < 0) continue;


        // 查找在 timestamp 区间内的 event
        // if ( remainder < tolerence || remainder > (length - tolerence) )  {

			if (baseline < -9) continue;
			if (numberoftriggers !=1) continue;
			// if (decaytime < 0.1) continue;
			// if (risetime > 0.05) continue;
			// if (amplitude < 3 || amplitude > 7) continue;
			if (amplitude < baseline *(-0.912313) - 0.9 ) continue;

            // 填充直方图
            hist->Fill(amplitude);          // 填充 amp 到 hist1
            // hist1->Fill(baseline);          // 填充 amp 到 hist1
            hist2d1->Fill(amplitude, baseline);  // 填充 amp 和 timestamp 到 hist2d
            hist2d2->Fill(amplitude, risetime);  // 填充 amp 和 timestamp 到 hist2d
            hist2d3->Fill(amplitude, decaytime);  // 填充 amp 和 timestamp 到 hist2d
            hist2d4->Fill(amplitude, numberoftriggers);  // 填充 amp 和 timestamp 到 hist2d
            // 将 (timestamp, amp) 数据点添加到 TGraph 中
            graph->SetPoint(graph->GetN(), baseline, amplitude);
   		    gAmpVsBsl_before->SetPoint(gAmpVsBsl_before->GetN(), baseline, amplitude);
       		double baseline0 = -9; // 可自行设定为平均或参考 baseline
        	double amp_stab = amplitude - (-0.912313) * (baseline - baseline0);

        	// === 新增：记录稳定化后 amp vs baseline ===
        	gAmpVsBsl_after->SetPoint(gAmpVsBsl_after->GetN(), baseline, amp_stab);

    		// std::cout << std::setprecision(20) << "Found time:" << timestamp << std::endl;  // 输出 3.14
    		// std::cout << std::setprecision(10) << ":" << quotient << std::endl;  // 输出 3.14
    		// std::cout << std::setprecision(10) << ":" << remainder << std::endl;  // 输出 3.14
		// }
    }

	TCanvas *c1 = new TCanvas("c1","",800,600);
	TCanvas *c2 = new TCanvas("c2","",800,600);
	TCanvas *c3 = new TCanvas("c3","",800,600);
	TCanvas *c4 = new TCanvas("c4","",800,600);

	hist2d1->SetYTitle("baseline");
	hist2d1->SetXTitle("ADC");	
	hist2d1->SetTitle("baseline vs ADC");
	hist2d2->SetYTitle("risetime");
	hist2d2->SetXTitle("ADC");
	hist2d2->SetTitle("risetime vs ADC");
	hist2d3->SetYTitle("decaytime");
	hist2d3->SetXTitle("ADC");
	hist2d3->SetTitle("decaytime vs ADC");

	c1->cd();
	hist2d1->Draw();
	c1->SaveAs("../../../output/plots/2d_baseline.png");
	c2->cd();
	hist2d2->Draw();
	c2->SaveAs("../../../output/plots/2d_risetime.png");	
	c3->cd();
	hist2d3->Draw();
	c3->SaveAs("../../../output/plots/2d_decaytime.png");
	c4->cd();	
	hist2d4->Draw();
	c4->SaveAs("../../../output/plots/2d_numberoftriggers.png");

    // === 新增：绘制 amp vs baseline（前后对比） ===
    TCanvas *cAmp = new TCanvas("cAmp", "Amp vs Baseline (Before & After)", 800, 600);
    gAmpVsBsl_before->SetMarkerStyle(20);
    gAmpVsBsl_before->SetMarkerColor(kRed);
    gAmpVsBsl_before->SetMarkerSize(0.6);
    gAmpVsBsl_before->SetTitle("Amplitude vs Baseline;Baseline;Amplitude");
    gAmpVsBsl_before->Draw("AP");

    gAmpVsBsl_after->SetMarkerStyle(24);
    gAmpVsBsl_after->SetMarkerColor(kBlue);
    gAmpVsBsl_after->SetMarkerSize(0.6);
    gAmpVsBsl_after->Draw("P SAME");

    // 加图例
    auto legend = new TLegend(0.65, 0.75, 0.88, 0.88);
    legend->AddEntry(gAmpVsBsl_before, "Before stabilization", "p");
    legend->AddEntry(gAmpVsBsl_after, "After stabilization", "p");
    legend->Draw();


if (1){
    // 做线性拟合
    TF1 *fitFunc = new TF1("fitFunc", "[0] + [1]*x", 0, 2); // 拟合函数: y = [0] + [1]*x
    graph->Fit(fitFunc);
	//hist2d->Fit(fitFunc,"R");

	double stable_b=fitFunc->GetParameter(0);
	double stable_k=fitFunc->GetParameter(1);
    // 输出拟合结果
    std::cout << "拟合结果: " << std::endl;
    std::cout << "截距 (b) = " << stable_b << std::endl;
    std::cout << "斜率 (m) = " << stable_k << std::endl;

    //创建画布
    TCanvas *canvas = new TCanvas("canvas", "拟合结果", 800, 600);
	canvas->cd();
	// 设置点的大小
    graph->SetMarkerSize(1.5);  // 设置点的大小为默认的 1.5 倍
    // 设置点的样式 (比如圆点)
    graph->SetMarkerStyle(20);  // 20 是圆形标记
    graph->Draw("AP"); // "A" 绘制数据点，"P" 使用点标记
	graph->GetXaxis()->SetTitle("Baseline");
	graph->GetYaxis()->SetTitle("ADC");
   	// 绘制拟合曲线
    fitFunc->Draw("same");  // "same" 表示在同一张图上绘制拟合曲线
	canvas->SaveAs("../../../output/plots/heater_fit.png");

}}

void octopus_heater(){
	//gStyle->SetOptStat(0);	
	gROOT->SetStyle("Modern");

	//for adc
	int nbin = 8.1*40;
	double min=- -0.1;
	double max= 8;
	
	int length=180;
	double tolerence=2;
	double bingo= 1734957078.30760002;

	//const char *file1="../rootfile/Processed_20240820T010600_000012_1.root";
	//const char *file1="../rootfile/Processed_20240824T130500_000013_1.root";
	//const char *file2="../rootfile/Processed_20241207T161500_000014_1.root";
	//const char *file2="../rootfile/Processed_20241208T213600_000001_1.root";
	const char *file="/mnt/c/Users/sky/Downloads/Heater/heater3.root";

	//draw(file1,"amplitude",nbin,min,max,cut);
	//draw(file2,"amplitude",60*4,0,4E-2,cut1);
	//draw(file1,"RMS",151*2,-1E3,15E4,cut);
	draw(file,nbin,min,max,length,tolerence,bingo);

}
