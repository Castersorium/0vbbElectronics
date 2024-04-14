#include <iostream> // C++ header
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header

namespace TTREEIO
{
// ����һ�����������ڽ�CSV��ʽ������ת��ΪTTree��ʽ
void convert2TTree::convertCSV2TTree( const std::string csvFileName, const std::string rootFileName )
{
    // ����һ���µ�ROOT�ļ�
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // ����һ���µ�TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // ���isDebugModeActive״̬Ϊtrue����ӡ��һ����Ϣ
    if ( isDebugModeActive )
    {

        std::cout << " - Input csv file read: " << csvFileName << std::endl;
        std::cout << " - Output root file created: " << rootFileName << std::endl;
    }

    // ��CSV�ļ�
    std::ifstream csvFile( csvFileName );
    if ( !csvFile.is_open() )
    {
        std::cerr << "Can not open csv file: " << csvFileName << std::endl;
        return;
    }

    
    // ��ȡ��һ����ȷ��ͨ����
    std::string line;
    std::getline( csvFile, line );
    int commaCount = std::count( line.begin(), line.end(), ',' );
    if ( commaCount == 0 )
    {
        std::cerr << "Error: CSV file format is incorrect" << std::endl;
        return;
    }
    int channelCount = commaCount / 2 + 1;

    // �����������洢branch��ֵ
    std::vector<double> amplitudes( channelCount, 0.0 );

    // ����branch
    for ( int i = 0; i < channelCount; ++i )
    {
        tree->Branch( ( "Amp" + std::to_string( i ) ).c_str(), &amplitudes[i], ( "Amp" + std::to_string( i ) + "/D" ).c_str() );
    }

    // ����ʣ���ǰ����
    for ( int i = 0; i < 3; ++i )
    {
        csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
    }

    // ��ȡCSV�ļ�������
    while ( std::getline( csvFile, line ) )
    {
        std::istringstream ss( line );
        std::string field;

        // ��ȡʱ���
        std::getline( ss, field, ',' );
        double timestamp = std::stod( field );

        // ��ȡÿ��ͨ���ķ���
        for ( int i = 0; i < channelCount; ++i )
        {
            std::getline( ss, field, ',' );
            amplitudes[i] = std::stod( field );

            // ������һ��ʱ���
            std::getline( ss, field, ',' );
        }

        // ���TTree
        tree->Fill();
    }

    // ��TTreeд�뵽ROOT�ļ���
    tree->Write();

    // �ر�ROOT�ļ�
    delete file;
}
}
