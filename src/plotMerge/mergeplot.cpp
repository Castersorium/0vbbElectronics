#include <TFile.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TKey.h>
#include <TList.h>
#include <TAxis.h>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>

void mergeGraphsFromFiles(const std::string &directoryPath, const std::string &outputFilePath)
{
    // 使用unique_ptr管理对象生命周期，避免手动删除
    auto multiGraph = std::make_unique<TMultiGraph>("mergedGraphs", "Merged Output Voltages Over Time");

    // 遍历目录中的所有文件
    for (const auto &entry : std::filesystem::directory_iterator(directoryPath))
    {
        std::string filePath = entry.path().string();

        // 仅处理.root文件
        if (filePath.substr(filePath.find_last_of(".") + 1) == "root")
        {
            // 打开ROOT文件
            std::unique_ptr<TFile> file(TFile::Open(filePath.c_str(), "READ"));
            if (!file || file->IsZombie())
            {
                std::cerr << "Error: Cannot open ROOT file: " << filePath << std::endl;
                continue;
            }

            // 获取所有的TGraphErrors对象
            TList *list = file->GetListOfKeys();
            TIter next(list);
            TKey *key;

            while ((key = (TKey *)next()))
            {
                if (std::string(key->GetClassName()) == "TGraphErrors")
                {
                    TGraphErrors *graph = (TGraphErrors *)key->ReadObj();
                    multiGraph->Add(graph);
                }
            }
        }
    }

    // 创建一个TCanvas来绘制合并后的图像
    auto canvas = std::make_unique<TCanvas>("mergedCanvas", "Merged Graphs", 1600, 900);

    // 开启X轴和Y轴的网格线
    canvas->SetGridx();
    canvas->SetGridy();

    // 绘制MultiGraph
    multiGraph->Draw("AEP");

    // 设置X轴和Y轴标题
    multiGraph->GetXaxis()->SetTitle("Time");
    multiGraph->GetYaxis()->SetTitle("Output [V]");

    // 设置X轴上的时间格式
    multiGraph->GetXaxis()->SetTimeDisplay(1);
    multiGraph->GetXaxis()->SetTimeFormat("%m/%d %H:%M:%S");

    // 创建一个图例
    canvas->BuildLegend();

    // 保存TCanvas到输出文件
    std::unique_ptr<TFile> outputFile(TFile::Open(outputFilePath.c_str(), "RECREATE"));
    if (!outputFile || outputFile->IsZombie())
    {
        std::cerr << "Error: Cannot create ROOT file: " << outputFilePath << std::endl;
        return;
    }

    canvas->Write();
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_directory> <output_file>" << std::endl;
        return 1;
    }

    std::string directoryPath = argv[1];
    std::string outputFilePath = argv[2];

    // 调用函数来合并图像并生成输出文件
    mergeGraphsFromFiles(directoryPath, outputFilePath);

    return 0;
}
