#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <vector>

#define OF
int canvas_count = 0;

// =====================================================
//    辅助函数：绘制 Scatter 图，可选拟合
// =====================================================
// void DrawScatter(
//     TTree* tree,
//     TString xvar, TString yvar,
//     const char* cut,
//     bool doFit = true,                // 是否进行直线拟合
//     double fitMin = 2.9, double fitMax = 3.0
// ){
//     // 准备变量
//     std::vector<double> xs, ys;

//     // 利用 Draw 获取数据
//     tree->Draw(Form("%s:%s", yvar.Data(), xvar.Data()), cut, "goff");
//     int n = tree->GetSelectedRows();

//     double *vx = tree->GetV2(); 
//     double *vy = tree->GetV1();

//     xs.assign(vx, vx + n);
//     ys.assign(vy, vy + n);

//     // 创建 Scatter graph
//     TGraph *gr = new TGraph(n, xs.data(), ys.data());
//     gr->SetTitle(Form("%s vs %s", xvar.Data(), yvar.Data()));
//     gr->SetMarkerStyle(20);
//     gr->SetMarkerSize(0.5);
//     gr->SetMarkerColor(kBlue);

//     // Canvas
//     TCanvas *c1 = new TCanvas(
//         Form("c_scatter_%d", canvas_count), "", 800, 600);
//     canvas_count++;

//     gr->GetXaxis()->SetTitle(xvar);
//     gr->GetYaxis()->SetTitle(yvar);
//     gr->Draw("AP");

//     // ==================================
//     //       如果需要拟合，就画拟合线
//     // ==================================
//     if (doFit)
//     {
//         TF1 *fitfunc = new TF1("fitfunc", "[0] + [1]*x", fitMin, fitMax);
//         fitfunc->SetLineColor(kRed);
//         fitfunc->SetLineWidth(2);
        
//         fitfunc->SetParameters(-2,0);

//         gr->Fit(fitfunc, "R");
// 		double p0 = fitfunc->GetParameter(0);
//         double p1 = fitfunc->GetParameter(1);
// 		TLatex latex;
// 		latex.SetNDC();
// 		latex.SetTextSize(0.04);
// 		latex.SetTextColor(kRed);
// 		latex.DrawLatex(0.15, 0.85, Form("y = %.4f x + %.4f", p1, p0));
        
// 		c1->SaveAs(Form("rootplot/Scatter_%svs%s_fit.png", 
//                         xvar.Data(), yvar.Data()));

//         // ============ 变换后的散点图 =============
//         std::vector<double> xs2, ys2;
//         xs2.reserve(n);
//         ys2.reserve(n);

//         for (int i = 0; i < n; i++) {
//             double x = xs[i];
//             double y = ys[i];

//             double denom = (y - p0) / p1;
//             if (fabs(denom) < 1e-9) continue;

//             xs2.push_back(20 * x / denom);
//             ys2.push_back(y);
//         }

//         TGraph *gr2 = new TGraph(xs2.size(), xs2.data(), ys2.data());
//         gr2->SetMarkerStyle(20);
//         gr2->SetMarkerSize(0.5);
//         gr2->SetMarkerColor(kRed);
//         gr2->SetTitle("Rescaled Scatter Plot");

//         TCanvas *c2 = new TCanvas(
//             Form("c_trans_%d", canvas_count), "", 800, 600);
//         canvas_count++;

//         gr2->GetXaxis()->SetTitle("x' = scale * x / ((y - p0)/p1)");
//         gr2->GetYaxis()->SetTitle("y' = y");
//         gr2->Draw("AP");

//         c2->SaveAs("rootplot/Scatter_transformed.png");
//     }
//     else
//     {
//         c1->SaveAs(Form("rootplot/Scatter_%svs%s.png", 
//                         xvar.Data(), yvar.Data()));
//     }
// }

