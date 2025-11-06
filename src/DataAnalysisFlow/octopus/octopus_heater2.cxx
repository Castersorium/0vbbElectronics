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
#define calib_k -0.912313
#define calib_b -0.756264

//void draw(const char *rootfile, const char *fillname, int nbin, double nmin, double nmax, const char *cut) {
void draw(const char *rootfile, TString fillname, int nbin, double nmin, double nmax, const char *cut, double length, double tolerence, double bingo) {
	
	// 打开 ROOT 文件
	TFile *file = new TFile(rootfile, "READ");
	// 获取各个树
	TTree *tree1 = (TTree*)file->Get("baseline");
	TTree *tree2 = (TTree*)file->Get("baselineslope");
	TTree *tree3 = (TTree*)file->Get("maxminusbaseline");
	TTree *tree4 = (TTree*)file->Get("risetime");
	TTree *tree5 = (TTree*)file->Get("decaytime");
	TTree *tree6 = (TTree*)file->Get("timestamp");


	// 定义变量，用于存储树中的数据
	double RMS, baseline;
	double offset, slope;
	double amplitude, maxtime;
	double risetime, decaytime;
	double starttime,stoptime;
	double starttime1,starttime2;
	double stoptime1,stoptime2;
	double timestamp;

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

    // 创建直方图
    TH1F *hist  = new TH1F("hist", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数
    TH1F *hist1 = new TH1F("hist1", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数
    TH1F *hist2 = new TH1F("hist2", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数
    TH1F *hist3 = new TH1F("hist3", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数
    TH1F *hist4 = new TH1F("hist4", "Amplitude Histogram", nbin, nmin, nmax);  // 请根据需要调整范围和 bins 数

    TH2F *hist2d  = new TH2F("hist2d", "2D Histogram", nbin, -20, -10, nbin, 0, 2);  // 请根据需要调整范围
    TH2F *hist2d1 = new TH2F("hist2d1", "2D Histogram", nbin, -20, -10, nbin, 0, 2);  // 请根据需要调整范围
    TH2F *hist2d2 = new TH2F("hist2d2", "2D Histogram", nbin, -20, -10, nbin, 0, 2);  // 请根据需要调整范围
    TH2F *hist2d3 = new TH2F("hist2d3", "2D Histogram", 80, 0, 3, 80, -2, 2);  // 请根据需要调整范围
    TH2F *hist2d4 = new TH2F("hist2d4", "2D Histogram", 80, 0, 3, 80, 0, 0.05);  // 请根据需要调整范围
    TGraph *graph = new TGraph();

    // 循环遍历 TTree 中的所有事件
    Long64_t nEntries = tree1->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree1->GetEntry(i);

		// 计算商
    	int quotient = (timestamp-bingo) / length;
    	// 计算余数 (浮点余数)
    	//double remainder = fmod((timestamp-bingo), length);
    	double remainder = ( timestamp - bingo ) - length * quotient;
		
		if (amplitude < 3) continue;
		if (amplitude < baseline *(-0.8) - 0.2 ) continue;

        // 查找在 timestamp 区间内的 event
        // if ( remainder < tolerence || remainder > (length - tolerence) )  {
            // 填充直方图
            hist->Fill(amplitude);          // 填充 amp 到 hist1
            //hist1->Fill(baseline);          // 填充 amp 到 hist1
            //hist2d4->Fill(amplitude, baseline);  // 填充 amp 和 timestamp 到 hist2d
            //hist2d3->Fill(amplitude, risetime);  // 填充 amp 和 timestamp 到 hist2d
            //hist2d4->Fill(amplitude, decaytime);  // 填充 amp 和 timestamp 到 hist2d
            // 将 (timestamp, amp) 数据点添加到 TGraph 中
            //graph->SetPoint(graph->GetN(), amplitude, baseline);
            graph->SetPoint(graph->GetN(), baseline, amplitude);

    		// std::cout << std::setprecision(20) << "Found time:" << timestamp << std::endl;  // 输出 3.14
    		// std::cout << std::setprecision(10) << ":" << quotient << std::endl;  // 输出 3.14
    		// std::cout << std::setprecision(10) << ":" << remainder << std::endl;  // 输出 3.14
		// }
    }

	// TCanvas *c3 = new TCanvas("c3","",800,600);
	// c3->cd();
	// hist2d4->Draw();
	// hist2d4->SetTitle("ADC vs Baseline");
	// hist2d4->SetXTitle("ADC");
	// hist2d4->SetYTitle("Baseline");
	//c3->SaveAs("../../../output/plots/heater_cut.png");


if(1){
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
	//canvas->SaveAs("../../../output/plots/heater_fit.png");


    for (Long64_t i = 0; i < nEntries; ++i) {
        tree1->GetEntry(i);
		// 计算商
    	int quotient = (timestamp-bingo) / length;
    	// 计算余数 (浮点余数)
    	double remainder = ( timestamp - bingo ) - length * quotient;

        // if ( (remainder > 0 && decaytime > 0.1 && amplitude > 0.4)  &&   ( remainder < tolerence || remainder > (length - tolerence) ) )  {
		if ( amplitude < baseline *(-0.8) - 0.2){
			hist2d1->Fill(baseline,amplitude-0.3);
			hist2d2->Fill(baseline,amplitude + baseline*stable_k - stable_b);
		}
		else{
		    hist1->Fill(amplitude);
			hist2->Fill(amplitude + baseline*stable_k - stable_b);          // 填充 amp 到 hist1
			hist2d3->Fill(amplitude + baseline*stable_k - stable_b,slope);
			hist2d4->Fill(amplitude + baseline*stable_k - stable_b,RMS);

			double test= amplitude + baseline*stable_k - stable_b;
			//if (decaytime<0.04 && decaytime > 0.02 && test > 0.5 && test < 1.5){
			//if (risetime < 0.02){
			if (decaytime < 0.04 && risetime < 0.02){
				hist3->Fill(amplitude + baseline*stable_k - stable_b);
			}
			else{
				hist4->Fill(amplitude + baseline*stable_k - stable_b);
			}
		}
	}

	//tree1->Draw(Form("(amplitude + baseline * %f)>>hist3", stable_k));
	TCanvas *c1 = new TCanvas("c1","",1600,600);
	TCanvas *c2 = new TCanvas("c2","",800,600);
	TCanvas *c3 = new TCanvas("c3","",1600,600);

	c3->cd();
	hist3->Draw();
	//hist4->Draw("same");
	hist3->SetTitle(" ");
	hist3->SetXTitle("ADC");
	hist3->SetYTitle("Events");
	hist3->SetLineColor(kRed);
	hist4->SetLineColor(kBlue);
    TLegend *legend3 = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend3->AddEntry(hist3, "YES", "l");
    legend3->AddEntry(hist4, "NO", "l");
    //legend3->Draw();
	c3->SetLogy();
	c3->SaveAs("/mnt/c/Users/sky/Desktop/plot/3.png");

	c1->cd();
	hist1->SetTitle(" ");
	hist1->SetXTitle("ADC");
	hist1->SetYTitle("Events");
	hist1->SetLineColor(kRed);
	hist2->SetLineColor(kBlue);
	hist1->SetFillStyle(3005);
	hist2->SetFillStyle(3004);
	hist1->Draw();
	hist2->Draw("same");
    // 添加图例来区分两个直方图
    TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, "Original", "l");
    legend->AddEntry(hist2, "Stablized", "l");
    legend->Draw();
	c1->SetLogy();
	//c1->SaveAs("../../../output/plots/heater_amp.png");


	c2->cd();
	hist2d1->SetFillColor(kRed);
	hist2d2->SetFillColor(kBlue);
	hist2d1->SetLineColor(kRed);
	hist2d2->SetLineColor(kBlue);
	hist2d1->SetFillColorAlpha(kRed, 0.3);  // 半透明填充
	hist2d2->SetFillColorAlpha(kBlue, 0.3);  // 半透明填充
	hist2d1->SetTitle("Baseline VS ADC");
	hist2d1->SetXTitle("Baseline");
	hist2d1->SetYTitle("ADC");
	hist2d1->Draw("COLZ");
	hist2d2->Draw("SAME COLZ");
    // 添加图例来区分两个直方图
    TLegend *legend2 = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend2->AddEntry(hist2d1, "Original", "f");
    legend2->AddEntry(hist2d2, "Stablized", "f");
    //legend2->Draw();
	//c2->SaveAs("../../../output/plots/heater_2d.png");


	TCanvas *c4 = new TCanvas("c4","",800,600);	
	TCanvas *c5 = new TCanvas("c5","",800,600);	
	hist2d3->SetYTitle("slope");
	hist2d3->SetXTitle("ADC");	
	hist2d4->SetYTitle("RMS");
	hist2d4->SetXTitle("ADC");
	c4->cd();
	hist2d3->Draw();
	c5->cd();
	hist2d4->Draw();
	c4->SaveAs("/mnt/c/Users/sky/Desktop/plot/1.png");
	c5->SaveAs("/mnt/c/Users/sky/Desktop/plot/2.png");

}

	//c->SaveAs(Form("rootplot/Cut%d_%s.png",canvas_count,fillname.Data()));
}


void octopus_heater2(){
	gStyle->SetOptStat(0);	
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");

	int nbin = 71*4;
	double min=-1E2;
	double max=7E3;
	
	int length=180;
	double tolerence=2;
	double bingo= 1734957078.30760002;

	//const char *file1="../rootfile/Processed_20240820T010600_000012_1.root";
	//const char *file1="../rootfile/Processed_20240824T130500_000013_1.root";
	//const char *file2="../rootfile/Processed_20241207T161500_000014_1.root";
	//const char *file2="../rootfile/Processed_20241208T213600_000001_1.root";
	// const char *file="/mnt/c/Users/sky/Desktop/testcode/rootfile/Processed_20241223T202908_122320_1.root";
	const char *file="/mnt/c/Users/sky/Downloads/Heater/heater.root";


	//const char *cut = "decaytime > 0.001 && decaytime < 0.04 && risetime < 0.035 && risetime > 0.0015 &&  slope > -8000 && slope < 4000 && maxtime < 45E-3 && starttime1 < stoptime1 && starttime2 < stoptime2 && starttime1 > 0.01 && stoptime1 > 0.028 && starttime2 > 0.03 && stoptime2 > 0.03";
	const char *cut1 = "decaytime > 0.004 && decaytime < 0.06 && risetime > 0.002 && risetime < 0.04   && starttime1 > 0.03 && stoptime1 > 0.03 && starttime2 > 0.03 && stoptime2 > 0.03";
	const char *cut2 = "decaytime > 0.005 && decaytime < 0.06 && risetime > 0.002 && risetime < 0.03   && starttime1 > 0.03 && starttime1 < 0.1 && stoptime1 > 0.03 && stoptime1 < 0.1 && starttime2 > 0.03 && stoptime2 > 0.03";
	const char *cut = "decaytime < 999";

	//draw(file1,"amplitude",nbin,min,max,cut);
	//draw(file2,"amplitude",60*4,0,4E-2,cut1);
	//draw(file1,"RMS",151*2,-1E3,15E4,cut);
	draw(file,"RMS",3.1*40,-0.1,3,cut1,length,tolerence,bingo);

}
