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

#include "TTreePlotter.hpp" // my header

namespace TTREEIO
{
// ��TTree����TGraphErrors�����浽ROOT�ļ��ķ���
void TTreePlotter::createGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const
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

    // ��ȡTTree�е�����branch������
    TObjArray * branches = tree->GetListOfBranches();
    int nBranches = branches->GetEntries();

    // ��TGraphErrors���浽�µ�ROOT�ļ�
    TFile * outputFile = TFile::Open( outputFilePath.c_str(), "RECREATE" );
    if ( !outputFile || outputFile->IsZombie() )
    {
        std::cerr << "Error: Cannot create ROOT file:" << outputFilePath << std::endl;
        return;
    }

    // Ϊÿ��branch����һ��TGraphErrors
    std::vector<TGraphErrors *> graphs_vec( nBranches );

    for ( int i = 0; i < nBranches; ++i )
    {
        TBranch * branch = static_cast<TBranch *>( branches->At( i ) );
        std::string branchName = branch->GetName();

        // ����timestamp���branch
        if ( branchName == "timestamp" )
            continue;

        // ��TTree�л�ȡ����
        int n = tree->GetEntries();

        std::vector <double> x_vec = {};
        std::vector <double> y_vec = {};
        std::vector <double> ex_vec = {};  // x�����
        std::vector <double> ey_vec = {};  // y�����

        double timestamp = 0;
        double Amp = 0;

        tree->SetBranchAddress( "timestamp", &timestamp );
        tree->SetBranchAddress( branchName.c_str(), &Amp );

        // ����һ���������洢ÿ��ʱ�䴰���ڵ�����ʱ���
        std::vector<double> timestamps_vec;
        TH1D * hist = nullptr;  // ��ǰ��ֱ��ͼ
        double timestamp_ini = -1.0;  // ��ʼ��ʱ���

        // ����TTree�е�ÿ��entry
        for ( int i = 0; i < n; ++i )
        {
            tree->GetEntry( i );

            // �����ǰ��ʱ���������[timestamp_ini, timestamp_ini + timeWindow)�ķ�Χ
            if ( timestamp < timestamp_ini || timestamp >= timestamp_ini + timeWindow )
            {
                // ���浱ǰ��ֱ��ͼ
                if ( hist )
                {
                    // ����ʱ�����ƽ��ֵ����ӡ����
                    double average = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();

                    // �����׼��
                    double sum_deviation = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average]( double sum, double val ) { return sum + ( val - average ) * ( val - average ); } );
                    double stddev = std::sqrt( sum_deviation / timestamps_vec.size() );

                    // �ҵ�ֱ��ͼ�����bin��x��ֵ����ӡ����
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
                        // ʹ��TDatime��ƽ��ʱ���ת��Ϊ���ں�ʱ��
                        TDatime date( static_cast<UInt_t>( average ) );
                        std::cout << "Average date and time for " << hist->GetName() << ": " << date.AsString() << std::endl;
                        std::cout << "Mode amplitude for " << hist->GetName() << ": " << maxX << std::endl;
                        hist->Write();
                    }
                    delete hist;
                }

                // ����һ���µ�ֱ��ͼ
                timestamp_ini = floor( timestamp );
                std::string histName = "hist_" + std::to_string( static_cast<int>( timestamp_ini ) );
                hist = new TH1D( histName.c_str(), ( "Amplitude for " + std::to_string( static_cast<int>( timestamp_ini ) ) + "s;Amplitude [V];counts" ).c_str(), ( xMax - xMin ) / xBinWidth, xMin, xMax );
                // ���ʱ�������
                timestamps_vec.clear();
            }

            // ��Amp_2cmLMO��ֵ��䵽ֱ��ͼ����Ӧʱ�䴰����
            hist->Fill( Amp );

            // ����ǰ��ʱ�����ӵ�������
            timestamps_vec.emplace_back( timestamp );
        }

        // �������һ��ֱ��ͼ
        if ( hist )
        {
            // ����ʱ�����ƽ��ֵ����ӡ����
            double average = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0 ) / timestamps_vec.size();

            // �����׼��
            double sum_deviation = std::accumulate( timestamps_vec.begin(), timestamps_vec.end(), 0.0, [average]( double sum, double val ) { return sum + ( val - average ) * ( val - average ); } );
            double stddev = std::sqrt( sum_deviation / timestamps_vec.size() );

            // �ҵ�ֱ��ͼ�����bin��x��ֵ����ӡ����
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
                // ʹ��TDatime��ƽ��ʱ���ת��Ϊ���ں�ʱ��
                TDatime date( static_cast<UInt_t>( average ) );
                std::cout << "Average date and time for " << hist->GetName() << ": " << date.AsString() << std::endl;
                std::cout << "Mode amplitude for " << hist->GetName() << ": " << maxX << std::endl;
                hist->Write();
            }
            delete hist;
        }

        // ����TGraphErrors������
        TGraphErrors * graph = new TGraphErrors( x_vec.size(), x_vec.data(), y_vec.data(), ex_vec.data(), ey_vec.data() );
        graph->SetName( branchName.c_str() );

        // ���õ����ʽ����ɫ
        graph->SetMarkerStyle( 21 );  // ���õ����ʽΪ������
        graph->SetMarkerColor( kBlue );  // ���õ����ɫΪ��ɫ

        // �����Ƶ�֮�������
        // graph->SetLineStyle( 0 );
        graph->SetDrawOption( "EP" );

        graphs_vec[i] = graph;
    }

    // ����һ��MultiGraph
    TMultiGraph * multiGraph = new TMultiGraph( "mg", "mg" );

    // �����е�TGraphErrors��ӵ�MultiGraph��
    for ( TGraphErrors * graphInstance : graphs_vec )
    {
        if ( !graphInstance )
            continue;

        if ( isDebugModeActive )
        {
            std::cout << "Adding graph " << graphInstance->GetName() << " to MultiGraph" << std::endl;
        }

        // ����TGraph�Ļ�ͼѡ��Ϊ"P"
        graphInstance->SetDrawOption( "EP" );

        multiGraph->Add( graphInstance );
    }

    multiGraph->SetDrawOption( "AEP" );

    // ��MultiGraphд���ļ�
    multiGraph->Write( "multiGraph" );

    outputFile->Close();

    delete outputFile;
    delete file;
}
}
