#include <iostream> // C++ header
#include <cmath>
#include <vector>
#include <numeric>

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TKey.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>

#include "TTreePlotter.hpp" // my header

namespace TTREEIO
{
// 从TTree创建TGraphErrors并保存到ROOT文件的方法
void TTreePlotter::createNIDAQGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const
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

    // 获取TTree中的所有branch的名称
    TObjArray * branches = tree->GetListOfBranches();
    int nBranches = branches->GetEntries();

    // 将TGraphErrors保存到新的ROOT文件
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    // 创建一个颜色的数组
    Color colors[] = { kRed, kBlue, kGreen, kMagenta, kCyan, kYellow, kBlack, kOrange };

    // 为每个branch创建一个TGraphErrors
    std::vector<TGraphErrors *> graphs_vec( nBranches );

    double xTimeStampMin = 0.0;
    double xTimeStampMax = 0.0;

    for ( int i = 0; i < nBranches; ++i )
    {
        TBranch * branch = static_cast<TBranch *>( branches->At( i ) );
        std::string branchName = branch->GetName();

        // 跳过timestamp这个branch
        if ( branchName == "timestamp" )
            continue;

        // 从TTree中获取数据
        int n = tree->GetEntries();

        // 创建向量来存储每个时间窗口内的平均时间戳和模式幅度
        std::vector <double> x_vec = {};
        std::vector <double> y_vec = {};
        std::vector <double> ex_vec = {};  // x轴误差
        std::vector <double> ey_vec = {};  // y轴误差

        double timestamp = 0;
        double Amp = 0;

        tree->SetBranchAddress( "timestamp", &timestamp );
        tree->SetBranchAddress( branchName.c_str(), &Amp );

        // 创建一个向量来存储每个时间窗口内的所有时间戳
        std::vector<double> timestamps_vec;
        TH1D * hist = nullptr;  // 当前的直方图
        double timestamp_ini = -1.0;  // 初始的时间戳

        // 遍历TTree中的每个entry
        for ( int i = 0; i < n; ++i )
        {
            tree->GetEntry( i );

            // 更新x轴的最小值和最大值
            if ( timestamp < xTimeStampMin )
                xTimeStampMin = timestamp;
            if ( timestamp > xTimeStampMax )
                xTimeStampMax = timestamp;

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
                    y_vec.emplace_back( maxX );
                    ex_vec.emplace_back( stddev );
                    ey_vec.emplace_back( hist->GetBinWidth( 0 ) );

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
            hist->Fill( Amp );

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
            y_vec.emplace_back( maxX );
            ex_vec.emplace_back( stddev );
            ey_vec.emplace_back( hist->GetBinWidth( 0 ) );

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

        // 创建TGraphErrors并命名
        TGraphErrors * graph = new TGraphErrors( x_vec.size(), x_vec.data(), y_vec.data(), ex_vec.data(), ey_vec.data() );
        graph->SetName( branchName.c_str() );
        graph->SetTitle( branchName.c_str() );

        // 设置点的样式
        graph->SetMarkerStyle( 21 );  // 设置点的样式为正方形
        graph->SetMarkerSize( 0.5 );  // 设置点的大小
        // 设置TGraphErrors的颜色
        graph->SetMarkerColor( colors[i % 8] );
        graph->SetLineWidth( 2 );
        graph->SetFillStyle( 0 );

        graphs_vec[i] = graph;
    }

    // The time offset is the one that will be used by all graphs.
    // If one changes it, it will be changed even on the graphs already defined
    gStyle->SetTimeOffset( 0 );

    // 创建一个TCanvas
    TCanvas * canvas = new TCanvas( "canvas", "canvas", 1600, 900 );

    // 创建一个MultiGraph
    TMultiGraph * multiGraph = new TMultiGraph( "mg", "mg" );

    // 将所有的TGraphErrors添加到MultiGraph中
    for ( TGraphErrors * graphInstance : graphs_vec )
    {
        if ( !graphInstance )
            continue;

        if ( isDebugModeActive )
        {
            std::cout << "Adding graph " << graphInstance->GetName() << " to MultiGraph" << std::endl;
        }

        // 将TGraphErrors添加到MultiGraph中
        multiGraph->Add( graphInstance );
    }

    // 在TCanvas上绘制MultiGraph
    multiGraph->Draw( "AEP" );

    multiGraph->GetXaxis()->SetTitle( "Timestamp [s]" );
    multiGraph->GetYaxis()->SetTitle( "Output [V]" );
    multiGraph->GetXaxis()->SetRangeUser( xTimeStampMin - 60, xTimeStampMax + 60 );
    multiGraph->GetXaxis()->SetTimeDisplay( 1 );
    // 设置X轴上的时间格式
    //multiGraph->GetXaxis()->SetTimeFormat( "%a %d/%m/%y %H:%M:%S" );
    multiGraph->GetXaxis()->SetTimeFormat( "%m/%d %H:%M:%S" );

    // 创建一个图例
    canvas->BuildLegend();

    // 将TCanvas写入文件
    canvas->Write( "canvas" );

    outputFile->Close();

    delete outputFile;
    delete file;
}
}
