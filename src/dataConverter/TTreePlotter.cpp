#include <iostream> // C++ header

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TGraphErrors.h>

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

    // 将TGraphErrors保存到新的ROOT文件
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    graph->Write( "graph" );
    outputFile->Close();

    delete outputFile;
    delete file;
}
}
