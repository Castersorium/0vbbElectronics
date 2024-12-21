import ROOT
import math
import sys
import numpy as np
import matplotlib.pyplot as plt

def main(input_file, tree_name, branch_name):
    # 打开 ROOT 文件并获取 tree1
    file = ROOT.TFile.Open(input_file)  # 使用传入的文件路径
    #tree = file.Get("tree1")
    tree = file.Get(f"{tree_name}")

    # 创建直方图并绘制 branch_name 的分布
    #hist = ROOT.TH1F("hist", "Filamp Distribution", 100, 0, 1.2e6)
    hist = ROOT.TH1F("hist", "Filamp Distribution", 100, 0, 0.5)
    #tree.Draw("branch_name >> hist")
    tree.Draw(f"{branch_name} >> hist")

    # 创建一个画布绘制直方图
    c1 = ROOT.TCanvas("c1", "Filamp Histogram", 800, 600)
    hist.GetXaxis().SetTitle(branch_name)
    hist.GetYaxis().SetTitle("Counts")
    hist.Draw()
    c1.SaveAs(f"cupid-china/{branch_name}_histogram.png")

    # 区间列表
    ranges = [
        (70E-3, 110E-3),
        (110E-3, 160E-3),
        (170E-3, 210E-3),
        (230E-3, 270E-3),
        (280E-3, 330E-3)
    ]

    # 计算网格大小（如 3x2 或 3x3）
    num_ranges = len(ranges)
    cols = math.ceil(math.sqrt(num_ranges))  # 列数为向上取整的平方根
    rows = math.ceil(num_ranges / cols)  # 行数为总数除以列数向上取整

    # 创建网格画布
    c2 = ROOT.TCanvas("c2", "Gaussian Fits", 1200, 800)  # 调整画布大小适配多图
    c2.Divide(cols, rows)  # 按行列分割画布

    # 创建一个 TList 来存储所有的直方图
    hist_list = ROOT.TList()

    # 存储每个区间的均值
    means = []

    # 遍历区间并拟合高斯
    for i, (low, high) in enumerate(ranges):
        # 切换到子画布
        c2.cd(i + 1)

        # 在区间内筛选数据
        cut = f"{branch_name} > {low} && {branch_name} < {high}"
        
        # 创建唯一名称的直方图
        temp_hist = ROOT.TH1F(f"temp_hist_{low}_{high}", f"Filamp {low}-{high}", 50, low, high)
        
        # 使用 `tree.Draw()` 绘制该区间的直方图
        tree.Draw(f"{branch_name} >> temp_hist_{low}_{high}", cut)

        # 将该直方图加入到 TList 中
        hist_list.Add(temp_hist)

        # 设置直方图的样式
        temp_hist.GetXaxis().SetTitle(branch_name)
        temp_hist.GetYaxis().SetTitle("Counts")

        # 绘制直方图
        temp_hist.Draw("HIST")  # 确保以 HIST 方式绘制直方图

        # 高斯拟合
        gaussian = ROOT.TF1(f"gaus_{low}_{high}", "gaus", low, high)
        temp_hist.Fit(gaussian, "R")

        # 获取高斯拟合的均值（mean），并存储
        mean = gaussian.GetParameter(1)
        #mean = (mean * 20)/(2**24)
        means.append(mean)

        # 设置高斯拟合线的颜色并绘制
        gaussian.SetLineColor(ROOT.kRed)
        gaussian.Draw("SAME")

        # 强制更新画布
        c2.Update()

    # 保存最终画布
    c2.SaveAs(f"cupid-china/{branch_name}_gaussian_fits_grid.png")

    # 自定义的 x 数组
    vol = [50,60,70,80,90]

    # 创建一个 TGraph 来存储自定义的均值数据
    n_points = len(vol)  # 根据自定义 x 数组的长度来设定点数
    graph = ROOT.TGraph(n_points)

    # 将均值添加到 TGraph
    for i in range(n_points):
        #graph.SetPoint(i, i + 1, means[i])
        graph.SetPoint(i, vol[i], means[i])  # x 使用 custom_x[i], y 使用 means[i]

    # 绘制均值图
    c3 = ROOT.TCanvas("c3", "Amplitude vs Voltage", 800, 600)
    graph.SetTitle("Amplitude vs Voltage;Voltage;Amplitude")
    graph.SetMarkerStyle(21)
    graph.SetMarkerColor(ROOT.kBlue)
    graph.Draw("AP")

    # 对均值图进行线性拟合
    #linear_fit = ROOT.TF1("linear_fit", "pol1", vol[0], vol[-1])
    #graph.Fit(linear_fit, "R")

    # 绘制拟合线
    #linear_fit.SetLineColor(ROOT.kRed)
    #linear_fit.Draw("SAME")

  # 对均值图进行二次多项式拟合
    quadratic_fit = ROOT.TF1("quadratic_fit", "pol2", vol[0], vol[-1])
    graph.Fit(quadratic_fit, "R")

    # 绘制二次拟合线
    quadratic_fit.SetLineColor(ROOT.kGreen)
    quadratic_fit.Draw("SAME")
    #quadratic_fit.Draw("R")

    # 添加图例
    legend = ROOT.TLegend(0.1, 0.7, 0.3, 0.9)
    #legend.AddEntry(linear_fit, "Linear Fit", "l")
    legend.AddEntry(quadratic_fit, "Quadratic Fit", "l")
    legend.Draw()

    # 保存均值图及其拟合
    c3.SaveAs(f"cupid-china/{branch_name}_mean_values_fit_with_quadratic.png")


    #vol2 = np.array([50, 70, 90, 110, 130, 150])  # 单位是mV
    vol2 = np.array([50, 60, 70, 80, 90])  # 单位是mV
    mean2 = np.array(means)
    # 转换电压为V
    vol_V = vol2 * 1e-3  # 1 mV = 10^-3 V
    # 计算功率 P = vol^2 / (0.3 MΩ) * 50 us
    R = 0.313306e6  # 0.3 MΩ = 0.3 * 10^6 Ω
    t = 50e-6  # 50 μs = 50 * 10^-6 s
    # 功率计算公式
    P = (vol_V**2) / R * t  # 单位：V^2 / Ω * s = Joules (J)
    # 将能量转换为 MeV (1 J = 6.242e12 MeV)
    P_MeV = P * 6.242e12

    # 绘制图形：P_MeV 为横坐标，vol 为纵坐标
    plt.figure(figsize=(8, 6))
    plt.scatter(P_MeV, mean2, color='blue')


    # 进行线性拟合
    fit_params = np.polyfit(P_MeV, mean2, 1)  # 1 表示一次线性拟合
    fit_line = np.polyval(fit_params, P_MeV)  # 计算拟合直线的 y 值

    # 创建扩展范围的 x 数据，从 0 到 P_MeV 的最大值
    P_MeV_extended = np.linspace(0, P_MeV.max(), 100)  # 更细密的点用于平滑绘图
    # 计算扩展范围内的拟合直线 y 值
    fit_line_extended = np.polyval(fit_params, P_MeV_extended)


    # 绘制拟合线
    #plt.plot(P_MeV, fit_line, color='red', label='Linear Fit')
    plt.plot(P_MeV_extended, fit_line_extended, color='red', label='Linear Fit')
    plt.xlim(-1, max(P_MeV) + 1)
    # 获取拟合参数
    slope, intercept = fit_params
    text = f"Linear Fit Parameters:\nSlope: {slope:.5f}\nIntercept: {intercept:.5f}"
    # 在图上显示拟合参数
    plt.text(0.05, 0.95, text, transform=plt.gca().transAxes, fontsize=10,verticalalignment='top', bbox=dict(boxstyle='round', facecolor='white', alpha=0.5))

    plt.scatter(0, intercept, color='green', label='Intercept Point', zorder=3, s = 50)

    # 将均值添加到 TGraph
    #for i in range(n_points):
    #    #graph.SetPoint(i, i + 1, means[i])
    #    graph.SetPoint(i, P_MeV[i], mean2[i])  # x 使用 custom_x[i], y 使用 means[i]

    ## 对均值图进行线性拟合
    #linear_fit = ROOT.TF1("linear_fit", "pol1", P_MeV[0], P_MeV[-1])
    #graph.Fit(linear_fit, "R")
    ## 绘制拟合线
    #linear_fit.SetLineColor(ROOT.kRed)
    #linear_fit.Draw("SAME")


# 添加图表标题和标签
    plt.title("Amplitude(V) vs Q(MeV)")
    plt.xlabel("Q(MeV)")
    plt.ylabel("Amplitude(V)")
    plt.legend()

    # 显示图形
    plt.grid(True)
    plt.show()
    #plt.plot()

    # 输出拟合参数
    #print(f"拟合直线的斜率: {fit_params[0]}")
    plt.savefig(f"cupid-china/{branch_name}_P_vs_amp.png")


if __name__ == "__main__":

    #input_file = sys.argv[1]
    #branch_name = sys.argv[2]
    #input_file =  'TriggerEvent.root'
    #branch_name = 'Amp_filtered' 
    input_file =  '../rootfile/heater3.root'
    branch_name = 'amplitude' 
    tree_name = 'maxminusbaseline' 
    #main(input_file, branch_name)
    main(input_file, tree_name, branch_name)
