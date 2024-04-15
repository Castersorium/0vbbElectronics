#include <iostream> // C++ header

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TGraphErrors.h>

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

    // ��TTree����TGraphErrors
    TGraphErrors * graph = new TGraphErrors( tree );

    // ��TGraphErrors���浽�µ�ROOT�ļ�
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
