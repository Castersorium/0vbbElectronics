#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TF1.h>
#include <iostream>

#define OF
#define STAB
//for amp
#define stab_amp_k -2.0292
#define stab_amp_b 3.9504
//for OF_amp
#define stab_of_k -0.3058
#define stab_of_b 4.0376

int canvas_count = 0;

// 全局变量（添加这些全局变量）
std::vector<double> gMeans;      // 存储所有拟合的均值
std::vector<double> gMeanErrors; // 存储所有拟合的均值误差
std::vector<double> gSigma;      // 存储所有拟合的均值
TH1D* gHist = nullptr;      // 需要改为 TH1D* 以匹配你的程序
std::vector<TF1*> gFits;
TCanvas* gCanvas = nullptr;
TLegend* gLegend = nullptr;

//--------------------------------------------
// 高斯拟合函数
//--------------------------------------------

TF1* GaussFit(TH1D* hist, double xmin, double xmax, int color = kRed) {
    if (!hist) {  // 改为使用传入的 hist 参数
        printf("错误：直方图为空！\n");
        return nullptr;
    }
    
    // 创建唯一的函数名
    TString funcName = Form("gauss_fit_%d", (int)gFits.size());
    
    // 创建高斯函数
    TF1* fitFunc = new TF1(funcName.Data(), "gaus", xmin, xmax);
    fitFunc->SetLineColor(color);
    fitFunc->SetLineWidth(2);
    fitFunc->SetLineStyle(1);
    
    // 设置初始参数（可选，提高拟合稳定性）
    int bin1 = hist->GetXaxis()->FindBin(xmin);
    int bin2 = hist->GetXaxis()->FindBin(xmax);
    
    double maxContent = 0;
    double maxBinCenter = (xmin + xmax) / 2.0;
    
    for (int bin = bin1; bin <= bin2; bin++) {
        double content = hist->GetBinContent(bin);
        if (content > maxContent) {
            maxContent = content;
            maxBinCenter = hist->GetXaxis()->GetBinCenter(bin);
        }
    }
    
    double initialSigma = (xmax - xmin) / 4.0;
    fitFunc->SetParameters(maxContent, maxBinCenter, initialSigma);
    
    // 执行拟合
    hist->Fit(funcName.Data(), "RQ+", "", xmin, xmax);
    
    // 添加到全局列表
    gFits.push_back(fitFunc);
    gMeans.push_back(fitFunc->GetParameter(1));
    gMeanErrors.push_back(fitFunc->GetParError(1));
    gSigma.push_back(fitFunc->GetParameter(2));

    // 创建或更新图例
    if (!gLegend) {
        gLegend = new TLegend(0.7, 0.7, 0.9, 0.9);
    }
    
    TString legEntry = Form("Gaussian (%.1f-%.1f)", xmin, xmax);
    gLegend->AddEntry(fitFunc, legEntry.Data(), "l");
    
    // 确保当前画布存在
    if (gCanvas) {
        gCanvas->cd();
        gLegend->Draw();
        gCanvas->Update();
    }
    
    // 打印拟合结果
    printf("\n高斯拟合结果 [%.1f - %.1f]:\n", xmin, xmax);
    printf("常数 = %.4f ± %.4f\n", fitFunc->GetParameter(0), fitFunc->GetParError(0));
    printf("均值 = %.4f ± %.4f\n", fitFunc->GetParameter(1), fitFunc->GetParError(1));
    printf("Sigma = %.4f ± %.4f\n", fitFunc->GetParameter(2), fitFunc->GetParError(2));
    printf("Chi2/NDF = %.2f\n", fitFunc->GetChisquare()/fitFunc->GetNDF());
    
    return fitFunc;
}

