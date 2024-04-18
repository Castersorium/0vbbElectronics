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
// 定义一个函数，用于将NIDAQCSV格式的数据转换为TTree格式
void convert2TTree::convertNIDAQCSV2TTree( const std::string & csvDirPath, const std::string & rootFileName ) const
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {
        std::cout << " - Input NI DAQ CSV directory read: " << csvDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // 创建一个变量来存储时间戳
    double timestamp = 0.0;
    double timestampOffset = 0.0;

    // 创建时间戳branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );

    // 遍历目录下的所有文件
    for ( const auto & entry : std::filesystem::directory_iterator( csvDirPath ) )
    {
        std::string csvFilePath = entry.path().string();

        // 打开CSV文件
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

        // 读取第一行来确定通道数
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
        std::getline( ssDate, date_string, ',' ); // 跳过"时间标识"
        std::getline( ssDate, date_string, ',' ); // 获取日期和时间

        // 将日期和时间字符串的格式转换为"yyyy-mm-dd hh:mm:ss"
        std::replace( date_string.begin(), date_string.end(), '/', '-' );

        TDatime datime( date_string.c_str() );
        timestampOffset = datime.Convert();

        if ( isDebugModeActive )
        {
            std::cout << " - Initial date_string: " << date_string << std::endl;
            std::cout << " - Initial TDatime: " << datime.AsString() << std::endl;
            std::cout << " - Initial timestampOffset: " << timestampOffset << std::endl;
        }

        // 确定通道数
        int channelCount = commaCount / 2 + 1;

        // 跳过第二行
        csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

        // 读取第三行来获取通道名称
        std::getline( csvFile, line );
        std::istringstream ssCh( line );
        std::vector<std::string> channelNames( channelCount );
        for ( int i = 0; i < channelCount; ++i )
        {
            std::getline( ssCh, channelNames[i], ',' );
            if ( i == channelCount - 1 )
            {
                // 如果到达了行的末尾，直接读取剩余的所有字符
                std::getline( ssCh, channelNames[i] );
                // 检查并去掉换行符
                if ( !channelNames[i].empty() && ( channelNames[i].back() == '\n' || channelNames[i].back() == '\r' ) )
                {
                    channelNames[i].pop_back();
                }
            }
            else
            {
                std::getline( ssCh, channelNames[i], ',' );
            }
            // 去掉双引号
            channelNames[i] = channelNames[i].substr( 1, channelNames[i].size() - 2 );
        }

        // 创建变量来存储branch的值
        std::vector<double> amplitudes( channelCount, 0.0 );

        // 创建branch
        for ( int i = 0; i < channelCount; ++i )
        {
            // 检查branch是否已经存在
            TBranch * branch = tree->GetBranch( channelNames[i].c_str() );
            if ( branch == nullptr )
            {
                // 如果branch不存在，创建新的branch
                tree->Branch( channelNames[i].c_str(), &amplitudes[i], ( channelNames[i] + "/D" ).c_str() );
            }
        }

        // 跳过剩余的前四行
        for ( int i = 0; i < 1; ++i )
        {
            csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        // 读取CSV文件的数据
        while ( std::getline( csvFile, line ) )
        {
            std::istringstream ssAmp( line );
            std::string field;

            // 获取时间戳
            std::getline( ssAmp, field, ',' );
            timestamp = std::stod( field ) + timestampOffset;

            // 获取每个通道的幅度
            for ( int i = 0; i < channelCount; ++i )
            {
                std::getline( ssAmp, field, ',' );
                amplitudes[i] = std::stod( field );

                // 跳过下一个时间戳
                std::getline( ssAmp, field, ',' );
            }

            // 填充TTree
            tree->Fill();
        }
    }

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}

// 定义一个函数，用于将BlueforsLog格式的数据转换为TTree格式
void convert2TTree::convertBlueforsTemperatureLog2TTree( const std::string & BlueforsLogDirPath, const std::string & rootFileName ) const
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {
        std::cout << " - Input BLUEFORS_LOG directory read: " << BlueforsLogDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // 创建一个变量来存储时间戳
    double timestamp = 0.0;
    double temperature_1 = 0.0;
    double temperature_2 = 0.0;
    double temperature_5 = 0.0;
    double temperature_6 = 0.0;
    double temperature_7 = 0.0;

    // 创建时间戳branch
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

    // 遍历目录下的所有日期文件夹
    for ( const auto & entry : std::filesystem::directory_iterator( BlueforsLogDirPath ) )
    {
        std::string dateDir = entry.path().filename().string();
        if ( dateDir < startDate || dateDir > endDate )
        {
            if ( isDebugModeActive )
            {
                std::cout << " - Skip date directory: " << dateDir << std::endl;
            }
            continue;  // 跳过范围之外的日期
        }

        for ( const auto & logFileString : logFileString_vec )
        {
            logFileName_vec.emplace_back( BlueforsLogDirPath + "/" + dateDir + "/" + logFileString + dateDir + ".log" );
            if ( isDebugModeActive )
            {
                std::cout << " - Input BLUEFORS_LOG file set: " << logFileName_vec.back() << std::endl;
            }
        }

        // 打开CSV文件
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

        // 读取CSV文件的数据
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
                std::getline( ss_vec[i], date_vec[i], ',' );  // 获取日期

                // 去除字符串左端的空格
                date_vec[i].erase( date_vec[i].begin(), std::find_if( date_vec[i].begin(), date_vec[i].end(), []( auto ch ) { return !std::isspace( ch ); } ) );

                std::getline( ss_vec[i], time_vec[i], ',' );  // 获取时间
                // 获取温度
                // 如果到达了行的末尾，直接读取剩余的所有字符
                std::getline( ss_vec[i], field_vec[i] );
                // 检查并去掉换行符
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

            // 将日期字符串转换为"YYYY-MM-DD"的格式
            for ( int i = 0; i < logFile_vec.size(); ++i )
            {
                date_vec[i] = "20" + date_vec[i].substr( 6, 2 ) + "-" + date_vec[i].substr( 3, 2 ) + "-" + date_vec[i].substr( 0, 2 );
            }

            // 创建TDatime对象vector
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

            // 填充TTree
            tree->Fill();
        }

        //    std::istringstream ssDate( line );
        //    std::string date_string;
        //    std::getline( ssDate, date_string, ',' ); // 跳过"时间标识"
        //    std::getline( ssDate, date_string, ',' ); // 获取日期和时间

        //    // 将日期和时间字符串的格式转换为"yyyy-mm-dd hh:mm:ss"
        //    std::replace( date_string.begin(), date_string.end(), '/', '-' );

        //    TDatime datime( date_string.c_str() );
        //    timestampOffset = datime.Convert();

        //    if ( isDebugModeActive )
        //    {
        //        std::cout << " - Initial date_string: " << date_string << std::endl;
        //        std::cout << " - Initial TDatime: " << datime.AsString() << std::endl;
        //        std::cout << " - Initial timestampOffset: " << timestampOffset << std::endl;
        //    }

        //    // 确定通道数
        //    int channelCount = commaCount / 2 + 1;

        //    // 跳过第二行
        //    csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

        //    // 读取第三行来获取通道名称
        //    std::getline( csvFile, line );
        //    std::istringstream ssCh( line );
        //    std::vector<std::string> channelNames( channelCount );
        //    for ( int i = 0; i < channelCount; ++i )
        //    {
        //        std::getline( ssCh, channelNames[i], ',' );
        //        if ( i == channelCount - 1 )
        //        {
        //            // 如果到达了行的末尾，直接读取剩余的所有字符
        //            std::getline( ssCh, channelNames[i] );
        //            // 检查并去掉换行符
        //            if ( !channelNames[i].empty() && ( channelNames[i].back() == '\n' || channelNames[i].back() == '\r' ) )
        //            {
        //                channelNames[i].pop_back();
        //            }
        //        }
        //        else
        //        {
        //            std::getline( ssCh, channelNames[i], ',' );
        //        }
        //        // 去掉双引号
        //        channelNames[i] = channelNames[i].substr( 1, channelNames[i].size() - 2 );
        //    }

        //    // 创建变量来存储branch的值
        //    std::vector<double> amplitudes( channelCount, 0.0 );

        //    // 创建branch
        //    for ( int i = 0; i < channelCount; ++i )
        //    {
        //        // 检查branch是否已经存在
        //        TBranch * branch = tree->GetBranch( channelNames[i].c_str() );
        //        if ( branch == nullptr )
        //        {
        //            // 如果branch不存在，创建新的branch
        //            tree->Branch( channelNames[i].c_str(), &amplitudes[i], ( channelNames[i] + "/D" ).c_str() );
        //        }
        //    }

        //    // 跳过剩余的前四行
        //    for ( int i = 0; i < 1; ++i )
        //    {
        //        csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        //    }

        //    // 读取CSV文件的数据
        //    while ( std::getline( csvFile, line ) )
        //    {
        //        std::istringstream ssAmp( line );
        //        std::string field;

        //        // 获取时间戳
        //        std::getline( ssAmp, field, ',' );
        //        timestamp = std::stod( field ) + timestampOffset;

        //        // 获取每个通道的幅度
        //        for ( int i = 0; i < channelCount; ++i )
        //        {
        //            std::getline( ssAmp, field, ',' );
        //            amplitudes[i] = std::stod( field );

        //            // 跳过下一个时间戳
        //            std::getline( ssAmp, field, ',' );
        //        }

        //        // 填充TTree
        //        tree->Fill();
        //    }
    }

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}
}
