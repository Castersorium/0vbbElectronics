#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TPad.h>

void test() {
    // 创建直方图
    TH1F* h1 = new TH1F("h1", "Histogram h1", 50, 0, 10);
    TH1F* h2 = new TH1F("h2", "Histogram h2", 50, 0, 10);

    // 填充数据
    for (int i = 0; i < 1000; i++) {
        h1->Fill(gRandom->Gaus(5, 1));
        h2->Fill(gRandom->Gaus(5, 1.5));
    }

    // 创建差值直方图
    TH1F* h_diff = (TH1F*)h1->Clone("h_diff");
    h_diff->Add(h2, -1);

    // 创建画布
    TCanvas* canvas = new TCanvas("canvas", "Two Pane Histogram", 800, 600);

    // 创建上下两个子画布
    TPad* topPad = new TPad("topPad", "Top Pad", 0, 0.3, 1, 1);
    TPad* bottomPad = new TPad("bottomPad", "Bottom Pad", 0, 0, 1, 0.3);

    // 调整边距
    topPad->SetBottomMargin(0.02);   // 上图底部边距较小
    bottomPad->SetTopMargin(0.02);  // 下图顶部边距较小
    bottomPad->SetBottomMargin(0.3); // 下图底部边距较大（显示 x 轴标题）

    // 激活两个子画布
    topPad->Draw();
    bottomPad->Draw();

    // 在上图绘制 h1 和 h2
    topPad->cd();
    h1->SetLineColor(kBlue);
    h2->SetLineColor(kRed);
    h1->Draw("HIST");
    h2->Draw("HISTSAME");

    // 添加图例
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(h1, "h1", "l");
    legend->AddEntry(h2, "h2", "l");
    legend->Draw();

    // 在下图绘制 h_diff
    bottomPad->cd();
    h_diff->SetLineColor(kBlack);
    h_diff->SetTitle("");  // 不需要标题
    h_diff->GetXaxis()->SetTitle("X-axis");  // 设置 x 轴标题
    h_diff->GetYaxis()->SetTitle("");        // 不需要单独 y 轴标题
    h_diff->GetXaxis()->SetLabelSize(0.1);   // 调整 x 轴标签大小
    h_diff->GetXaxis()->SetTitleSize(0.12);  // 调整 x 轴标题大小
    h_diff->GetYaxis()->SetLabelSize(0.1);   // 调整 y 轴标签大小
    h_diff->Draw("HIST");

    // 添加共享 y 轴标题
    canvas->cd();
    TLatex* ylabel = new TLatex();
    ylabel->SetTextSize(0.04);
    ylabel->SetTextAngle(90);
    ylabel->DrawLatex(0.04, 0.5, "Counts");
}

