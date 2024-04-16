#include <iostream> // C++ header
#include <cmath>

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
    double * x = new double[n];
    double * y = new double[n];
    double * ex = new double[n];  // x轴误差
    double * ey = new double[n];  // y轴误差

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

    TH1D * hist = nullptr;  // 当前的直方图
    double timestamp_ini = -1.0;  // 初始的时间戳

    // 遍历TTree中的每个entry
    for ( int i = 0; i < n; ++i )
    {
        tree->GetEntry( i );

        // 如果当前的时间戳超出了[timestamp_ini, timestamp_ini+1)的范围
        if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + 1 )
        {
            // 保存当前的直方图
            if ( hist )
            {
                hist->Write();
                delete hist;
            }

            // 创建一个新的直方图
            timestamp_ini = floor( timestamp );
            std::string histName = "hist_" + std::to_string( static_cast<int>( timestamp_ini ) );
            hist = new TH1D( histName.c_str(), ( "Amplitude for " + std::to_string( static_cast<int>( timestamp_ini ) ) + "s;Amplitude [V];counts" ).c_str(), 1000, -10, 10 );
        }

        // 将Amp_2cmLMO的值填充到直方图的相应时间窗口中
        hist->Fill( Amp_2cmLMO );
    }

    // 保存最后一个直方图
    if ( hist )
    {
        hist->Write();
        delete hist;
    }

    timestamp = 0;
    Amp_2cmLMO = 0;

    for ( int i = 0; i < n; ++i )
    {
        tree->GetEntry( i );
        x[i] = timestamp;
        y[i] = Amp_2cmLMO;
        // 假设x轴没有误差
        ex[i] = 0;
        // 假设y轴的误差是y的10%
        ey[i] = y[i] * 0.1;
    }

    // 创建TGraphErrors
    TGraphErrors * graph = new TGraphErrors( n, x, y, ex, ey );

    // 注意：在使用完x和y后，记得删除它们
    delete[] x;
    delete[] y;
    delete[] ex;
    delete[] ey;

    graph->Write( "graph" );
    outputFile->Close();

    delete outputFile;
    delete file;
}
}
