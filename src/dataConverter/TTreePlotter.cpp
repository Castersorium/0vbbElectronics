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
    TTree * tree = dynamic_cast<TTree *>( file->Get( "NIDAQReadings" ) );
    if ( !tree )
    {
        std::cerr << "Error: Cannot find TTree in ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 获取TTree中的所有branch的名称
    TObjArray * branches = tree->GetListOfBranches();
    size_t nBranches = branches->GetEntries();

    // 将TGraphErrors保存到新的ROOT文件
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    // 为每个branch创建一个TGraphErrors
    std::vector<TGraphErrors *> graphs_vec( nBranches );

    double xTimeStampMin = 0.0;
    double xTimeStampMax = 0.0;

    for ( size_t i = 0; i < nBranches; ++i )
    {
        TBranch * branch = static_cast<TBranch *>( branches->At( i ) );
        std::string branchName = branch->GetName();

        // 跳过timestamp这个branch
        if ( branchName == "timestamp" )
            continue;

        // 从TTree中获取数据
        size_t n = tree->GetEntries();

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
        std::vector<double> Amp_vec;
        double timestamp_ini = -1.0;  // 初始的时间戳

        // 遍历TTree中的每个entry
        for ( size_t entryIndex = 0; entryIndex < n; ++entryIndex )
        {
            tree->GetEntry( entryIndex );

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
                    double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();

                    // 找到直方图中最高bin的x轴值并打印出来
                    int maxBin = hist->GetMaximumBin();
                    double maxX = hist->GetBinCenter( maxBin );

                    // 计算标准差
                    double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
                    double sum_deviation_Y = std::accumulate( Amp_vec.begin(), Amp_vec.end(), 0.0, [maxX]( auto sum, auto val ) { return sum + ( val - maxX ) * ( val - maxX ); } );
                    double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
                    double stddev_Y = std::sqrt( sum_deviation_Y / Amp_vec.size() );

                    if ( hist->GetStdDev() > 0 )
                    {
                        x_vec.emplace_back( average_X );
                        y_vec.emplace_back( maxX );
                        ex_vec.emplace_back( stddev_X );
                        ey_vec.emplace_back( stddev_Y );
                    }

                    if ( isDebugModeActive )
                    {
                        std::cout << "Average timestamp for " << hist->GetName() << ": " << average_X << std::endl;
                        std::cout << "Standard deviation of timestamp for " << hist->GetName() << ": " << stddev_X << std::endl;
                        // 使用TDatime将平均时间戳转换为日期和时间
                        TDatime date( static_cast<UInt_t>( average_X ) );
                        std::cout << "Average date and time for " << hist->GetName() << ": " << date.AsString() << std::endl;
                        std::cout << "Mode amplitude for " << hist->GetName() << ": " << maxX << std::endl;
                        std::cout << "Standard deviation of amplitude for " << hist->GetName() << ": " << stddev_Y << std::endl;
                        hist->Write();
                    }

                    // 清空时间戳向量
                    timestamps_vec.clear();
                    Amp_vec.clear();

                    delete hist;
                }

                // 创建一个新的直方图
                timestamp_ini = floor( timestamp );
                std::string histName = branchName + "hist_" + std::to_string( static_cast<int>( timestamp_ini ) );
                hist = new TH1D( histName.c_str(), ( "Amplitude for " + std::to_string( static_cast<int>( timestamp_ini ) ) + "s;Amplitude [V];counts" ).c_str(), ( xMax - xMin ) / xBinWidth + 1, xMin, xMax );
                // 清空时间戳向量
                timestamps_vec.clear();
            }

            if ( !std::isnan( Amp ) )
            {
                // 将Amp_2cmLMO的值填充到直方图的相应时间窗口中
                hist->Fill( Amp );

                // 将当前的时间戳添加到向量中
                timestamps_vec.emplace_back( timestamp );
                Amp_vec.emplace_back( Amp );
            }
        }

        // 保存最后一个直方图
        if ( hist )
        {
            // 计算时间戳的平均值并打印出来
            double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();

            // 找到直方图中最高bin的x轴值并打印出来
            int maxBin = hist->GetMaximumBin();
            double maxX = hist->GetBinCenter( maxBin );

            // 计算标准差
            double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
            double sum_deviation_Y = std::accumulate( Amp_vec.begin(), Amp_vec.end(), 0.0, [maxX]( auto sum, auto val ) { return sum + ( val - maxX ) * ( val - maxX ); } );
            double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
            double stddev_Y = std::sqrt( sum_deviation_Y / Amp_vec.size() );

            if ( hist->GetStdDev() > 0 )
            {
                x_vec.emplace_back( average_X );
                y_vec.emplace_back( maxX );
                ex_vec.emplace_back( stddev_X );
                ey_vec.emplace_back( stddev_Y );
            }

            if ( isDebugModeActive )
            {
                std::cout << "Average timestamp for " << hist->GetName() << ": " << average_X << std::endl;
                std::cout << "Standard deviation of timestamp for " << hist->GetName() << ": " << stddev_X << std::endl;
                // 使用TDatime将平均时间戳转换为日期和时间
                TDatime date( static_cast<UInt_t>( average_X ) );
                std::cout << "Average date and time for " << hist->GetName() << ": " << date.AsString() << std::endl;
                std::cout << "Mode amplitude for " << hist->GetName() << ": " << maxX << std::endl;
                std::cout << "Standard deviation of amplitude for " << hist->GetName() << ": " << stddev_Y << std::endl;
                hist->Write();
            }

            // 清空时间戳向量
            timestamps_vec.clear();
            Amp_vec.clear();

            delete hist;
        }

        // 创建TGraphErrors并命名
        TGraphErrors * graph = new TGraphErrors( x_vec.size(), x_vec.data(), y_vec.data(), ex_vec.data(), ey_vec.data() );
        graph->SetName( branchName.c_str() );
        graph->SetTitle( branchName.c_str() );

        // 设置点的样式
        graph->SetMarkerStyle( MarkerStyle_vec[i / 8] );  // 设置点的样式
        graph->SetMarkerSize( 0.7 );  // 设置点的大小
        // 设置TGraphErrors的颜色
        graph->SetMarkerColor( Color_vec[i % 8] );
        graph->SetLineWidth( 1 );
        graph->SetFillStyle( 0 );

        graphs_vec[i] = graph;
    }

    // The time offset is the one that will be used by all graphs.
    // If one changes it, it will be changed even on the graphs already defined
    gStyle->SetTimeOffset( 0 );

    // 创建一个TCanvas
    TCanvas * canvas = new TCanvas( "canvas_Amp", "canvas_Amp", 1600, 900 );

    // 开启X轴和Y轴的网格线
    canvas->SetGridx();
    canvas->SetGridy();

    // 创建一个MultiGraph
    TMultiGraph * multiGraph = new TMultiGraph( "mg_Amp", "Output voltage of each channel" );

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

    multiGraph->GetXaxis()->SetTitle( "Time" );
    multiGraph->GetYaxis()->SetTitle( "Output [V]" );
    multiGraph->GetXaxis()->SetRangeUser( xTimeStampMin - 60, xTimeStampMax + 60 );
    multiGraph->GetXaxis()->SetTimeDisplay( 1 );
    // 设置X轴上的时间格式
    //multiGraph->GetXaxis()->SetTimeFormat( "%a %d/%m/%y %H:%M:%S" );
    multiGraph->GetXaxis()->SetTimeFormat( "%m/%d %H:%M:%S" );

    // 创建一个图例
    canvas->BuildLegend();

    // 将TCanvas写入文件
    canvas->Write( "canvas_Amp" );

    outputFile->Close();

    delete outputFile;
    delete file;
}

