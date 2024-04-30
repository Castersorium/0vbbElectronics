#include <iostream> // C++ header
#include <cmath>
#include <vector>
#include <numeric>
#include <iomanip>
#include <sstream>

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TGraphErrors.h>
#include <TH1D.h>
#include <TKey.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TVirtualFFT.h>

#include "TTreePlotter.hpp" // my header

namespace TTREEIO
{
// 从TTree创建TGraphErrors并保存到ROOT文件的方法
void TTreePlotter::createNIDAQFFTFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const
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

    // 为每个branch创建两个个TGraphErrors分别用于表征6Hz噪声成分和50Hz噪声成分
    std::vector<TGraphErrors *> graphs_vec( nBranches * 2 );

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

        // 创建向量来存储每个时间窗口内的平均时间戳和噪声功率幅度
        std::vector <double> x_vec = {};
        std::vector <double> y_6Hz_vec = {};
        std::vector <double> y_50Hz_vec = {};
        std::vector <double> ex_vec = {};  // x轴误差
        std::vector <double> ey_6Hz_vec = {};  // y轴误差6Hz
        std::vector <double> ey_50Hz_vec = {};  // y轴误差50Hz

        double timestamp = 0;
        double Amp = 0;

        tree->SetBranchAddress( "timestamp", &timestamp );
        tree->SetBranchAddress( branchName.c_str(), &Amp );

        // 创建一个向量来存储每个时间窗口内的所有时间戳
        std::vector<double> timestamps_vec;
        TH1D * hist = nullptr;  // 当前的直方图
        std::vector<double> Power_6Hz_vec;
        std::vector<double> Power_50Hz_vec;
        double timestamp_ini = -1.0;  // 初始的时间戳
        double timestamp_ini_FFT = -1.0;  // 初始的FFT时间戳

        int FFT_counter = 0;

