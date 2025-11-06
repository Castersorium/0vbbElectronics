#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <TCanvas.h>
#include <TLegend.h>
#include <cmath>
#include "TGraph.h"
#include "TF1.h"
#include "TreeLoader.h"

int canvas_count = 0; 

#define CALIB
//#define calib_k 0.483618264*1E3
//#define calib_b 2.426496603*1E3
#define calib_k 478.5860639
#define calib_b 2.373786877

//void draw(const char *rootfile, const char *fillname, int nbin, double nmin, double nmax, const char *cut) {
void draw(const char *rootfile, int nbin, double nmin, double nmax, double length, double tolerence, double bingo) {
	
	//LoadTrees(rootfile);
	TTree *tree1, *tree2, *tree3, *tree4, *tree5, *tree6;
    LoadTrees(rootfile, tree1, tree2, tree3, tree4, tree5, tree6);

    // 在draw函数中定义需要的变量
    double amplitude, baseline, risetime, decaytime, timestamp;

    // 设置树的分支地址
    tree1->SetBranchAddress("baseline", &baseline);
    tree3->SetBranchAddress("amplitude", &amplitude);
    tree4->SetBranchAddress("risetime", &risetime);
    tree5->SetBranchAddress("decaytime", &decaytime);
    tree6->SetBranchAddress("timestamp", &timestamp);


    // 创建直方图
    TH1F *hist  = new TH1F("hist", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数
    TH1F *hist1 = new TH1F("hist1", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数

    TH2F *hist2d  = new TH2F("hist2d", "2D Histogram",  nbin, nmin, nmax, 100, -20, -5);  // 请根据需要调整范围
    TH2F *hist2d1 = new TH2F("hist2d1", "2D Histogram", nbin, nmin, nmax, 100, -20, -5);  // 请根据需要调整范围
    TH2F *hist2d2 = new TH2F("hist2d2", "2D Histogram", nbin, nmin, nmax, 100, 0, 0.05);  // 请根据需要调整范围
    TH2F *hist2d3 = new TH2F("hist2d3", "2D Histogram", nbin, nmin, nmax, 100, 0, 0.3);  // 请根据需要调整范围
    TGraph *graph = new TGraph();

    // 循环遍历 TTree 中的所有事件
    Long64_t nEntries = tree1->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree1->GetEntry(i);

		double timestamp = 987654321E10; //dummy value
		// 计算商
    	int quotient = (timestamp-bingo) / length;
    	// 计算余数 (浮点余数)
    	//double remainder = fmod((timestamp-bingo), length);
    	double remainder = ( timestamp - bingo ) - length * quotient;	
		//去掉setpoint前面的
		if (remainder < 0) continue;


        // 查找在 timestamp 区间内的 event
        //if ( remainder < tolerence || remainder > (length - tolerence) )  {

			//if (decaytime < 0.1) continue;
			//if (risetime < 0.01) continue;
			if (amplitude < 4) continue;

            // 填充直方图
            hist->Fill(amplitude);          // 填充 amp 到 hist1
            // hist1->Fill(baseline);          // 填充 amp 到 hist1
            hist2d1->Fill(amplitude, baseline);  // 填充 amp 和 timestamp 到 hist2d
            hist2d2->Fill(amplitude, risetime);  // 填充 amp 和 timestamp 到 hist2d
            hist2d3->Fill(amplitude, decaytime);  // 填充 amp 和 timestamp 到 hist2d
            // 将 (timestamp, amp) 数据点添加到 TGraph 中
            graph->SetPoint(graph->GetN(), baseline, amplitude);

    		// std::cout << std::setprecision(20) << "Found time:" << timestamp << std::endl;  // 输出 3.14
    		// std::cout << std::setprecision(10) << ":" << quotient << std::endl;  // 输出 3.14
    		// std::cout << std::setprecision(10) << ":" << remainder << std::endl;  // 输出 3.14
		//}
    }

	TCanvas *c1 = new TCanvas("c1","",800,600);
	TCanvas *c2 = new TCanvas("c2","",800,600);
	TCanvas *c3 = new TCanvas("c3","",800,600);

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
	c1->SaveAs("/mnt/c/Users/sky/Desktop/plot/2d_baseline.png");
	c2->cd();
	hist2d2->Draw();
	c2->SaveAs("/mnt/c/Users/sky/Desktop/plot/2d_risetime.png");	
	c3->cd();
	hist2d3->Draw();
	c3->SaveAs("/mnt/c/Users/sky/Desktop/plot/2d_decaytime.png");


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

void octopus_heater3(){
	//gStyle->SetOptStat(0);	
	gROOT->SetStyle("Modern");

	//for adc
	int nbin = 3.1*40;
	double min=- -0.1;
	double max= 3;
	
	int length=180;
	double tolerence=2;
	double bingo= 1734957078.30760002;

	//const char *file1="../rootfile/Processed_20240820T010600_000012_1.root";
	//const char *file1="../rootfile/Processed_20240824T130500_000013_1.root";
	//const char *file2="../rootfile/Processed_20241207T161500_000014_1.root";
	//const char *file2="../rootfile/Processed_20241208T213600_000001_1.root";
	const char *file="/mnt/c/Users/sky/Downloads/Heater/Processed_20251103T003500_000001_1.root";

	//LoadTrees(file);
	//TTree *tree1, *tree2, *tree3, *tree4, *tree5, *tree6;
    //LoadTrees(file, tree1, tree2, tree3, tree4, tree5, tree6);
	//draw(file1,"amplitude",nbin,min,max,cut);
	//draw(file2,"amplitude",60*4,0,4E-2,cut1);
	//draw(file1,"RMS",151*2,-1E3,15E4,cut);
	draw(file,nbin,min,max,length,tolerence,bingo);

}