// 高斯+线性背景拟合函数
TF1* GaussLinearFit(TH1D* hist, double xmin, double xmax, int color = kMagenta) {
    if (!hist) {
        printf("错误：直方图为空！\n");
        return nullptr;
    }
    
    // 创建唯一的函数名
    TString funcName = Form("gauss_linear_fit_%d", (int)gFits.size());
    
    // 创建高斯+线性函数：gaus(0) + pol1(3)
    // 参数: [0]常数, [1]均值, [2]sigma, [3]线性截距, [4]线性斜率
    TF1* fitFunc = new TF1(funcName.Data(), "[0]*exp(-0.5*((x-[1])/[2])**2) + [3] + [4]*x", xmin, xmax);
    fitFunc->SetLineColor(color);
    fitFunc->SetLineWidth(2);
    fitFunc->SetLineStyle(2); // 虚线
    
    // 设置参数名
    fitFunc->SetParName(0, "Constant");
    fitFunc->SetParName(1, "Mean");
    fitFunc->SetParName(2, "Sigma");
    fitFunc->SetParName(3, "Intercept");
    fitFunc->SetParName(4, "Slope");
    
    // 设置初始参数
    int bin1 = hist->GetXaxis()->FindBin(xmin);
    int bin2 = hist->GetXaxis()->FindBin(xmax);
    
    double maxContent = 0;
    double maxBinCenter = (xmin + xmax) / 2.0;
    
    for (int bin = bin1; bin <= bin2; bin++) {
        double content = hist->GetBinContent(bin);
        if (content > maxContent) {
            maxContent = content;
            maxBinCenter = hist->GetXaxis()->GetBinCenter(bin);
        }
    }
    
    double initialSigma = (xmax - xmin) / 4.0;
    
    // 估计线性背景：使用区间两端的平均值
    double leftEdge = (hist->GetBinContent(bin1) + hist->GetBinContent(bin1+1)) / 2.0;
    double rightEdge = (hist->GetBinContent(bin2) + hist->GetBinContent(bin2-1)) / 2.0;
    double initialIntercept = leftEdge;
    double initialSlope = (rightEdge - leftEdge) / (xmax - xmin);
    
    fitFunc->SetParameters(maxContent, maxBinCenter, initialSigma, initialIntercept, initialSlope);
    
    // 可以设置参数限制（可选）
    fitFunc->SetParLimits(0, 0, maxContent * 1.5); // 高斯常数必须为正
    fitFunc->SetParLimits(2, 0.0001, (xmax - xmin) / 2.0); // sigma 限制
    
    // 执行拟合
    hist->Fit(funcName.Data(), "RQ+", "", xmin, xmax);
    

    // 添加到全局列表
    gMeans.push_back(fitFunc->GetParameter(1));
    gMeanErrors.push_back(fitFunc->GetParError(1));
    gSigma.push_back(fitFunc->GetParameter(2));

    // 添加到全局列表
    gFits.push_back(fitFunc);
    
    // 创建或更新图例
    if (!gLegend) {
        gLegend = new TLegend(0.7, 0.7, 0.9, 0.9);
    }
    
    TString legEntry = Form("Gauss+Linear (%.1f-%.1f)", xmin, xmax);
    gLegend->AddEntry(fitFunc, legEntry.Data(), "l");
    
    // 确保当前画布存在
    if (gCanvas) {
        gCanvas->cd();
        gLegend->Draw();
        gCanvas->Update();
    }
    
    // 打印拟合结果
    printf("\n高斯+线性拟合结果 [%.1f - %.1f]:\n", xmin, xmax);
    printf("高斯常数 = %.4f ± %.4f\n", fitFunc->GetParameter(0), fitFunc->GetParError(0));
    printf("均值 = %.4f ± %.4f\n", fitFunc->GetParameter(1), fitFunc->GetParError(1));
    printf("Sigma = %.4f ± %.4f\n", fitFunc->GetParameter(2), fitFunc->GetParError(2));
    printf("线性截距 = %.4f ± %.4f\n", fitFunc->GetParameter(3), fitFunc->GetParError(3));
    printf("线性斜率 = %.4f ± %.4f\n", fitFunc->GetParameter(4), fitFunc->GetParError(4));
    printf("Chi2/NDF = %.2f\n", fitFunc->GetChisquare()/fitFunc->GetNDF());
    
    return fitFunc;
}