        // 遍历TTree中的每个entry
        for ( size_t entryIndex = 0; entryIndex < n; ++entryIndex )
        {
            tree->GetEntry( entryIndex );

            // 更新x轴的最小值和最大值
            if ( timestamp < xTimeStampMin )
                xTimeStampMin = timestamp;
            if ( timestamp > xTimeStampMax )
                xTimeStampMax = timestamp;

            // 如果当前的时间戳超出了[timestamp_ini_FFT, timestamp_ini_FFT + FFTtimeWindow)的范围
            if ( timestamp < timestamp_ini_FFT || timestamp >= timestamp_ini_FFT + FFTtimeWindow )
            {
                // 保存当前的直方图
                if ( hist )
                {
                    TH1 * hm = nullptr;
                    // 使用FFT计算功率谱
                    TVirtualFFT::SetTransform( nullptr );
                    hm = hist->FFT( hm, "MAG" );
                    if ( !hm )
                    {
                        std::cerr << "Error: Cannot perform FFT on histogram" << std::endl;
                        return;
                    }
                    std::string FFThistName = "FFT_" + branchName + "_" + std::to_string( FFT_counter ) + "_hist_" + std::to_string( static_cast<int>( timestamp_ini_FFT ) );
                    hm->SetName( FFThistName.c_str() );
                    hm->SetTitle( ( "FFT for " + std::to_string( static_cast<int>( timestamp_ini_FFT ) ) + "s;Frequency * " + std::to_string( static_cast<int>( FFTtimeWindow ) ) + " [Hz];Power [W/" + to_string_with_precision( 1.0 / FFTtimeWindow, 3 ) + "Hz]" ).c_str() );

                    Power_6Hz_vec.emplace_back( hm->GetBinContent( hm->GetXaxis()->FindBin( 6.0 * FFTtimeWindow ) ) );
                    Power_50Hz_vec.emplace_back( hm->GetBinContent( hm->GetXaxis()->FindBin( 50.0 * FFTtimeWindow ) ) );
                    if ( isDebugModeActive )
                    {
                        hist->Write();
                        hm->Write();
                    }

                    FFT_counter++;
                    delete hist;
                }
                // 创建一个新的直方图
                timestamp_ini_FFT = floor( timestamp );
                std::string histName = branchName + std::to_string( FFT_counter ) + "hist_" + std::to_string( static_cast<int>( timestamp_ini_FFT ) );
                hist = new TH1D( histName.c_str(), ( "Amplitude for " + std::to_string( static_cast<int>( timestamp_ini_FFT ) ) + "s;time [s];Amplitude [V]" ).c_str(), ( FFTtimeWindow - 0 ) / xBinWidth + 1, 0, FFTtimeWindow );
            }

            // 如果当前的时间戳超出了[timestamp_ini, timestamp_ini + timeWindow)的范围
            if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + timeWindow )
            {
                if ( !timestamps_vec.empty() )
                {
                    // 计算时间戳的平均值并打印出来
                    double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
                    double average_Y_6Hz = std::accumulate( Power_6Hz_vec.begin(), Power_6Hz_vec.end(), 0.0 ) / Power_6Hz_vec.size();
                    double average_Y_50Hz = std::accumulate( Power_50Hz_vec.begin(), Power_50Hz_vec.end(), 0.0 ) / Power_50Hz_vec.size();

                    // 计算标准差
                    double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
                    double sum_deviation_Y_6Hz = std::accumulate( Power_6Hz_vec.begin(), Power_6Hz_vec.end(), 0.0, [average_Y_6Hz]( auto sum, auto val ) { return sum + ( val - average_Y_6Hz ) * ( val - average_Y_6Hz ); } );
                    double sum_deviation_Y_50Hz = std::accumulate( Power_50Hz_vec.begin(), Power_50Hz_vec.end(), 0.0, [average_Y_50Hz]( auto sum, auto val ) { return sum + ( val - average_Y_50Hz ) * ( val - average_Y_50Hz ); } );
                    double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
                    double stddev_Y_6Hz = std::sqrt( sum_deviation_Y_6Hz / Power_6Hz_vec.size() );
                    double stddev_Y_50Hz = std::sqrt( sum_deviation_Y_50Hz / Power_50Hz_vec.size() );

                    if ( isDebugModeActive )
                    {
                        std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                        std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                        // 使用TDatime将平均时间戳转换为日期和时间
                        TDatime date( static_cast<UInt_t>( average_X ) );
                        std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                        std::cout << "Average Power of 6 Hz for " << static_cast<int>( timestamp_ini ) << ": " << average_Y_6Hz << std::endl;
                        std::cout << "Standard deviation of Power of 6 Hz for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y_6Hz << std::endl;
                        std::cout << "Average Power of 50 Hz for " << static_cast<int>( timestamp_ini ) << ": " << average_Y_50Hz << std::endl;
                        std::cout << "Standard deviation of Power of 50 Hz for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y_50Hz << std::endl;
                    }

                    if ( average_Y_6Hz != 0 )
                    {
                        x_vec.emplace_back( average_X );
                        y_6Hz_vec.emplace_back( average_Y_6Hz );
                        y_50Hz_vec.emplace_back( average_Y_50Hz );
                        ex_vec.emplace_back( stddev_X );
                        ey_6Hz_vec.emplace_back( stddev_Y_6Hz );
                        ey_50Hz_vec.emplace_back( stddev_Y_50Hz );
                    }
                }

                timestamp_ini = floor( timestamp );

                // 清空时间戳向量
                timestamps_vec.clear();
                Power_6Hz_vec.clear();
                Power_50Hz_vec.clear();
            }

