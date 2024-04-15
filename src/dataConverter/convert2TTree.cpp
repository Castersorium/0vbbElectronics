#include <iostream> // C++ header
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TDatime.h>

#include "convert2TTree.hpp" // my header

namespace TTREEIO
{
// ����һ�����������ڽ�CSV��ʽ������ת��ΪTTree��ʽ
void convert2TTree::convertCSV2TTree( const std::string & csvDirPath, const std::string & rootFileName )
{
    // ����һ���µ�ROOT�ļ�
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // ����һ���µ�TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // ���isDebugModeActive״̬Ϊtrue����ӡ��һ����Ϣ
    if ( isDebugModeActive )
    {
        std::cout << " - Input CSV directory read: " << csvDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // ����һ���������洢ʱ���
    double timestamp = 0.0;
    double timestampOffset = 0.0;

    // ����ʱ���branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );

    // ����Ŀ¼�µ������ļ�
    for ( const auto & entry : std::filesystem::directory_iterator( csvDirPath ) )
    {
        std::string csvFilePath = entry.path().string();

        // ��CSV�ļ�
        std::ifstream csvFile( csvFilePath );
        if ( !csvFile.is_open() )
        {
            std::cerr << "Can not open csv file: " << csvFilePath << std::endl;
            continue;
        }

        if ( isDebugModeActive )
        {
            std::cout << " - Input CSV file read: " << csvFilePath << std::endl;
        }

        // ��ȡ��һ����ȷ��ͨ����
        std::string line;
        std::getline( csvFile, line );
        int commaCount = std::count( line.begin(), line.end(), ',' );
        if ( commaCount == 0 )
        {
            std::cerr << "Error: CSV file format is incorrect" << std::endl;
            continue;
        }
        std::istringstream ssDate( line );
        std::string date_string;
        std::getline( ssDate, date_string, ',' ); // ����"ʱ���ʶ"
        std::getline( ssDate, date_string, ',' ); // ��ȡ���ں�ʱ��

        // �����ں�ʱ���ַ����ĸ�ʽת��Ϊ"yyyy-mm-dd hh:mm:ss"
        std::replace( date_string.begin(), date_string.end(), '/', '-' );

        TDatime datime( date_string.c_str() );
        timestampOffset = datime.Convert();

        if ( isDebugModeActive )
        {
            std::cout << " - Initial date_string: " << date_string << std::endl;
            std::cout << " - Initial TDatime: " << datime.AsString() << std::endl;
            std::cout << " - Initial timestampOffset: " << timestampOffset << std::endl;
        }

        // ȷ��ͨ����
        int channelCount = commaCount / 2 + 1;

        // �����ڶ���
        csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

        // ��ȡ����������ȡͨ������
        std::getline( csvFile, line );
        std::istringstream ssCh( line );
        std::vector<std::string> channelNames( channelCount );
        for ( int i = 0; i < channelCount; ++i )
        {
            std::getline( ssCh, channelNames[i], ',' );
            if ( i == channelCount - 1 )
            {
                // ����������е�ĩβ��ֱ�Ӷ�ȡʣ��������ַ�
                std::getline( ssCh, channelNames[i] );
                // ��鲢ȥ�����з�
                if ( !channelNames[i].empty() && ( channelNames[i].back() == '\n' || channelNames[i].back() == '\r' ) )
                {
                    channelNames[i].pop_back();
                }
            }
            else
            {
                std::getline( ssCh, channelNames[i], ',' );
            }
            // ȥ��˫����
            channelNames[i] = channelNames[i].substr( 1, channelNames[i].size() - 2 );
        }

        // �����������洢branch��ֵ
        std::vector<double> amplitudes( channelCount, 0.0 );

        // ����branch
        for ( int i = 0; i < channelCount; ++i )
        {
            // ���branch�Ƿ��Ѿ�����
            TBranch * branch = tree->GetBranch( channelNames[i].c_str() );
            if ( branch == nullptr )
            {
                // ���branch�����ڣ������µ�branch
                tree->Branch( channelNames[i].c_str(), &amplitudes[i], ( channelNames[i] + "/D" ).c_str() );
            }
        }

        // ����ʣ���ǰ����
        for ( int i = 0; i < 1; ++i )
        {
            csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        // ��ȡCSV�ļ�������
        while ( std::getline( csvFile, line ) )
        {
            std::istringstream ssAmp( line );
            std::string field;

            // ��ȡʱ���
            std::getline( ssAmp, field, ',' );
            timestamp = std::stod( field ) + timestampOffset;

            // ��ȡÿ��ͨ���ķ���
            for ( int i = 0; i < channelCount; ++i )
            {
                std::getline( ssAmp, field, ',' );
                amplitudes[i] = std::stod( field );

                // ������һ��ʱ���
                std::getline( ssAmp, field, ',' );
            }

            // ���TTree
            tree->Fill();
        }
    }

    // ��TTreeд�뵽ROOT�ļ���
    tree->Write();

    // �ر�ROOT�ļ�
    delete file;
}
}