//--------------------------------------------
// 主绘图函数
//--------------------------------------------
void draw(const char *rootfile, TString fillname, int nbin, double nmin, double nmax, const char *cut) {

    TString name = fillname;

    // 打开 ROOT 文件
    TFile *file = new TFile(rootfile, "READ");
    if(!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << rootfile << std::endl;
        return;
    }

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

    // 定义变量
    double RMS, baseline, offset, slope, amplitude, maxtime, risetime, decaytime;
    double starttime1, stoptime1, starttime2, stoptime2;
    int numberoftriggers;
    bool issignal;
    double OF_amplitude, OF_Chi2;
    TString fillname2;

    // 设置分支地址
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

    // 设置朋友树
    tree1->AddFriend(tree0);
    tree1->AddFriend(tree2);
    tree1->AddFriend(tree3);
    tree1->AddFriend(tree4, "friend4");
    tree1->AddFriend(tree5, "friend5");
    tree1->AddFriend(tree6);
#ifdef OF
    tree1->AddFriend(tree7, "friend7");
    tree1->AddFriend(tree8, "friend8");
#endif

    // 设置别名
    tree1->SetAlias("starttime1","friend4.starttime");
    tree1->SetAlias("stoptime1","friend4.stoptime");
    tree1->SetAlias("starttime2","friend5.starttime");
    tree1->SetAlias("stoptime2","friend5.stoptime");
#ifdef OF
    tree1->SetAlias("OF_amplitude","friend7.amplitude");
    tree1->SetAlias("OF_Chi2","friend8.Chi2");
#endif

#ifdef STAB 
    if(name == "OF_amplitude")
            fillname2 = Form("(20 * OF_amplitude / ((baseline - %f)/%f) )", stab_of_b, stab_of_k);
    else if(name == "amplitude")
    fillname2 = Form("(3 * amplitude / ((baseline - %f)/%f) )", stab_amp_b, stab_amp_k);
#endif

    // 创建直方图
    TH1D *h_stab = new TH1D("h_stab", "", nbin, nmin, nmax);
    TCut mycut(cut);
    tree1->Draw(Form("%s>>h_stab", fillname2.Data()), mycut.GetTitle());
    // 绘制
    TString canvas_name = TString::Format("canvas_%d", canvas_count++);
    TCanvas *c = new TCanvas(canvas_name,"",1200,600);
    h_stab->SetLineColor(kBlue);
    h_stab->SetStats(0);
    h_stab->Draw();
    h_stab->SetXTitle(Form("%s", fillname.Data()));
    h_stab->SetYTitle("Events");
    // c->SetLogy();
    c->SaveAs(Form("rootplot/Cut%d_%s.png", canvas_count, name.Data()));
    
    // 保存当前画布指针到全局变量
    gCanvas = c;
}