            if ( !std::isnan( Amp ) )
            {
                if ( hist->GetBinContent( hist->GetXaxis()->FindBin( timestamp - timestamp_ini_FFT ) ) == 0 )
                {
                    // 将Amp的值填充到直方图的相应时间窗口中
                    hist->SetBinContent( hist->GetXaxis()->FindBin( timestamp - timestamp_ini_FFT ), Amp );
                }

                // 将当前的时间戳添加到向量中
                timestamps_vec.emplace_back( timestamp );
            }
        }

        // 保存当前的直方图
        if ( hist )
        {
            TH1 * hm = nullptr;
            // 使用FFT计算功率谱
            TVirtualFFT::SetTransform( nullptr );
            hm = hist->FFT( hm, "MAG" );
            if ( !hm )
            {
                std::cerr << "Error: Cannot perform FFT on histogram" << std::endl;
                return;
            }
            std::string FFThistName = "FFT_" + branchName + "_" + std::to_string( FFT_counter ) + "_hist_" + std::to_string( static_cast<int>( timestamp_ini_FFT ) );
            hm->SetName( FFThistName.c_str() );
            hm->SetTitle( ( "FFT for " + std::to_string( static_cast<int>( timestamp_ini_FFT ) ) + "s;Frequency * " + std::to_string( static_cast<int>( FFTtimeWindow ) ) + " [Hz];Power [W/" + to_string_with_precision( 1.0 / FFTtimeWindow, 3 ) + "Hz]" ).c_str() );

            Power_6Hz_vec.emplace_back( hm->GetBinContent( hm->GetXaxis()->FindBin( 6.0 * FFTtimeWindow ) ) );
            Power_50Hz_vec.emplace_back( hm->GetBinContent( hm->GetXaxis()->FindBin( 50.0 * FFTtimeWindow ) ) );
            if ( isDebugModeActive )
            {
                hist->Write();
                hm->Write();
            }

            FFT_counter++;
            delete hist;
        }

        if ( !timestamps_vec.empty() )
        {
            // 计算时间戳的平均值并打印出来
            double average_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();
            double average_Y_6Hz = std::accumulate( Power_6Hz_vec.begin(), Power_6Hz_vec.end(), 0.0 ) / Power_6Hz_vec.size();
            double average_Y_50Hz = std::accumulate( Power_50Hz_vec.begin(), Power_50Hz_vec.end(), 0.0 ) / Power_50Hz_vec.size();

            // 计算标准差
            double sum_deviation_X = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average_X]( auto sum, auto val ) { return sum + ( val - average_X ) * ( val - average_X ); } );
            double sum_deviation_Y_6Hz = std::accumulate( Power_6Hz_vec.begin(), Power_6Hz_vec.end(), 0.0, [average_Y_6Hz]( auto sum, auto val ) { return sum + ( val - average_Y_6Hz ) * ( val - average_Y_6Hz ); } );
            double sum_deviation_Y_50Hz = std::accumulate( Power_50Hz_vec.begin(), Power_50Hz_vec.end(), 0.0, [average_Y_50Hz]( auto sum, auto val ) { return sum + ( val - average_Y_50Hz ) * ( val - average_Y_50Hz ); } );
            double stddev_X = std::sqrt( sum_deviation_X / timestamps_vec.size() );
            double stddev_Y_6Hz = std::sqrt( sum_deviation_Y_6Hz / Power_6Hz_vec.size() );
            double stddev_Y_50Hz = std::sqrt( sum_deviation_Y_50Hz / Power_50Hz_vec.size() );

            if ( isDebugModeActive )
            {
                std::cout << "Average timestamp for " << static_cast<int>( timestamp_ini ) << ": " << average_X << std::endl;
                std::cout << "Standard deviation of timestamp for " << static_cast<int>( timestamp_ini ) << ": " << stddev_X << std::endl;
                // 使用TDatime将平均时间戳转换为日期和时间
                TDatime date( static_cast<UInt_t>( average_X ) );
                std::cout << "Average date and time for " << static_cast<int>( timestamp_ini ) << ": " << date.AsString() << std::endl;
                std::cout << "Average Power of 6 Hz for " << static_cast<int>( timestamp_ini ) << ": " << average_Y_6Hz << std::endl;
                std::cout << "Standard deviation of Power of 6 Hz for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y_6Hz << std::endl;
                std::cout << "Average Power of 50 Hz for " << static_cast<int>( timestamp_ini ) << ": " << average_Y_50Hz << std::endl;
                std::cout << "Standard deviation of Power of 50 Hz for " << static_cast<int>( timestamp_ini ) << ": " << stddev_Y_50Hz << std::endl;
            }

            if ( average_Y_6Hz != 0 )
            {
                x_vec.emplace_back( average_X );
                y_6Hz_vec.emplace_back( average_Y_6Hz );
                y_50Hz_vec.emplace_back( average_Y_50Hz );
                ex_vec.emplace_back( stddev_X );
                ey_6Hz_vec.emplace_back( stddev_Y_6Hz );
                ey_50Hz_vec.emplace_back( stddev_Y_50Hz );
            }

            // 清空时间戳向量
            timestamps_vec.clear();
            Power_6Hz_vec.clear();
            Power_50Hz_vec.clear();
        }

        // 创建TGraphErrors并命名
        TGraphErrors * graph_6Hz = new TGraphErrors( x_vec.size(), x_vec.data(), y_6Hz_vec.data(), ex_vec.data(), ey_6Hz_vec.data() );
        graph_6Hz->SetName( ( branchName + "_6Hz" ).c_str() );
        graph_6Hz->SetTitle( ( branchName + "_6Hz" ).c_str() );

        // 设置点的样式
        graph_6Hz->SetMarkerStyle( MarkerStyle_vec[( i * 2 ) / 8] );  // 设置点的样式
        graph_6Hz->SetMarkerSize( 0.7 );  // 设置点的大小
        // 设置TGraphErrors的颜色
        graph_6Hz->SetMarkerColor( Color_vec[( i * 2 ) % 8] );
        graph_6Hz->SetLineWidth( 1 );
        graph_6Hz->SetFillStyle( 0 );

        // 创建TGraphErrors并命名
        TGraphErrors * graph_50Hz = new TGraphErrors( x_vec.size(), x_vec.data(), y_50Hz_vec.data(), ex_vec.data(), ey_50Hz_vec.data() );
        graph_50Hz->SetName( ( branchName + "_50Hz" ).c_str() );
        graph_50Hz->SetTitle( ( branchName + "_50Hz" ).c_str() );

        // 设置点的样式
        graph_50Hz->SetMarkerStyle( MarkerStyle_vec[( i * 2 + 1 ) / 8] );  // 设置点的样式
        graph_50Hz->SetMarkerSize( 0.7 );  // 设置点的大小
        // 设置TGraphErrors的颜色
        graph_50Hz->SetMarkerColor( Color_vec[( i * 2 + 1 ) % 8] );
        graph_50Hz->SetLineWidth( 1 );
        graph_50Hz->SetFillStyle( 0 );

        graphs_vec[i * 2] = graph_6Hz;
        graphs_vec[i * 2 + 1] = graph_50Hz;
    }

    // The time offset is the one that will be used by all graphs.
    // If one changes it, it will be changed even on the graphs already defined
    gStyle->SetTimeOffset( 0 );

    // 创建一个TCanvas
    TCanvas * canvas = new TCanvas( "canvas_FFT", "canvas_FFT", 1600, 900 );

    // 开启X轴和Y轴的网格线
    canvas->SetGridx();
    canvas->SetGridy();

    // 创建一个MultiGraph
    TMultiGraph * multiGraph = new TMultiGraph( "mg_FFT", "6 Hz and 50 Hz noise components of each channel" );

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
    multiGraph->GetYaxis()->SetTitle( ( "Power [W/" + to_string_with_precision( 1.0 / FFTtimeWindow, 3 ) + "Hz]" ).c_str() );
    multiGraph->GetXaxis()->SetRangeUser( xTimeStampMin - 60, xTimeStampMax + 60 );
    multiGraph->GetXaxis()->SetTimeDisplay( 1 );
    // 设置X轴上的时间格式
    //multiGraph->GetXaxis()->SetTimeFormat( "%a %d/%m/%y %H:%M:%S" );
    multiGraph->GetXaxis()->SetTimeFormat( "%m/%d %H:%M:%S" );

    // 创建一个图例
    canvas->BuildLegend();

    // 将TCanvas写入文件
    canvas->Write( "canvas_FFT" );

    outputFile->Close();

    delete outputFile;
    delete file;
}

std::string TTreePlotter::to_string_with_precision( const double a_value, const int n = 6 ) const
{
    int nn = n;
    std::ostringstream out;
    out << std::fixed << std::setprecision( nn ) << a_value;
    return out.str();
}
}

