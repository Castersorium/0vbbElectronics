#include <iostream> // C++ header
#include <cmath>
#include <vector>
#include <numeric>

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TKey.h>

#include "TTreePlotter.hpp" // my header

namespace TTREEIO
{
// 从TTree创建TGraphErrors并保存到ROOT文件的方法
void TTreePlotter::createGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath )
{
    // 打开ROOT文件
    TFile * file = TFile::Open( rootFilePath.c_str(), "READ" );
    if ( !file || file->IsZombie() )
    {
        std::cerr << "Error: Cannot open ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 从文件中获取TTree
    TTree * tree = dynamic_cast<TTree *>( file->Get( "tree" ) );
    if ( !tree )
    {
        std::cerr << "Error: Cannot find TTree in ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 从TTree中获取数据
    int n = tree->GetEntries();

    std::vector <double> x_vec = {};
    std::vector <double> y_2cmLMO_vec = {};
    std::vector <double> ex_vec = {};  // x轴误差
    std::vector <double> ey_2cmLMO_vec = {};  // y轴误差

    double timestamp = 0;
    double Amp_2cmLMO = 0;

    tree->SetBranchAddress( "timestamp", &timestamp );
    tree->SetBranchAddress( "2cmLMO", &Amp_2cmLMO );

    // 将TGraphErrors保存到新的ROOT文件
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    // 创建一个向量来存储每个时间窗口内的所有时间戳
    std::vector<double> timestamps_vec;
    TH1D * hist = nullptr;  // 当前的直方图
    double timestamp_ini = -1.0;  // 初始的时间戳

    // 遍历TTree中的每个entry
    for ( int i = 0; i < n; ++i )
    {
        tree->GetEntry( i );

        // 如果当前的时间戳超出了[timestamp_ini, timestamp_ini + timeWindow)的范围
        if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + timeWindow )
        {
            // 保存当前的直方图
            if ( hist )
            {
                // 计算时间戳的平均值并打印出来
                double average = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();

                // 计算标准差
                double sum_deviation = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average]( double sum, double val ) { return sum + ( val - average ) * ( val - average ); } );
                double stddev = std::sqrt( sum_deviation / timestamps_vec.size() );

                // 找到直方图中最高bin的x轴值并打印出来
                int maxBin = hist->GetMaximumBin();
                double maxX = hist->GetBinCenter( maxBin );

                x_vec.emplace_back( average );
                y_2cmLMO_vec.emplace_back( maxX );
                ex_vec.emplace_back( stddev );
                ey_2cmLMO_vec.emplace_back( hist->GetBinWidth( 0 ) );

                if ( isDebugModeActive )
                {
                    std::cout << "Average timestamp for " << hist->GetName() << ": " << average << std::endl;
                    std::cout << "Standard deviation of timestamp for " << hist->GetName() << ": " << stddev << std::endl;
                    // 使用TDatime将平均时间戳转换为日期和时间
                    TDatime date( static_cast<UInt_t>( average ) );
                    std::cout << "Average date and time for " << hist->GetName() << ": " << date.AsString() << std::endl;
                    std::cout << "Mode amplitude for " << hist->GetName() << ": " << maxX << std::endl;
                    hist->Write();
                }
                delete hist;
            }

            // 创建一个新的直方图
            timestamp_ini = floor( timestamp );
            std::string histName = "hist_" + std::to_string( static_cast<int>( timestamp_ini ) );
            hist = new TH1D( histName.c_str(), ( "Amplitude for " + std::to_string( static_cast<int>( timestamp_ini ) ) + "s;Amplitude [V];counts" ).c_str(), ( xMax - xMin ) / xBinWidth, xMin, xMax );
            // 清空时间戳向量
            timestamps_vec.clear();
        }

        // 将Amp_2cmLMO的值填充到直方图的相应时间窗口中
        hist->Fill( Amp_2cmLMO );

        // 将当前的时间戳添加到向量中
        timestamps_vec.emplace_back( timestamp );
    }

    // 保存最后一个直方图
    if ( hist )
    {
        // 计算时间戳的平均值并打印出来
        double average = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();

        // 计算标准差
        double sum_deviation = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average]( double sum, double val ) { return sum + ( val - average ) * ( val - average ); } );
        double stddev = std::sqrt( sum_deviation / timestamps_vec.size() );

        // 找到直方图中最高bin的x轴值并打印出来
        int maxBin = hist->GetMaximumBin();
        double maxX = hist->GetBinCenter( maxBin );

        x_vec.emplace_back( average );
        y_2cmLMO_vec.emplace_back( maxX );
        ex_vec.emplace_back( stddev );
        ey_2cmLMO_vec.emplace_back( hist->GetBinWidth( 0 ) );

        if ( isDebugModeActive )
        {
            std::cout << "Average timestamp for " << hist->GetName() << ": " << average << std::endl;
            std::cout << "Standard deviation of timestamp for " << hist->GetName() << ": " << stddev << std::endl;
            // 使用TDatime将平均时间戳转换为日期和时间
            TDatime date( static_cast<UInt_t>( average ) );
            std::cout << "Average date and time for " << hist->GetName() << ": " << date.AsString() << std::endl;
            std::cout << "Mode amplitude for " << hist->GetName() << ": " << maxX << std::endl;
            hist->Write();
        }
        delete hist;
    }

    // 创建TGraphErrors
    TGraphErrors * graph = new TGraphErrors( x_vec.size(), x_vec.data(), y_2cmLMO_vec.data(), ex_vec.data(), ey_2cmLMO_vec.data() );

    // 设置点的样式和颜色
    graph->SetMarkerStyle( 21 );  // 设置点的样式为正方形
    graph->SetMarkerColor( kBlue );  // 设置点的颜色为蓝色

    // 不绘制点之间的连线
    graph->SetLineStyle( 0 );

    graph->Write( "graph" );
    outputFile->Close();

    delete outputFile;
    delete file;
}
}
