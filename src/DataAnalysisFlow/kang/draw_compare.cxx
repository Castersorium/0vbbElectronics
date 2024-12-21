#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>

void plot(const char* file1, const char* file2, const char* tree, const char* branch, const char* branch2 , int nbin, double min, double max) {
   
    TFile *f1 = TFile::Open(file1);
    TFile *f2 = TFile::Open(file2);

    // 从文件中获取树
    TTree *tree1 = (TTree*)f1->Get(Form("%s",tree));
    TTree *tree2 = (TTree*)f2->Get(Form("%s",tree));

    // 创建画布
    TCanvas *canvas = new TCanvas("canvas", "Comparison of Branches", 2000, 1200);
    //canvas->Divide(1, 2); // 分为 2x2 画布

    // 绘制第一个文件的 branch1
    canvas->cd(1);
    TH1F *hist1 = new TH1F("hist1", "Branch from File 1", nbin, min, max); // 设置合适的 bin 数量和范围
    tree1->Draw(Form("%s >> hist1", branch));
    hist1->SetLineColor(kRed);
    hist1->SetLineWidth(2);     // 设置线宽
    //hist1->SetTitle(Form("Histogram of %s from File 1", branch));
    //canvas->SaveAs("hist1.pdf");

    // 绘制第二个文件的 branch2
    TH1F *hist2 = new TH1F("hist2", "Branch from File 2", nbin, min, max); // 设置合适的 bin 数量和范围
    tree2->Draw(Form("%s >> hist2", branch2));
    //tree2->Draw("Amp_filtered>>hist2");
    hist2->SetLineColor(kBlack);
    hist2->SetLineWidth(2);     // 设置线宽
    //hist2->SetTitle(Form("Histogram of %s from File 2", branch));
    //canvas->SaveAs("hist2.pdf");

    // 关闭直方图的标题和统计框
    hist1->SetTitle("");
    hist2->SetTitle("");
    gStyle->SetOptStat(0);

    double scale1=51.*60+12;
    double scale2=33.25*60;
    //double scale2=14.25*60;
    double inte1 = hist1->Integral(); // 获取总积分
    double inte2 = hist2->Integral(); // 获取总积分
    double max1 = hist1->GetMaximum();
    double max2 = hist2->GetMaximum();
    //hist1->Scale(max2/max1);    // 缩放因子 = 1 / 积分
    //hist2->Scale(24*60/scale2);    // 缩放因子 = 1 / 积分

    // 确定最大值并设置Y轴范围
    //double max1 = hist1->GetMaximum();
    //double max2 = hist2->GetMaximum();
    double maxVal = (max1 > max2) ? max1 : max2;
    //hist1->SetMaximum(1.3 * maxVal); // 将Y轴最大值设置为更大的直方图的1.1倍，以确保显示
    hist1->SetMaximum(100); // 将Y轴最大值设置为更大的直方图的1.1倍，以确保显示

    // 创建差值直方图
    TH1F* hist_diff = (TH1F*)hist1->Clone("hist_diff");
    //hist_diff->SetTitle("Histogram Difference (hist1 - hist2)");
    hist_diff->SetLineColor(kBlue);  // 设置颜色
    hist_diff->SetLineWidth(1);     // 设置线宽
    hist_diff->SetLineStyle(3);     // 设置线宽
    hist_diff->SetFillStyle(3013);       // 设置填充样式为斜线网格
    hist_diff->SetFillColor(kBlue);      // 设置填充颜色为蓝色
    // 计算差值 hist_diff = hist1 - hist2
    hist_diff->Add(hist2, -1); // 第二个参数为 -1 表示减去 hist2
    //hist_diff->SetMaximum(2. * maxVal); // 将Y轴最大值设置为更大的直方图的1.1倍，以确保显示
    //hist_diff->GetYaxis()->SetRangeUser(0, 2.*maxVal); // 根据归一化后 bin 的值设置合适的范围
	
    // 创建画布
    //TCanvas *canvas = new TCanvas("canvas", "Comparison of Branches", 1600, 600);
    //canvas->Divide(2, 1); // 分为 2x2 画布
    // 创建上下两个子画布
    TPad* topPad = new TPad("topPad", "Top Pad", 0, 0.3, 1, 1);
    TPad* bottomPad = new TPad("bottomPad", "Bottom Pad", 0, 0, 1, 0.3);
    // 调整边距
    topPad->SetBottomMargin(0.0);   // 上图底部边距较小
    bottomPad->SetTopMargin(0.0);  // 下图顶部边距较小
    bottomPad->SetBottomMargin(0.3); // 下图底部边距较大（显示 x 轴标题）
    topPad->Draw();
    bottomPad->Draw();
 
    // 绘制两个分支的叠加图
    topPad->cd();
    //canvas->SetLogy();
    //topPad->SetLogy();
    //hist1->GetXaxis()->SetTitle(Form("Histogram of %s ", branch));
    hist1->GetXaxis()->SetLabelSize(0);   // 调整 x 轴标签大小
    hist1->GetYaxis()->SetTitle("Counts/day");
    //hist1->GetYaxis()->SetLabelSize(0.1);   // 调整 x 轴标签大小
    hist1->GetYaxis()->SetTitleSize(0.06);  // 调整 x 轴标题大小
    hist1->GetYaxis()->SetTitleOffset(0.5);  // 调整 x 轴标题大小
    //hist1->GetYaxis()->SetNdivisions(511);  // 调整 x 轴标题大小
    //hist1->GetYaxis()->SetMoreLogLabels(false); // 禁用额外的 log 标签（如 2×10^1 等）    
    //hist1->GetYaxis()->SetNoExponent(false); // 显示指数格式（如 10^1 而不是 10）    
    hist1->Draw("HIST"); // 先绘制 hist1
    hist2->Draw("HISTSAME"); // 叠加绘制 hist2
    //hist_diff->Draw("HISTSAME"); // 叠加绘制
    
    // 添加图例
    TLegend *legend = new TLegend(0.7, 0.75, 0.9, 0.9);
    //legend->AddEntry(hist1, file1, "l");
    //legend->AddEntry(hist2, file2, "l");
    legend->AddEntry(hist1, "Trigger1", "l");
    legend->AddEntry(hist2, "Trigger2", "l");
    legend->AddEntry(hist_diff, "Diff", "l");
    legend->Draw();
    

    bottomPad->cd();
    //canvas->SetLogy();
    //bottomPad->SetLogy();
    //hist_diff->SetMaximum(1.3 * maxVal); // 将Y轴最大值设置为更大的直方图的1.1倍，以确保显示
    //hist_diff->GetYaxis()->SetRangeUser(0, 2.*maxVal); // 根据归一化后 bin 的值设置合适的范围
    hist_diff->GetXaxis()->SetTitle(Form("Histogram of %s ", branch));
    hist_diff->GetXaxis()->SetLabelSize(0.1);   // 调整 x 轴标签大小
    hist_diff->GetXaxis()->SetTitleSize(0.12);  // 调整 x 轴标题大小
    //hist_diff->GetYaxis()->SetTitle("Counts/day");
    hist_diff->Draw("HIST"); // 叠加绘制
    //TLegend *legend2 = new TLegend(0.7, 0.8, 0.9, 0.9);
    //legend2->AddEntry(hist_diff, "Diff", "l");
    //legend2->Draw();

    //canvas->SetLogy();
    //canvas->SaveAs(Form("%s_%s.png",file1,branch));
    canvas->SaveAs(Form("Compare_%s.png",branch));
    // 更新画布
    canvas->Update();
}

void draw_compare(){
	//gROOT->SetStyle("Pub");
	gROOT->SetStyle("Modern");
	plot("Heater/data/light.root", "Heater/data/TriggerEvent.root", "tree1", "rawamp", "Amp_raw", 200,0,1.2E6);   
	plot("Heater/data/light.root", "Heater/data/TriggerEvent.root", "tree1", "filamp", "Amp_filtered", 200,1E5,1.2E6);   
	//plot("test/LD.root", "test/LD_BKG.root", "tree1", "filamp",200,0,.2E6);   
	//plot("/mnt/e/RUNs_data_analysis/RUN2408/Converted_Data/root/1cmLMO_SB12_T6at9p46mK_bkg_4h40min_5kHz_2408200106/light.root", "/mnt/e/RUNs_data_analysis/RUN2408/Converted_Data/root/1cmLMO_SB12_T6at9p46mK_bkg_4h40min_5kHz_2408200106/TriggerEvent.root", "tree1", "filamp",200,0,.2E5);   
}