// 从TTree创建TGraphErrors并保存到ROOT文件的方法
void TTreePlotter::createBlueforsTemperatureGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const
{
    // 打开ROOT文件
    TFile * file = TFile::Open( rootFilePath.c_str(), "READ" );
    if ( !file || file->IsZombie() )
    {
        std::cerr << "Error: Cannot open ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 从文件中获取TTree
    TTree * tree = dynamic_cast<TTree *>( file->Get( "TemperatureReadings" ) );
    if ( !tree )
    {
        std::cerr << "Error: Cannot find TTree in ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 获取TTree中的所有branch的名称
    TObjArray * branches = tree->GetListOfBranches();
    size_t nBranches = branches->GetEntries();

    // 将TGraphErrors保存到新的ROOT文件
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    // 为每个branch创建一个TGraphErrors
    std::vector<TGraphErrors *> graphs_vec( nBranches );

    double xTimeStampMin = 0.0;
    double xTimeStampMax = 0.0;

    for ( size_t i = 0; i < nBranches; ++i )
    {
        TBranch * branch = static_cast<TBranch *>( branches->At( i ) );
        std::string branchName = branch->GetName();

        // 跳过timestamp这个branch
        if ( branchName == "timestamp" )
            continue;

        // 从TTree中获取数据
        size_t n = tree->GetEntries();

        // 创建向量来存储每个时间窗口内的平均时间戳和模式幅度
        std::vector <double> x_vec = {};
        std::vector <double> y_vec = {};
        std::vector <double> ex_vec = {};  // x轴误差
        std::vector <double> ey_vec = {};  // y轴误差

        double timestamp = 0;
        double temperature = 0;

        tree->SetBranchAddress( "timestamp", &timestamp );
        tree->SetBranchAddress( branchName.c_str(), &temperature );

        // 创建一个向量来存储每个时间窗口内的所有时间戳
        std::vector<double> timestamps_vec;
        std::vector<double> temperatures_vec;
        double timestamp_ini = -1.0;  // 初始的时间戳

        // 遍历TTree中的每个entry
        for ( size_t entryIndex = 0; entryIndex < n; ++entryIndex )
        {
            tree->GetEntry( entryIndex );

            // 更新x轴的最小值和最大值
            if ( timestamp < xTimeStampMin )
                xTimeStampMin = timestamp;
            if ( timestamp > xTimeStampMax )
                xTimeStampMax = timestamp;

            // 如果当前的时间戳超出了[timestamp_ini, timestamp_ini + timeWindow)的范围
            if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + timeWindow )
            {
                if ( !timestamps_vec.empty() )
                {
                    // 计算时间戳的平均值并打印出来
                    double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
                    double average_Y = std::accumulate( temperatures_vec.begin(), temperatures_vec.end(), 0.0 ) / temperatures_vec.size();

                    // 计算标准差
                    double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
                    double sum_deviation_Y = std::accumulate( temperatures_vec.begin(), temperatures_vec.end(), 0.0, [average_Y]( auto sum, auto val ) { return sum + ( val - average_Y ) * ( val - average_Y ); } );
                    double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
                    double stddev_Y = std::sqrt( sum_deviation_Y / temperatures_vec.size() );

                    if ( isDebugModeActive )
                    {
                        std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                        std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                        // 使用TDatime将平均时间戳转换为日期和时间
                        TDatime date( static_cast<UInt_t>( average_X ) );
                        std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                        std::cout << "Average temperature for " << static_cast<int>( timestamp_ini ) << ": " << average_Y << std::endl;
                        std::cout << "Standard deviation of temperature for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y << std::endl;
                    }

                    if ( average_Y != 0 )
                    {
                        x_vec.emplace_back( average_X );
                        y_vec.emplace_back( average_Y );
                        ex_vec.emplace_back( stddev_X );
                        ey_vec.emplace_back( stddev_Y );
                    }
                }

                timestamp_ini = floor( timestamp );

                // 清空时间戳向量
                timestamps_vec.clear();
                temperatures_vec.clear();
            }

            if ( temperature != 0.0 )
            {
                // 将当前的时间戳添加到向量中
                timestamps_vec.emplace_back( timestamp );
                temperatures_vec.emplace_back( temperature );
            }
        }

        // 保存最后一个点
        if ( !timestamps_vec.empty() )
        {
            // 计算时间戳的平均值并打印出来
            double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
            double average_Y = std::accumulate( temperatures_vec.begin(), temperatures_vec.end(), 0.0 ) / temperatures_vec.size();

            // 计算标准差
            double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
            double sum_deviation_Y = std::accumulate( temperatures_vec.begin(), temperatures_vec.end(), 0.0, [average_Y]( auto sum, auto val ) { return sum + ( val - average_Y ) * ( val - average_Y ); } );
            double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
            double stddev_Y = std::sqrt( sum_deviation_Y / temperatures_vec.size() );

            if ( isDebugModeActive )
            {
                std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                // 使用TDatime将平均时间戳转换为日期和时间
                TDatime date( static_cast<UInt_t>( average_X ) );
                std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                std::cout << "Average temperature for " << static_cast<int>( timestamp_ini ) << ": " << average_Y << std::endl;
                std::cout << "Standard deviation of temperature for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y << std::endl;
            }

            if ( average_Y != 0 )
            {
                x_vec.emplace_back( average_X );
                y_vec.emplace_back( average_Y );
                ex_vec.emplace_back( stddev_X );
                ey_vec.emplace_back( stddev_Y );
            }

            // 清空时间戳向量
            timestamps_vec.clear();
            temperatures_vec.clear();
        }

        // 创建TGraphErrors并命名
        TGraphErrors * graph = new TGraphErrors( x_vec.size(), x_vec.data(), y_vec.data(), ex_vec.data(), ey_vec.data() );
        graph->SetName( branchName.c_str() );
        graph->SetTitle( branchName.c_str() );

        // 设置点的样式
        graph->SetMarkerStyle( MarkerStyle_vec[i / 8] );  // 设置点的样式
        graph->SetMarkerSize( 0.7 );  // 设置点的大小
        // 设置TGraphErrors的颜色
        graph->SetMarkerColor( Color_vec[i % 8] );
        graph->SetLineWidth( 1 );
        graph->SetFillStyle( 0 );

        graphs_vec[i] = graph;
    }

    // The time offset is the one that will be used by all graphs.
    // If one changes it, it will be changed even on the graphs already defined
    gStyle->SetTimeOffset( 0 );

    // 创建一个TCanvas
    TCanvas * canvas = new TCanvas( "canvas_Temperature", "canvas_Temperature", 1600, 900 );

    // 开启X轴和Y轴的网格线
    canvas->SetGridx();
    canvas->SetGridy();

    // 创建一个MultiGraph
    TMultiGraph * multiGraph = new TMultiGraph( "mg_Temperature", "Temperature of T1_50K, T2_4K, T5_STILL, T6_MXC and T7_SAMPLE" );

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

    multiGraph->GetXaxis()->SetTitle( "Time" );
    multiGraph->GetYaxis()->SetTitle( "Temperature [K]" );
    multiGraph->GetXaxis()->SetRangeUser( xTimeStampMin - 60, xTimeStampMax + 60 );
    multiGraph->GetXaxis()->SetTimeDisplay( 1 );
    // 设置X轴上的时间格式
    //multiGraph->GetXaxis()->SetTimeFormat( "%a %d/%m/%y %H:%M:%S" );
    multiGraph->GetXaxis()->SetTimeFormat( "%m/%d %H:%M:%S" );

    // 创建一个图例
    canvas->BuildLegend();

    // 将TCanvas写入文件
    canvas->Write( "canvas_Temperature" );

    outputFile->Close();

    delete outputFile;
    delete file;
}

// 从TTree创建TGraphErrors并保存到ROOT文件的方法
void TTreePlotter::createMultimeterGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const
{
    // 打开ROOT文件
    TFile * file = TFile::Open( rootFilePath.c_str(), "READ" );
    if ( !file || file->IsZombie() )
    {
        std::cerr << "Error: Cannot open ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 从文件中获取TTree
    TTree * tree = dynamic_cast<TTree *>( file->Get( "MultimeterReadings" ) );
    if ( !tree )
    {
        std::cerr << "Error: Cannot find TTree in ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 获取TTree中的所有branch的名称
    TObjArray * branches = tree->GetListOfBranches();
    size_t nBranches = branches->GetEntries();

    // 将TGraphErrors保存到新的ROOT文件
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    // 为每个branch创建一个TGraphErrors
    std::vector<TGraphErrors *> graphs_vec( nBranches );

    double xTimeStampMin = 0.0;
    double xTimeStampMax = 0.0;

    for ( size_t i = 0; i < nBranches; ++i )
    {
        TBranch * branch = static_cast<TBranch *>( branches->At( i ) );
        std::string branchName = branch->GetName();

        // 跳过timestamp这个branch
        if ( branchName == "timestamp" )
            continue;

        // 从TTree中获取数据
        size_t n = tree->GetEntries();

        // 创建向量来存储每个时间窗口内的平均时间戳和模式幅度
        std::vector <double> x_vec = {};
        std::vector <double> y_vec = {};
        std::vector <double> ex_vec = {};  // x轴误差
        std::vector <double> ey_vec = {};  // y轴误差

        double timestamp = 0;
        double reading = 0;

        tree->SetBranchAddress( "timestamp", &timestamp );
        tree->SetBranchAddress( branchName.c_str(), &reading );

        // 创建一个向量来存储每个时间窗口内的所有时间戳
        std::vector<double> timestamps_vec;
        std::vector<double> readings_vec;
        double timestamp_ini = -1.0;  // 初始的时间戳

        // 遍历TTree中的每个entry
        for ( size_t entryIndex = 0; entryIndex < n; ++entryIndex )
        {
            tree->GetEntry( entryIndex );

            // 更新x轴的最小值和最大值
            if ( timestamp < xTimeStampMin )
                xTimeStampMin = timestamp;
            if ( timestamp > xTimeStampMax )
                xTimeStampMax = timestamp;

            // 如果当前的时间戳超出了[timestamp_ini, timestamp_ini + timeWindow)的范围
            if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + timeWindow )
            {
                if ( !timestamps_vec.empty() )
                {
                    // 计算时间戳的平均值并打印出来
                    double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
                    double average_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0 ) / readings_vec.size();

                    // 计算标准差
                    double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
                    double sum_deviation_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0, [average_Y]( auto sum, auto val ) { return sum + ( val - average_Y ) * ( val - average_Y ); } );
                    double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
                    double stddev_Y = std::sqrt( sum_deviation_Y / readings_vec.size() );

                    if ( isDebugModeActive )
                    {
                        std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                        std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                        // 使用TDatime将平均时间戳转换为日期和时间
                        TDatime date( static_cast<UInt_t>( average_X ) );
                        std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                        std::cout << "Average reading for " << static_cast<int>( timestamp_ini ) << ": " << average_Y << std::endl;
                        std::cout << "Standard deviation of reading for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y << std::endl;
                    }

                    x_vec.emplace_back( average_X );
                    y_vec.emplace_back( average_Y );
                    ex_vec.emplace_back( stddev_X );
                    ey_vec.emplace_back( stddev_Y );

                }

                timestamp_ini = floor( timestamp );

                // 清空时间戳向量
                timestamps_vec.clear();
                readings_vec.clear();
            }

            // 将当前的时间戳添加到向量中
            timestamps_vec.emplace_back( timestamp );
            readings_vec.emplace_back( reading );
        }

        // 保存最后一个点
        if ( !timestamps_vec.empty() )
        {
            // 计算时间戳的平均值并打印出来
            double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
            double average_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0 ) / readings_vec.size();

            // 计算标准差
            double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
            double sum_deviation_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0, [average_Y]( auto sum, auto val ) { return sum + ( val - average_Y ) * ( val - average_Y ); } );
            double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
            double stddev_Y = std::sqrt( sum_deviation_Y / readings_vec.size() );

            if ( isDebugModeActive )
            {
                std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                // 使用TDatime将平均时间戳转换为日期和时间
                TDatime date( static_cast<UInt_t>( average_X ) );
                std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                std::cout << "Average reading for " << static_cast<int>( timestamp_ini ) << ": " << average_Y << std::endl;
                std::cout << "Standard deviation of reading for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y << std::endl;
            }

            x_vec.emplace_back( average_X );
            y_vec.emplace_back( average_Y );
            ex_vec.emplace_back( stddev_X );
            ey_vec.emplace_back( stddev_Y );
        }

        // 创建TGraphErrors并命名
        TGraphErrors * graph = new TGraphErrors( x_vec.size(), x_vec.data(), y_vec.data(), ex_vec.data(), ey_vec.data() );
        graph->SetName( branchName.c_str() );
        graph->SetTitle( branchName.c_str() );

        // 设置点的样式
        graph->SetMarkerStyle( MarkerStyle_vec[i / 8] );  // 设置点的样式
        graph->SetMarkerSize( 0.7 );  // 设置点的大小
        // 设置TGraphErrors的颜色
        graph->SetMarkerColor( Color_vec[i % 8] );
        graph->SetLineWidth( 1 );
        graph->SetFillStyle( 0 );

        graphs_vec[i] = graph;
    }

    // The time offset is the one that will be used by all graphs.
    // If one changes it, it will be changed even on the graphs already defined
    gStyle->SetTimeOffset( 0 );

    // 创建一个TCanvas
    TCanvas * canvas = new TCanvas( "canvas_MultimeterReading", "canvas_MultimeterReading", 1600, 900 );

    // 开启X轴和Y轴的网格线
    canvas->SetGridx();
    canvas->SetGridy();

    // 创建一个MultiGraph
    TMultiGraph * multiGraph = new TMultiGraph( "mg_MultimeterReading", "Multimeter Reading out" );

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

    multiGraph->GetXaxis()->SetTitle( "Time" );
    multiGraph->GetYaxis()->SetTitle( "Multimeter Reading" );
    multiGraph->GetXaxis()->SetRangeUser( xTimeStampMin - 60, xTimeStampMax + 60 );
    multiGraph->GetXaxis()->SetTimeDisplay( 1 );
    // 设置X轴上的时间格式
    //multiGraph->GetXaxis()->SetTimeFormat( "%a %d/%m/%y %H:%M:%S" );
    multiGraph->GetXaxis()->SetTimeFormat( "%m/%d %H:%M:%S" );

    // 创建一个图例
    canvas->BuildLegend();

    // 将TCanvas写入文件
    canvas->Write( "canvas_MultimeterReading" );

    outputFile->Close();

    delete outputFile;
    delete file;
}