void DrawScatter(
    TTree* tree,
    TString xvar, TString yvar,
    const char* cut,
    bool doFit = true,
    double fitMin = 2.9, double fitMax = 3.0,
    double yFitMin = -1e9, double yFitMax = 1e9
){
    if (!tree) {
        Error("DrawScatter", "Null TTree pointer!");
        return;
    }

    // ===============================
    //        读取数据
    // ===============================
    std::vector<double> xs, ys;

    tree->Draw(Form("%s:%s", yvar.Data(), xvar.Data()),
               cut, "goff");
    int n = tree->GetSelectedRows();
    if (n <= 0) {
        Warning("DrawScatter", "No entries selected!");
        return;
    }

    double *vx = tree->GetV2(); // x
    double *vy = tree->GetV1(); // y

    xs.assign(vx, vx + n);
    ys.assign(vy, vy + n);

    // ===============================
    //        原始散点图
    // ===============================
    TGraph *gr = new TGraph(n, xs.data(), ys.data());
    gr->SetTitle(Form("%s vs %s", xvar.Data(), yvar.Data()));
    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(0.5);
    gr->SetMarkerColor(kBlue);

    TCanvas *c1 = new TCanvas(
        Form("c_scatter_%d", canvas_count), "", 800, 600);
    canvas_count++;

    gr->GetXaxis()->SetTitle(xvar);
    gr->GetYaxis()->SetTitle(yvar);
    gr->Draw("AP");

    // ===============================
    //            拟合
    // ===============================
    if (doFit)
    {
        // -------- 仅用于拟合的 y-cut --------
        std::vector<double> xs_fit, ys_fit;
        xs_fit.reserve(n);
        ys_fit.reserve(n);

        for (int i = 0; i < n; i++) {
            if (ys[i] >= yFitMin && ys[i] <= yFitMax) {
                xs_fit.push_back(xs[i]); // x 不限制
                ys_fit.push_back(ys[i]);
            }
        }

        if (xs_fit.size() < 2) {
            Warning("DrawScatter",
                    "Not enough points in y-fit range!");
            c1->SaveAs(Form("rootplot/Scatter_%svs%s.png",
                            xvar.Data(), yvar.Data()));
            return;
        }

        TGraph *gr_fit = new TGraph(xs_fit.size(),
                                    xs_fit.data(),
                                    ys_fit.data());

        // -------- 拟合函数 --------
        TF1 *fitfunc = new TF1("fitfunc",
                               "[0] + [1]*x",fitMin,fitMax);
        fitfunc->SetLineColor(kRed);
        fitfunc->SetLineWidth(2);
        fitfunc->SetParameters(-2, 0);

        // ★ 用 y-cut 后的点拟合
        gr_fit->Fit(fitfunc, "R");
        // gr_fit->SetMarkerStyle(21);
        // gr_fit->SetMarkerSize(0.8);
        // gr_fit->SetMarkerColor(kRed);
        // gr_fit->Draw("P SAME");
        fitfunc->Draw("SAME");

        double p0 = fitfunc->GetParameter(0);
        double p1 = fitfunc->GetParameter(1);

        // -------- 拟合公式显示 --------
        TLatex latex;
        latex.SetNDC();
        latex.SetTextSize(0.04);
        latex.SetTextColor(kRed);
        latex.DrawLatex(0.15, 0.85,
            Form("y = %.4f x + %.4f", p1, p0));

        c1->SaveAs(Form("rootplot/Scatter_%svs%s_fit.png",
                        xvar.Data(), yvar.Data()));

        // ===============================
        //        变换后的散点图
        // ===============================
        std::vector<double> xs2, ys2;
        xs2.reserve(n);
        ys2.reserve(n);

        for (int i = 0; i < n; i++) {
            double x = xs[i];
            double y = ys[i];

            double denom = x*p1 + p0;
            if (std::fabs(denom) < 1e-9) continue;

            xs2.push_back(x);
            ys2.push_back(8 * y / denom);
        }

        TGraph *gr2 = new TGraph(xs2.size(),
                                 xs2.data(),
                                 ys2.data());
        gr2->SetMarkerStyle(20);
        gr2->SetMarkerSize(0.5);
        gr2->SetMarkerColor(kRed);
        gr2->SetTitle("Rescaled Scatter Plot");

        TCanvas *c2 = new TCanvas(
            Form("c_trans_%d", canvas_count), "", 800, 600);
        canvas_count++;

        gr2->GetXaxis()->SetTitle(
            "x' = 20 * x / ((y - p0)/p1)");
        gr2->GetYaxis()->SetTitle("y");
        gr2->Draw("AP");

        c2->SaveAs("rootplot/Scatter_transformed.png");
    }
    else
    {
        c1->SaveAs(Form("rootplot/Scatter_%svs%s.png",
                        xvar.Data(), yvar.Data()));
    }
}



// =====================================================
//                   主绘图函数
// =====================================================
void draw(const char *rootfile,
          TString fillname1, TString fillname2,
          int nbin, double nmin, double nmax,
          int nbin2, double nmin2, double nmax2,
          const char *cut,
          bool drawFit = true     // ⭐ 可选是否拟合
){
    // 打开 ROOT 文件
    TFile *file = new TFile(rootfile, "READ");

    // 获取所有 Tree
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

    // 加 Friend 结构
    tree1->AddFriend(tree0);
    tree1->AddFriend(tree2);
    tree1->AddFriend(tree3);
    tree1->AddFriend(tree4, "friend4");
    tree1->AddFriend(tree5, "friend5");
    tree1->AddFriend(tree6);
#ifdef OF
    tree1->AddFriend(tree7, "friend7");
#endif

    // alias
    tree1->SetAlias("starttime1", "friend4.starttime");
    tree1->SetAlias("stoptime1", "friend4.stoptime");
    tree1->SetAlias("starttime2", "friend5.starttime");
    tree1->SetAlias("stoptime2", "friend5.stoptime");
#ifdef OF
    tree1->SetAlias("OF_amplitude", "friend7.amplitude");
    tree1->SetAlias("OF_Chi2", "friend7.Chi2");
#endif

    // =============================
    //         绘制散点图
    // =============================
    // DrawScatter(tree1, fillname1, fillname2, cut, drawFit, 19.5, 20.1);
    DrawScatter(tree1, fillname1, fillname2, cut, drawFit, -9,-4,3,7);
    // DrawScatter(tree1, fillname1, fillname2, cut, drawFit, -2.15,-1.9,2.9,3);
}



// =====================================================
//                     Quicklook2
// =====================================================
void Quicklook_scatter()
{
    gStyle->SetOptStat(0);

    // const char *file1="./Processed_20251121T122223_000033_1.root";
    const char *file1="/mnt/c/Users/jiaow/OneDrive/Desktop/ccvr3/2510/test/cs.root";
    // const char *file1="./Processed_20251121T122223_000033_1.root";
    const char *cut1 = "issignal == 1 && numberoftriggers == 1 && slope>-0.004 && slope<0.004 && RMS<0.002";

    draw(
        file1,
        "baseline", "amplitude",
        10000,-10,-3,
        10000,-1,10,
        // "OF_amplitude", "baseline",
        // 1000, 0.1, 50,
        // 200, -10, 10,
        cut1,
        true              // ⭐ 控制是否拟合（可改 false）
    );
}