//--------------------------------------------
// 快速浏览示例
//--------------------------------------------
void Quicklook_calibration() {
    gROOT->SetStyle("Modern");

    const char *file1 = "./Processed_20251121T122223_000033_1.root";
    int nbin = 20*100;
    double min = -0.1;
    double max = 6000;

    // const char *cut = "issignal == 1 && numberoftriggers == 1 && slope > -0.004 && slope < 0.004 && RMS < 0.002 && risetime > 0.0005 && risetime < 0.007 && decaytime > 0.0005 && decaytime < 0.012";
    const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 ";
    // const char *cut1 = "issignal == 1 && numberoftriggers == 1  && slope > -0.004 && slope < 0.004 && RMS < 0.002 && starttime1>0.295 && starttime1<0.305";

    // draw(file1,"amplitude",10*1000,-0.1,5,cut1);
    draw(file1,"OF_amplitude",1500,-0.1,30,cut1);


    // 如果想进行高斯拟合，可以单独调用 GaussFit，比如：
    TH1D *h = (TH1D*)gDirectory->Get("h_stab");
        // GaussFit(h, 19.8, 20.1); // 对 h_cut 在 2.96~3.04 区间拟合高斯
        // 可以多次调用
        // GaussLinearFit(h, 0.93, 1.0, kRed);
        GaussLinearFit(h, 2.0, 2.15, kRed);
        GaussLinearFit(h, 2.35, 2.4,kRed); // 第三个拟合
        // GaussLinearFit(h, 3.6, 3.8,kRed); // 对 h_cut 在 2.96~3.04 区间拟合高斯
        // GaussFit(h, 3.65, 3.8,kRed); // 对 h_cut 在 2.96~3.04 区间拟合高斯
        GaussFit(h, 6.3, 6.5,kRed); // 对 h_cut 在 2.96~3.04 区间拟合高斯
        GaussFit(h, 10.2, 10.4,kRed); // 对 h_cut 在 2.96~3.04 区间拟合高斯
        // GaussLinearFit(h, 0.305, 0.315, kRed);
        // GaussLinearFit(h, 0.35, 0.36,kRed); // 第三个拟合
        // GaussLinearFit(h, 0.55,0.57,kRed); // 对 h_cut 在 2.96~3.04 区间拟合高斯
        // GaussLinearFit(h, 0.96, 0.98,kRed); // 对 h_cut 在 2.96~3.04 区间拟合高斯
        // GaussLinearFit(h, 1.53, 1.58,kRed); // 对 h_cut 在 2.96~3.04 区间拟合高斯

    // 自动打印所有结果汇总
    printf("\n=== 所有拟合结果汇总 ===\n");
    for (size_t i = 0; i < gMeans.size(); i++) {
        printf("峰值 %zu: 均值 = %.4f ± %.4f\n", i+1, gMeans[i], gMeanErrors[i]);
    }


        // 进行能量校准（使用已知能量）
    if (gMeans.size() >= 2) {
        // 已知能量值（根据实际情况修改）
        double energies[] = { 510.7, 583.2, 1620,2614.5}; // keV
        // double energies[] = {238.6, 510.7, 583.2, 911.2,2614.5}; // keV
        
        // 确定使用的数据点数量
        int nPoints = gMeans.size() ;

        TCanvas* c2 = new TCanvas("c2", "calibration", 800, 600);
        c2->SetGrid();
        
        TGraphErrors* g = new TGraphErrors(nPoints);
        for (int i = 0; i < nPoints; i++) {
            double x = gMeans[i];
            double y = energies[i];
            double ex = gMeanErrors[i];
                        
            g->SetPoint(i, x, y);
            g->SetPointError(i, ex, 0.0);

        }
        
        
        g->SetTitle("Calibration Fit;Amplitude;Energy (keV)");
        g->SetMarkerStyle(20);
        g->SetMarkerSize(1.2);
        g->SetMarkerColor(kBlue);
        g->Draw("AP");
        
        // 获取数据范围用于设置拟合范围
        double minX = gMeans[0];
        double maxX = gMeans[0];
        for (int i = 1; i < nPoints; i++) {
            if (gMeans[i] < minX) minX = gMeans[i];
            if (gMeans[i] > maxX) maxX = gMeans[i];
        }
        
        // 扩展拟合范围
        double rangeMin = minX - 0.5;
        double rangeMax = maxX + 0.5;
        printf("拟合范围: %.2f 到 %.2f\n", rangeMin, rangeMax);
        
        // TF1* calib = new TF1("calib", "[0] + [1]*x", rangeMin, rangeMax);
        TF1* calib = new TF1("calib", "[0]*x + [1]*x*x", rangeMin, rangeMax);
        double estimatedSlope = (energies[nPoints-1] - energies[0]) / (gMeans[nPoints-1] - gMeans[0]);
        double estimatedIntercept = energies[0] - estimatedSlope * gMeans[0];

        printf("估计初始参数: 截距=%.2f, 斜率=%.2f\n", estimatedIntercept, estimatedSlope);
        // calib->SetParameters(estimatedIntercept, estimatedSlope);
        calib->SetParameters(estimatedSlope, 0);
        
        printf("开始拟合...\n");
        TFitResultPtr fitResult = g->Fit("calib", "RSQ");

        TLatex latex;
		latex.SetNDC();
		latex.SetTextSize(0.04);
		latex.SetTextColor(kRed);
		// latex.DrawLatex(0.15, 0.85, Form("y = %.4f x + %.4f", calib->GetParameter(1), calib->GetParameter(0)));
		latex.DrawLatex(0.15, 0.85, Form("y = %.4f x^2 + %.4f * x ", calib->GetParameter(1), calib->GetParameter(0)));

        printf("拟合状态: %d (0表示成功)\n", fitResult->Status());
        printf("拟合参数协方差矩阵状态: %d\n", fitResult->CovMatrixStatus());

        double calib_k = calib->GetParameter(1);
        double calib_b = calib->GetParameter(0);
        for (int i = 0; i < nPoints; i++) {
            double x = gMeans[i];
            double y = energies[i];   
            double sigma = gSigma[i];
            double R = 2.355 * (sigma * calib_k ) / y * 100; // 能量分辨率百分比
            double FWHM = 2.355 * sigma * calib_k;
            printf("energy point %f: Sigma = %.4f , FWHM = %.4f ,R = %.4f\n", y, sigma, FWHM,R);
        }
        
        c2->Update();
        c2->SaveAs("rootplot/calibration.png");
        
        printf("\n校准图已保存为 calibration.png\n");
    }



}