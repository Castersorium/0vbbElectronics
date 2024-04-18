#include <iostream> // C++ header
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TDatime.h>

#include "convert2TTree.hpp" // my header

namespace TTREEIO
{
// ����һ�����������ڽ�NIDAQCSV��ʽ������ת��ΪTTree��ʽ
void convert2TTree::convertNIDAQCSV2TTree( const std::string & csvDirPath, const std::string & rootFileName ) const
{
    // ����һ���µ�ROOT�ļ�
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // ����һ���µ�TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // ���isDebugModeActive״̬Ϊtrue����ӡ��һ����Ϣ
    if ( isDebugModeActive )
    {
        std::cout << " - Input NI DAQ CSV directory read: " << csvDirPath << std::endl;
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

// ����һ�����������ڽ�BlueforsLog��ʽ������ת��ΪTTree��ʽ
void convert2TTree::convertBlueforsTemperatureLog2TTree( const std::string & BlueforsLogDirPath, const std::string & rootFileName ) const
{
    // ����һ���µ�ROOT�ļ�
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // ����һ���µ�TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // ���isDebugModeActive״̬Ϊtrue����ӡ��һ����Ϣ
    if ( isDebugModeActive )
    {
        std::cout << " - Input BLUEFORS_LOG directory read: " << BlueforsLogDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // ����һ���������洢ʱ���
    double timestamp = 0.0;
    double temperature_1 = 0.0;
    double temperature_2 = 0.0;
    double temperature_5 = 0.0;
    double temperature_6 = 0.0;
    double temperature_7 = 0.0;

    // ����ʱ���branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );
    tree->Branch( "T1", &temperature_1, "T1_50K/D" );
    tree->Branch( "T2", &temperature_2, "T2_4K/D" );
    tree->Branch( "T5", &temperature_5, "T5_STILL/D" );
    tree->Branch( "T6", &temperature_6, "T6_MXC/D" );
    tree->Branch( "T7", &temperature_7, "T7_SAMPLE/D" );

    std::vector<std::string> logFileString_vec = { "CH1 T ", "CH2 T ", "CH5 T ", "CH6 T ", "CH7 T " };
    std::vector<std::string> logFileName_vec = {};
    std::vector<std::ifstream> logFile_vec = {};
    std::vector<std::string> line_vec = {};

    // ����Ŀ¼�µ����������ļ���
    for ( const auto & entry : std::filesystem::directory_iterator( BlueforsLogDirPath ) )
    {
        std::string dateDir = entry.path().filename().string();
        if ( dateDir < startDate || dateDir > endDate )
        {
            if ( isDebugModeActive )
            {
                std::cout << " - Skip date directory: " << dateDir << std::endl;
            }
            continue;  // ������Χ֮�������
        }

        for ( const auto & logFileString : logFileString_vec )
        {
            logFileName_vec.emplace_back( BlueforsLogDirPath + "/" + dateDir + "/" + logFileString + dateDir + ".log" );
            if ( isDebugModeActive )
            {
                std::cout << " - Input BLUEFORS_LOG file set: " << logFileName_vec.back() << std::endl;
            }
        }

        // ��CSV�ļ�
        for ( const auto & logFileName : logFileName_vec )
        {
            logFile_vec.emplace_back( logFileName );
            if ( !logFile_vec.back().is_open() )
            {
                std::cerr << "Can not open log file: " << logFileName << std::endl;
                logFile_vec.pop_back();
                continue;
            }

            if ( isDebugModeActive )
            {
                std::cout << " - Input BLUEFORS_LOG file read: " << logFileName << std::endl;
            }

            line_vec.emplace_back( "" );
        }

        // ��ȡCSV�ļ�������
        while ( std::getline( logFile_vec[0], line_vec[0] )
                && std::getline( logFile_vec[1], line_vec[1] )
                && std::getline( logFile_vec[2], line_vec[2] )
                && std::getline( logFile_vec[3], line_vec[3] )
                && std::getline( logFile_vec[4], line_vec[4] )
                )
        {
            std::vector<std::istringstream> ss_vec( logFile_vec.size() );
            std::vector<std::string> date_vec( logFile_vec.size() );
            std::vector<std::string> time_vec( logFile_vec.size() );
            std::vector<std::string> field_vec( logFile_vec.size() );
            for ( int i = 0; i < logFile_vec.size(); ++i )
            {
                ss_vec[i] = std::istringstream( line_vec[i] );
                std::getline( ss_vec[i], date_vec[i], ',' );  // ��ȡ����

                // ȥ���ַ�����˵Ŀո�
                date_vec[i].erase( date_vec[i].begin(), std::find_if( date_vec[i].begin(), date_vec[i].end(), []( auto ch ) { return !std::isspace( ch ); } ) );

                std::getline( ss_vec[i], time_vec[i], ',' );  // ��ȡʱ��
                // ��ȡ�¶�
                // ����������е�ĩβ��ֱ�Ӷ�ȡʣ��������ַ�
                std::getline( ss_vec[i], field_vec[i] );
                // ��鲢ȥ�����з�
                if ( !field_vec[i].empty() && ( field_vec[i].back() == '\n' || field_vec[i].back() == '\r' ) )
                {
                    field_vec[i].pop_back();
                }
            }

            if ( isDebugModeActive )
            {
                std::cout << " - date_string: " << date_vec[0] << std::endl;
                std::cout << " - time_string: " << time_vec[0] << std::endl;
            }

            // �������ַ���ת��Ϊ"YYYY-MM-DD"�ĸ�ʽ
            for ( int i = 0; i < logFile_vec.size(); ++i )
            {
                date_vec[i] = "20" + date_vec[i].substr( 6, 2 ) + "-" + date_vec[i].substr( 3, 2 ) + "-" + date_vec[i].substr( 0, 2 );
            }

            // ����TDatime����vector
            std::vector<TDatime> datetime_vec( logFile_vec.size() );
            for ( int i = 0; i < logFile_vec.size(); ++i )
            {
                datetime_vec[i] = TDatime( ( date_vec[i] + " " + time_vec[i] ).c_str() );
            }

            timestamp = datetime_vec[0].Convert();

            if ( isDebugModeActive )
            {
                std::cout << " - date_string: " << date_vec[0] + " " + time_vec[0] << std::endl;
                std::cout << " - TDatime: " << datetime_vec[0].AsString() << std::endl;
                std::cout << " - timestamp: " << timestamp << std::endl;
            }

            for ( int i = 1; i < logFile_vec.size(); ++i )
            {
                if ( datetime_vec[i].Convert() - timestamp > 60.0 || datetime_vec[i].Convert() - timestamp < -60.0 )
                {
                    std::clog << "Warning: Date and time are not consistent: " << datetime_vec[0].AsString() << " vs. " << datetime_vec[i].AsString() << " in " << logFileName_vec[0] << " and " << logFileName_vec[i] << std::endl;
                }
            }

            temperature_1 = std::stod( field_vec[0] );
            temperature_2 = std::stod( field_vec[1] );
            temperature_5 = std::stod( field_vec[2] );
            temperature_6 = std::stod( field_vec[3] );
            temperature_7 = std::stod( field_vec[4] );

            if ( isDebugModeActive )
            {
                std::cout << " - temperature_1: " << temperature_1 << std::endl;
                std::cout << " - temperature_2: " << temperature_2 << std::endl;
                std::cout << " - temperature_5: " << temperature_5 << std::endl;
                std::cout << " - temperature_6: " << temperature_6 << std::endl;
                std::cout << " - temperature_7: " << temperature_7 << std::endl;
            }

            // ���TTree
            tree->Fill();
        }

        //    std::istringstream ssDate( line );
        //    std::string date_string;
        //    std::getline( ssDate, date_string, ',' ); // ����"ʱ���ʶ"
        //    std::getline( ssDate, date_string, ',' ); // ��ȡ���ں�ʱ��

        //    // �����ں�ʱ���ַ����ĸ�ʽת��Ϊ"yyyy-mm-dd hh:mm:ss"
        //    std::replace( date_string.begin(), date_string.end(), '/', '-' );

        //    TDatime datime( date_string.c_str() );
        //    timestampOffset = datime.Convert();

        //    if ( isDebugModeActive )
        //    {
        //        std::cout << " - Initial date_string: " << date_string << std::endl;
        //        std::cout << " - Initial TDatime: " << datime.AsString() << std::endl;
        //        std::cout << " - Initial timestampOffset: " << timestampOffset << std::endl;
        //    }

        //    // ȷ��ͨ����
        //    int channelCount = commaCount / 2 + 1;

        //    // �����ڶ���
        //    csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

        //    // ��ȡ����������ȡͨ������
        //    std::getline( csvFile, line );
        //    std::istringstream ssCh( line );
        //    std::vector<std::string> channelNames( channelCount );
        //    for ( int i = 0; i < channelCount; ++i )
        //    {
        //        std::getline( ssCh, channelNames[i], ',' );
        //        if ( i == channelCount - 1 )
        //        {
        //            // ����������е�ĩβ��ֱ�Ӷ�ȡʣ��������ַ�
        //            std::getline( ssCh, channelNames[i] );
        //            // ��鲢ȥ�����з�
        //            if ( !channelNames[i].empty() && ( channelNames[i].back() == '\n' || channelNames[i].back() == '\r' ) )
        //            {
        //                channelNames[i].pop_back();
        //            }
        //        }
        //        else
        //        {
        //            std::getline( ssCh, channelNames[i], ',' );
        //        }
        //        // ȥ��˫����
        //        channelNames[i] = channelNames[i].substr( 1, channelNames[i].size() - 2 );
        //    }

        //    // �����������洢branch��ֵ
        //    std::vector<double> amplitudes( channelCount, 0.0 );

        //    // ����branch
        //    for ( int i = 0; i < channelCount; ++i )
        //    {
        //        // ���branch�Ƿ��Ѿ�����
        //        TBranch * branch = tree->GetBranch( channelNames[i].c_str() );
        //        if ( branch == nullptr )
        //        {
        //            // ���branch�����ڣ������µ�branch
        //            tree->Branch( channelNames[i].c_str(), &amplitudes[i], ( channelNames[i] + "/D" ).c_str() );
        //        }
        //    }

        //    // ����ʣ���ǰ����
        //    for ( int i = 0; i < 1; ++i )
        //    {
        //        csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        //    }

        //    // ��ȡCSV�ļ�������
        //    while ( std::getline( csvFile, line ) )
        //    {
        //        std::istringstream ssAmp( line );
        //        std::string field;

        //        // ��ȡʱ���
        //        std::getline( ssAmp, field, ',' );
        //        timestamp = std::stod( field ) + timestampOffset;

        //        // ��ȡÿ��ͨ���ķ���
        //        for ( int i = 0; i < channelCount; ++i )
        //        {
        //            std::getline( ssAmp, field, ',' );
        //            amplitudes[i] = std::stod( field );

        //            // ������һ��ʱ���
        //            std::getline( ssAmp, field, ',' );
        //        }

        //        // ���TTree
        //        tree->Fill();
        //    }
    }

    // ��TTreeд�뵽ROOT�ļ���
    tree->Write();

    // �ر�ROOT�ļ�
    delete file;
}
}