// 从TTree创建TGraphErrors并保存到ROOT文件的方法
void TTreePlotter::createCelsiusGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const
{
    // 打开ROOT文件
    TFile * file = TFile::Open( rootFilePath.c_str(), "READ" );
    if ( !file || file->IsZombie() )
    {
        std::cerr << "Error: Cannot open ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 从文件中获取TTree
    TTree * tree = dynamic_cast<TTree *>( file->Get( "Centigrade_Humidity" ) );
    if ( !tree )
    {
        std::cerr << "Error: Cannot find TTree in ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // 获取TTree中的所有branch的名称
    TObjArray * branches = tree->GetListOfBranches();
    size_t nBranches = branches->GetEntries();

    // 将TGraphErrors保存到新的ROOT文件
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    // 为每个branch创建一个TGraphErrors
    std::vector<TGraphErrors *> graphs_vec( nBranches );

    double xTimeStampMin = 0.0;
    double xTimeStampMax = 0.0;

    for ( size_t i = 0; i < nBranches; ++i )
    {
        TBranch * branch = static_cast<TBranch *>( branches->At( i ) );
        std::string branchName = branch->GetName();

        // 跳过timestamp这个branch
        if ( branchName == "timestamp" )
            continue;

        // 从TTree中获取数据
        size_t n = tree->GetEntries();

        // 创建向量来存储每个时间窗口内的平均时间戳和模式幅度
        std::vector <double> x_vec = {};
        std::vector <double> y_vec = {};
        std::vector <double> ex_vec = {};  // x轴误差
        std::vector <double> ey_vec = {};  // y轴误差

        double timestamp = 0;
        double reading = 0;

        tree->SetBranchAddress( "timestamp", &timestamp );
        tree->SetBranchAddress( branchName.c_str(), &reading );

        // 创建一个向量来存储每个时间窗口内的所有时间戳
        std::vector<double> timestamps_vec;
        std::vector<double> readings_vec;
        double timestamp_ini = -1.0;  // 初始的时间戳

        // 遍历TTree中的每个entry
        for ( size_t entryIndex = 0; entryIndex < n; ++entryIndex )
        {
            tree->GetEntry( entryIndex );

            // 更新x轴的最小值和最大值
            if ( timestamp < xTimeStampMin )
                xTimeStampMin = timestamp;
            if ( timestamp > xTimeStampMax )
                xTimeStampMax = timestamp;

            // 如果当前的时间戳超出了[timestamp_ini, timestamp_ini + timeWindow)的范围
            if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + timeWindow )
            {
                if ( !timestamps_vec.empty() )
                {
                    // 计算时间戳的平均值并打印出来
                    double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
                    double average_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0 ) / readings_vec.size();

                    // 计算标准差
                    double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
                    double sum_deviation_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0, [average_Y]( auto sum, auto val ) { return sum + ( val - average_Y ) * ( val - average_Y ); } );
                    double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
                    double stddev_Y = std::sqrt( sum_deviation_Y / readings_vec.size() );

                    if ( isDebugModeActive )
                    {
                        std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                        std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                        // 使用TDatime将平均时间戳转换为日期和时间
                        TDatime date( static_cast<UInt_t>( average_X ) );
                        std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                        std::cout << "Average reading for " << static_cast<int>( timestamp_ini ) << ": " << average_Y << std::endl;
                        std::cout << "Standard deviation of reading for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y << std::endl;
                    }

                    x_vec.emplace_back( average_X );
                    y_vec.emplace_back( average_Y );
                    ex_vec.emplace_back( stddev_X );
                    ey_vec.emplace_back( stddev_Y );

                }

                timestamp_ini = floor( timestamp );

                // 清空时间戳向量
                timestamps_vec.clear();
                readings_vec.clear();
            }

            // 将当前的时间戳添加到向量中
            timestamps_vec.emplace_back( timestamp );
            readings_vec.emplace_back( reading );
        }

        // 保存最后一个点
        if ( !timestamps_vec.empty() )
        {
            // 计算时间戳的平均值并打印出来
            double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
            double average_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0 ) / readings_vec.size();

            // 计算标准差
            double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
            double sum_deviation_Y = std::accumulate( readings_vec.begin(), readings_vec.end(), 0.0, [average_Y]( auto sum, auto val ) { return sum + ( val - average_Y ) * ( val - average_Y ); } );
            double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
            double stddev_Y = std::sqrt( sum_deviation_Y / readings_vec.size() );

            if ( isDebugModeActive )
            {
                std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                // 使用TDatime将平均时间戳转换为日期和时间
                TDatime date( static_cast<UInt_t>( average_X ) );
                std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                std::cout << "Average reading for " << static_cast<int>( timestamp_ini ) << ": " << average_Y << std::endl;
                std::cout << "Standard deviation of reading for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y << std::endl;
            }

            x_vec.emplace_back( average_X );
            y_vec.emplace_back( average_Y );
            ex_vec.emplace_back( stddev_X );
            ey_vec.emplace_back( stddev_Y );
        }

        // 创建TGraphErrors并命名
        TGraphErrors * graph = new TGraphErrors( x_vec.size(), x_vec.data(), y_vec.data(), ex_vec.data(), ey_vec.data() );
        graph->SetName( branchName.c_str() );
        graph->SetTitle( branchName.c_str() );

        // 设置点的样式
        graph->SetMarkerStyle( MarkerStyle_vec[i / 8] );  // 设置点的样式
        graph->SetMarkerSize( 0.7 );  // 设置点的大小
        // 设置TGraphErrors的颜色
        graph->SetMarkerColor( Color_vec[i % 8] );
        graph->SetLineWidth( 1 );
        graph->SetFillStyle( 0 );

        graphs_vec[i] = graph;
    }

    // The time offset is the one that will be used by all graphs.
    // If one changes it, it will be changed even on the graphs already defined
    gStyle->SetTimeOffset( 0 );

    // 创建一个TCanvas
    TCanvas * canvas = new TCanvas( "canvas_Centigrade_Humidity", "canvas_Centigrade_Humidity", 1600, 900 );

    // 开启X轴和Y轴的网格线
    canvas->SetGridx();
    canvas->SetGridy();

    // 创建一个MultiGraph
    TMultiGraph * multiGraph = new TMultiGraph( "mg_Centigrade_Humidity", "Centigrade and Humidity" );

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

    multiGraph->GetXaxis()->SetTitle( "Time" );
    multiGraph->GetYaxis()->SetTitle( "Centigrade and Humidity" );
    multiGraph->GetXaxis()->SetRangeUser( xTimeStampMin - 60, xTimeStampMax + 60 );
    multiGraph->GetXaxis()->SetTimeDisplay( 1 );
    // 设置X轴上的时间格式
    //multiGraph->GetXaxis()->SetTimeFormat( "%a %d/%m/%y %H:%M:%S" );
    multiGraph->GetXaxis()->SetTimeFormat( "%m/%d %H:%M:%S" );

    // 创建一个图例
    canvas->BuildLegend();

    // 将TCanvas写入文件
    canvas->Write( "canvas_Centigrade_Humidity" );

    outputFile->Close();

    delete outputFile;
    delete file;
}
}
