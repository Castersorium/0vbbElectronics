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
// ��TTree����TGraphErrors�����浽ROOT�ļ��ķ���
void TTreePlotter::createGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath )
{
    // ��ROOT�ļ�
    TFile * file = TFile::Open( rootFilePath.c_str(), "READ" );
    if ( !file || file->IsZombie() )
    {
        std::cerr << "Error: Cannot open ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // ���ļ��л�ȡTTree
    TTree * tree = dynamic_cast<TTree *>( file->Get( "tree" ) );
    if ( !tree )
    {
        std::cerr << "Error: Cannot find TTree in ROOT file: " << rootFilePath << std::endl;
        return;
    }

    // ��TTree�л�ȡ����
    int n = tree->GetEntries();
    double * x = new double[n];
    double * y = new double[n];
    double * ex = new double[n];  // x�����
    double * ey = new double[n];  // y�����

    double timestamp = 0;
    double Amp_2cmLMO = 0;

    tree->SetBranchAddress( "timestamp", &timestamp );
    tree->SetBranchAddress( "2cmLMO", &Amp_2cmLMO );

    // ��TGraphErrors���浽�µ�ROOT�ļ�
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    TH1D * hist = nullptr;  // ��ǰ��ֱ��ͼ
    double timestamp_ini = -1.0;  // ��ʼ��ʱ���

    // ����TTree�е�ÿ��entry
    for ( int i = 0; i < n; ++i )
    {
        tree->GetEntry( i );

        // �����ǰ��ʱ���������[timestamp_ini, timestamp_ini+1)�ķ�Χ
        if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + 1 )
        {
            // ���浱ǰ��ֱ��ͼ
            if ( hist )
            {
                hist->Write();
                delete hist;
            }

            // ����һ���µ�ֱ��ͼ
            timestamp_ini = floor( timestamp );
            std::string histName = "hist_" + std::to_string( static_cast<int>( timestamp_ini ) );
            hist = new TH1D( histName.c_str(), ( "Amplitude for " + std::to_string( static_cast<int>( timestamp_ini ) ) + "s;Amplitude [V];counts" ).c_str(), 1000, -10, 10 );
        }

        // ��Amp_2cmLMO��ֵ��䵽ֱ��ͼ����Ӧʱ�䴰����
        hist->Fill( Amp_2cmLMO );
    }

    // �������һ��ֱ��ͼ
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
        // ����x��û�����
        ex[i] = 0;
        // ����y��������y��10%
        ey[i] = y[i] * 0.1;
    }

    // ����TGraphErrors
    TGraphErrors * graph = new TGraphErrors( n, x, y, ex, ey );

    // ע�⣺��ʹ����x��y�󣬼ǵ�ɾ������
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
