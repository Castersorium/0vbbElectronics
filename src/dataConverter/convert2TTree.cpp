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
    TTree * tree = new TTree( "NIDAQReadings", "Output voltage readings from NI DAQ" );
    // tree -> SetMaxTreeSize(4000000LL);

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {
        std::cout << " - Input NI DAQ CSV directory read: " << csvDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // 创建一个变量来存储时间戳
    double timestamp = 0.0;
    double timestampOffset = 0.0;

    // 创建一个容器来存储通道名称
    std::vector<std::string> channelNames{};

    // 创建一个容器来存储branch的值
    std::vector<double> amplitudes{};

    // 创建时间戳branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );

    // 遍历目录下的所有文件，创建各个通道的branch所需的变量
    for ( const auto & entry : std::filesystem::directory_iterator( csvDirPath ) )
    {
        // if (entry.is_regular_file())
        // {
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
        size_t commaCount = std::count( line.begin(), line.end(), ',' );
        if ( commaCount == 0 )
        {
            std::cerr << "Error: CSV file format is incorrect in: " << csvFilePath << std::endl;
            continue;
        }

        // 确定通道数
        size_t channelCount = static_cast<size_t>( commaCount / 2 + 1 );

        // 跳过剩余的第二行
        for ( size_t i = 0; i < 1; ++i )
        {
            csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        // 读取第三行来确定通道顺序
        std::getline( csvFile, line );
        std::istringstream ssCh( line );

        for ( size_t i = 0; i < channelCount; ++i )
        {
            std::string channelName_temp = "";
            std::getline( ssCh, channelName_temp, ',' );
            if ( i == channelCount - 1 )
            {
                // 如果到达了行的末尾，直接读取剩余的所有字符
                std::getline( ssCh, channelName_temp );
                // 检查并去掉换行符
                if ( !channelName_temp.empty() && ( channelName_temp.back() == '\n' || channelName_temp.back() == '\r' ) )
                {
                    channelName_temp.pop_back();
                }
            }
            else
            {
                std::getline( ssCh, channelName_temp, ',' );
            }
            // 去掉双引号
            channelName_temp = channelName_temp.substr( 1, channelName_temp.size() - 2 );

            // 检查branch是否已经存在
            auto channelItr = std::find( channelNames.begin(), channelNames.end(), channelName_temp );
            if ( channelItr == channelNames.end() )
            {
                // 如果branch不存在，创建新branch所需的变量
                channelNames.emplace_back( channelName_temp );
                amplitudes.emplace_back( 0.0 );
            }
        }
        // }
    }

    if ( isDebugModeActive )
    {
        std::cout << " - Number of channels: " << channelNames.size() << std::endl;
        for ( const auto & channelName : channelNames )
        {
            std::cout << " - Channel name: " << channelName << std::endl;
        }
    }

    // 遍历目录下的所有文件，创建各个通道的branch
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

        // 读取第一行来确定通道数
        std::string line;
        std::getline( csvFile, line );
        size_t commaCount = std::count( line.begin(), line.end(), ',' );
        if ( commaCount == 0 )
        {
            std::cerr << "Error: CSV file format is incorrect in: " << csvFilePath << std::endl;
            continue;
        }

        // 确定通道数
        size_t channelCount = static_cast<size_t>( commaCount / 2 + 1 );

        // 跳过剩余的第二行
        for ( size_t i = 0; i < 1; ++i )
        {
            csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        // 读取第三行来确定通道顺序
        std::getline( csvFile, line );
        std::istringstream ssCh( line );

        for ( size_t i = 0; i < channelCount; ++i )
        {
            std::string channelName_temp = "";
            std::getline( ssCh, channelName_temp, ',' );
            if ( i == channelCount - 1 )
            {
                // 如果到达了行的末尾，直接读取剩余的所有字符
                std::getline( ssCh, channelName_temp );
                // 检查并去掉换行符
                if ( !channelName_temp.empty() && ( channelName_temp.back() == '\n' || channelName_temp.back() == '\r' ) )
                {
                    channelName_temp.pop_back();
                }
            }
            else
            {
                std::getline( ssCh, channelName_temp, ',' );
            }
            // 去掉双引号
            channelName_temp = channelName_temp.substr( 1, channelName_temp.size() - 2 );

            // 检查branch是否已经存在
            TBranch * branch = tree->GetBranch( channelName_temp.c_str() );
            if ( branch == nullptr )
            {
                // 如果branch不存在，创建新的branch
                // 如果branch的名称与通道名称匹配，则将通道名称容器channelNames中的index与amplitudes中的index对应
                for ( size_t vec_Index = 0; vec_Index < channelNames.size(); vec_Index++ )
                {
                    if ( channelNames[vec_Index] == channelName_temp )
                    {
                        tree->Branch( channelNames[vec_Index].c_str(), &amplitudes[vec_Index], ( channelNames[vec_Index] + "/D" ).c_str() );
                    }
                }
            }
        }
    }

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

        // 读取第一行来确定通道数
        std::string line;
        std::getline( csvFile, line );
        size_t commaCount = std::count( line.begin(), line.end(), ',' );
        if ( commaCount == 0 )
        {
            std::cerr << "Error: CSV file format is incorrect in: " << csvFilePath << std::endl;
            continue;
        }
        std::istringstream ssDate( line );
        std::string date_string;
        std::getline( ssDate, date_string, ',' ); // 跳过"时间标识"
        std::getline( ssDate, date_string, ',' ); // 获取日期和时间

        // 将日期和时间字符串的格式转换为"yyyy-mm-dd hh:mm:ss"
        std::replace( date_string.begin(), date_string.end(), '/', '-' );
        std::replace( date_string.begin(), date_string.end(), 'T', ' ' );

        // 如果日期和时间字符串中包含小数点，则去掉小数点及其后的部分，并且单独提取小数部分为双精度浮点数
        double millisecond = 0.0; // 但是因为这里进行解析的时候保留了小数点，所以这里得到的数值的单位仍然是秒
        size_t dotPos = date_string.find( '.' );
        if ( dotPos != std::string::npos )
        {
            millisecond = std::stod( date_string.substr( dotPos ) );
            date_string = date_string.substr( 0, dotPos );
        }

        TDatime datime( date_string.c_str() );
        timestampOffset = datime.Convert() + millisecond;

        if ( isDebugModeActive )
        {
            std::cout << " - Initial date_string: " << date_string << std::endl;
            std::cout << " - Initial TDatime: " << datime.AsString() << std::endl;
            std::cout << " - Initial millisecond: " << millisecond << std::endl;
            std::cout << " - Initial timestampOffset: " << timestampOffset << std::endl;
        }

        // 确定通道数
        size_t channelCount = static_cast<size_t>( commaCount / 2 + 1 );

        // 跳过第二行
        csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

        // 创建变量来存储branch的值
        std::vector<size_t> Indices_order( channelCount, 0 );

        // 读取第三行来获取通道名称
        std::getline( csvFile, line );
        std::istringstream ssCh( line );
        for ( size_t i = 0; i < channelCount; ++i )
        {
            std::string channelName_temp = "";
            std::getline( ssCh, channelName_temp, ',' );
            if ( i == channelCount - 1 )
            {
                // 如果到达了行的末尾，直接读取剩余的所有字符
                std::getline( ssCh, channelName_temp );
                // 检查并去掉换行符
                if ( !channelName_temp.empty() && ( channelName_temp.back() == '\n' || channelName_temp.back() == '\r' ) )
                {
                    channelName_temp.pop_back();
                }
            }
            else
            {
                std::getline( ssCh, channelName_temp, ',' );
            }
            // 去掉双引号
            channelName_temp = channelName_temp.substr( 1, channelName_temp.size() - 2 );

            // 检查branch是否已经存在
            TBranch * branch = tree->GetBranch( channelName_temp.c_str() );
            if ( branch == nullptr )
            {
                // 如果branch不存在，抛出异常
                std::cerr << "Error: Branch " << channelName_temp << " does not exist." << std::endl;
            }
            else
            {
                // 如果branch存在，检查branch的名称是否与通道名称匹配
                if ( std::string( branch->GetName() ) != channelName_temp )
                {
                    std::cerr << "Error: Branch " << branch->GetName() << " does not match channel name " << channelName_temp << "." << std::endl;
                }

                // 如果branch的名称与通道名称匹配，继续
                if ( isDebugModeActive )
                {
                    std::cout << " - Branch " << branch->GetName() << " matches channel name " << channelName_temp << "." << std::endl;
                }

                // 如果branch的名称与通道名称匹配，则将通道名称容器channelNames中的index与amplitudes中的index对应
                for ( size_t vec_Index = 0; vec_Index < channelNames.size(); vec_Index++ )
                {
                    if ( channelNames[vec_Index] == channelName_temp )
                    {
                        Indices_order[i] = vec_Index;
                    }
                }
            }
        }

        if ( isDebugModeActive )
        {
            for ( size_t i = 0; i < channelCount; ++i )
            {
                std::cout << " - Indices_order[" << i << "]: " << Indices_order[i] << std::endl;
            }
        }

        // 跳过剩余的第四行
        for ( size_t i = 0; i < 1; ++i )
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

            std::generate( amplitudes.begin(), amplitudes.end(), []()
                           {
                               return NAN;
                           } );

            // 获取每个通道的幅度
            for ( size_t i = 0; i < channelCount; ++i )
            {
                std::getline( ssAmp, field, ',' );
                amplitudes[Indices_order[i]] = std::stod( field );

                // 跳过下一个时间戳
                std::getline( ssAmp, field, ',' );
            }

            // 填充TTree
            tree->Fill();
        }
    }

    // 检查TTree是否包含任何条目
    if ( tree->GetEntries() == 0 )
    {
        std::clog << "Warning: TTree " << tree->GetName() << " does not contain any entries." << std::endl;
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
    TTree * tree = new TTree( "TemperatureReadings", "Temperature readings from BLUEFORS" );

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
            continue; // 跳过范围之外的日期
        }

        if ( isDebugModeActive )
        {
            std::cout << " - Reading date directory: " << dateDir << std::endl;
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
        while ( std::getline( logFile_vec[0], line_vec[0] ) && std::getline( logFile_vec[1], line_vec[1] ) && std::getline( logFile_vec[2], line_vec[2] ) && std::getline( logFile_vec[3], line_vec[3] ) && std::getline( logFile_vec[4], line_vec[4] ) )
        {
            std::vector<std::istringstream> ss_vec( logFile_vec.size() );
            std::vector<std::string> date_vec( logFile_vec.size() );
            std::vector<std::string> time_vec( logFile_vec.size() );
            std::vector<std::string> field_vec( logFile_vec.size() );
            for ( size_t i = 0; i < logFile_vec.size(); ++i )
            {
                ss_vec[i] = std::istringstream( line_vec[i] );
                std::getline( ss_vec[i], date_vec[i], ',' ); // 获取日期

                // 去除字符串左端的空格
                date_vec[i].erase( date_vec[i].begin(), std::find_if( date_vec[i].begin(), date_vec[i].end(), []( auto ch )
                                                                      {
                                                                          return !std::isspace( ch );
                                                                      } ) );

                std::getline( ss_vec[i], time_vec[i], ',' ); // 获取时间
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

            // 将日期字符串从"DD-MM-YY"转换为"YYYY-MM-DD"的格式
            for ( size_t i = 0; i < logFile_vec.size(); ++i )
            {
                date_vec[i] = "20" + date_vec[i].substr( 6, 2 ) + "-" + date_vec[i].substr( 3, 2 ) + "-" + date_vec[i].substr( 0, 2 );
            }

            // 创建TDatime对象vector
            std::vector<TDatime> datetime_vec( logFile_vec.size() );
            for ( size_t i = 0; i < logFile_vec.size(); ++i )
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

            for ( size_t i = 1; i < logFile_vec.size(); ++i )
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

        line_vec.clear();
        logFile_vec.clear();
        logFileName_vec.clear();
    }

    // 检查TTree是否包含任何条目
    if ( tree->GetEntries() == 0 )
    {
        std::clog << "Warning: TTree " << tree->GetName() << " does not contain any entries." << std::endl;
    }

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}

// 定义一个函数，用于将BlueforsLog格式的数据转换为TTree格式
void convert2TTree::convertBlueforsPressureLog2TTree( const std::string & BlueforsLogDirPath, const std::string & rootFileName ) const
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "PressureReadings", "Pressure readings from BLUEFORS" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {
        std::cout << " - Input BLUEFORS_LOG directory read: " << BlueforsLogDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // 创建一个变量来存储时间戳
    double timestamp = 0.0;
    double pressure_1 = 0.0;
    double pressure_2 = 0.0;
    double pressure_3 = 0.0;
    double pressure_4 = 0.0;
    double pressure_5 = 0.0;
    double pressure_6 = 0.0;

    // 创建时间戳branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );
    tree->Branch( "P1", &pressure_1, "P1_CHAMBER/D" );
    tree->Branch( "P2", &pressure_2, "P2_STILL/D" );
    tree->Branch( "P3", &pressure_3, "P3_CONDENSING/D" );
    tree->Branch( "P4", &pressure_4, "P4_TRAP/D" );
    tree->Branch( "P5", &pressure_5, "P5_TANK/D" );
    tree->Branch( "P6", &pressure_6, "P6_SERVICE/D" );

    std::vector<std::string> logFileString_vec = { "maxigauge " };
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
            continue; // 跳过范围之外的日期
        }

        if ( isDebugModeActive )
        {
            std::cout << " - Reading date directory: " << dateDir << std::endl;
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
        while ( std::getline( logFile_vec[0], line_vec[0] ) )
        {
            std::vector<std::istringstream> ss_vec( logFile_vec.size() );
            std::vector<std::string> date_vec( logFile_vec.size() );
            std::vector<std::string> time_vec( logFile_vec.size() );
            std::vector<std::string> field_vec( 6 );
            for ( size_t i = 0; i < logFile_vec.size(); ++i )
            {
                ss_vec[i] = std::istringstream( line_vec[i] );
                std::getline( ss_vec[i], date_vec[i], ',' ); // 获取日期

                // 去除字符串左端的空格
                date_vec[i].erase( date_vec[i].begin(), std::find_if( date_vec[i].begin(), date_vec[i].end(), []( auto ch )
                                                                      {
                                                                          return !std::isspace( ch );
                                                                      } ) );

                std::getline( ss_vec[i], time_vec[i], ',' ); // 获取时间

                // 获取压强
                for ( size_t ColumnCount = 0; ColumnCount < 4; ColumnCount++ )
                {
                    std::getline( ss_vec[i], field_vec[0], ',' ); // 获取压强
                }

                for ( size_t PressureCount = 1; PressureCount < 6; PressureCount++ )
                {
                    for ( size_t ColumnCount = 0; ColumnCount < 6; ColumnCount++ )
                    {
                        std::getline( ss_vec[i], field_vec[PressureCount], ',' ); // 获取压强
                    }
                }
            }

            if ( isDebugModeActive )
            {
                std::cout << " - date_string: " << date_vec[0] << std::endl;
                std::cout << " - time_string: " << time_vec[0] << std::endl;
            }

            // 将日期字符串从"DD-MM-YY"转换为"YYYY-MM-DD"的格式
            for ( size_t i = 0; i < logFile_vec.size(); ++i )
            {
                date_vec[i] = "20" + date_vec[i].substr( 6, 2 ) + "-" + date_vec[i].substr( 3, 2 ) + "-" + date_vec[i].substr( 0, 2 );
            }

            // 创建TDatime对象vector
            std::vector<TDatime> datetime_vec( logFile_vec.size() );
            for ( size_t i = 0; i < logFile_vec.size(); ++i )
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

            pressure_1 = std::stod( field_vec[0] );
            pressure_2 = std::stod( field_vec[1] );
            pressure_3 = std::stod( field_vec[2] );
            pressure_4 = std::stod( field_vec[3] );
            pressure_5 = std::stod( field_vec[4] );
            pressure_6 = std::stod( field_vec[5] );

            if ( isDebugModeActive )
            {
                std::cout << " - pressure_1: " << pressure_1 << std::endl;
                std::cout << " - pressure_2: " << pressure_2 << std::endl;
                std::cout << " - pressure_3: " << pressure_3 << std::endl;
                std::cout << " - pressure_4: " << pressure_4 << std::endl;
                std::cout << " - pressure_5: " << pressure_5 << std::endl;
                std::cout << " - pressure_6: " << pressure_6 << std::endl;
            }

            // 填充TTree
            tree->Fill();
        }

        line_vec.clear();
        logFile_vec.clear();
        logFileName_vec.clear();
    }

    // 检查TTree是否包含任何条目
    if ( tree->GetEntries() == 0 )
    {
        std::clog << "Warning: TTree " << tree->GetName() << " does not contain any entries." << std::endl;
    }

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}

// 定义一个函数，用于将MultimeterData格式的数据转换为TTree格式
void convert2TTree::convertMultimeterData2TTree( const std::string & csvDirPath, const std::string & rootFileName ) const
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "MultimeterReadings", "Multimeter readings" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {
        std::cout << " - Input Multimeter Data directory read: " << csvDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // 创建一个变量来存储时间戳
    double timestamp = 0.0;

    size_t columnCount = 0;

    std::vector<std::string> columnName_vec = { "DCV_V", "ACV_V", "DCI_A", "ACI_A", "Freq_Hz", "Period_s", "Res_ohm", "Cap_F", "Temp_K" };

    // 创建一个容器来存储通道名称
    std::vector<std::string> channelNames{};

    // 创建一个容器来存储branch的值
    std::vector<double> readings_vec{};

    // 创建时间戳branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );

    // 遍历目录下的所有文件，创建各个通道的branch所需的变量
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

        // 跳过第一行
        for ( size_t i = 0; i < 1; ++i )
        {
            csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        std::string line;

        // 读取CSV文件的第二行来获取通道名称
        std::getline( csvFile, line );
        std::istringstream ssIn( line );
        std::string date_string;
        std::string time_string;
        std::string millisecond_string;
        std::string field;

        std::getline( ssIn, date_string, '>' ); // 获取日期

        // 检查并去掉短横线
        if ( !date_string.empty() && date_string.back() == '-' )
        {
            date_string.pop_back();
        }

        // 将日期字符串的格式转换为"yyyy-mm-dd"
        std::replace( date_string.begin(), date_string.end(), '/', '-' );

        std::getline( ssIn, time_string, '.' );         // 获取时间
        std::getline( ssIn, millisecond_string, '\t' ); // 获取毫秒

        // 去除字符串左端的空格
        millisecond_string.erase( millisecond_string.begin(), std::find_if( millisecond_string.begin(), millisecond_string.end(), []( auto ch )
                                                                            {
                                                                                return !std::isspace( ch );
                                                                            } ) );

        TDatime datime( ( date_string + " " + time_string ).c_str() );
        timestamp = datime.Convert() + std::stod( millisecond_string ) / 1000.0;

        if ( isDebugModeActive )
        {
            std::cout << " - date_string: " << date_string + " " + time_string << std::endl;
            std::cout << " - TDatime: " << datime.AsString() << std::endl;
            std::cout << " - millisecond: " << millisecond_string << std::endl;
            std::cout << " - timestamp: " << timestamp << std::endl;
        }

        do
        {
            std::getline( ssIn, field, '\t' ); // 获取测量值

            columnCount++;

            // 检查并去掉换行符
            if ( !field.empty() && ( field.back() == '\n' || field.back() == '\r' ) )
            {
                field.pop_back();
            }

            if ( isDebugModeActive )
            {
                std::cout << " - readings_string: " << field << std::endl;
                std::cout << " - NaN value?: " << ( field == "--" ) << std::endl;
                std::cout << " - column count: " << columnCount << std::endl;
            }
        } while ( field == "--" );

        double readings = std::stod( field );

        if ( isDebugModeActive )
        {
            std::cout << " - readings_value: " << readings << std::endl;
        }

        // 检查branch是否已经存在
        auto channelItr = std::find( channelNames.begin(), channelNames.end(), columnName_vec[columnCount - 1] );
        if ( channelItr == channelNames.end() )
        {
            // 如果branch不存在，创建新branch所需的变量
            channelNames.emplace_back( columnName_vec[columnCount - 1] );
            readings_vec.emplace_back( 0.0 );
        }

        columnCount = 0;
    }

    if ( isDebugModeActive )
    {
        std::cout << " - Number of channels: " << channelNames.size() << std::endl;
        for ( const auto & channelName : channelNames )
        {
            std::cout << " - Channel name: " << channelName << std::endl;
        }
    }

    // 遍历目录下的所有文件，创建各个通道的branch
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

        // 跳过第一行
        for ( size_t i = 0; i < 1; ++i )
        {
            csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        std::string line;

        // 读取CSV文件的第二行来获取通道名称
        std::getline( csvFile, line );
        std::istringstream ssIn( line );
        std::string date_string;
        std::string time_string;
        std::string millisecond_string;
        std::string field;

        std::getline( ssIn, date_string, '>' ); // 获取日期

        // 检查并去掉短横线
        if ( !date_string.empty() && date_string.back() == '-' )
        {
            date_string.pop_back();
        }

        // 将日期字符串的格式转换为"yyyy-mm-dd"
        std::replace( date_string.begin(), date_string.end(), '/', '-' );

        std::getline( ssIn, time_string, '.' );         // 获取时间
        std::getline( ssIn, millisecond_string, '\t' ); // 获取毫秒

        // 去除字符串左端的空格
        millisecond_string.erase( millisecond_string.begin(), std::find_if( millisecond_string.begin(), millisecond_string.end(), []( auto ch )
                                                                            {
                                                                                return !std::isspace( ch );
                                                                            } ) );

        TDatime datime( ( date_string + " " + time_string ).c_str() );
        timestamp = datime.Convert() + std::stod( millisecond_string ) / 1000.0;

        if ( isDebugModeActive )
        {
            std::cout << " - date_string: " << date_string + " " + time_string << std::endl;
            std::cout << " - TDatime: " << datime.AsString() << std::endl;
            std::cout << " - millisecond: " << millisecond_string << std::endl;
            std::cout << " - timestamp: " << timestamp << std::endl;
        }

        do
        {
            std::getline( ssIn, field, '\t' ); // 获取测量值

            columnCount++;

            // 检查并去掉换行符
            if ( !field.empty() && ( field.back() == '\n' || field.back() == '\r' ) )
            {
                field.pop_back();
            }

            if ( isDebugModeActive )
            {
                std::cout << " - readings_string: " << field << std::endl;
                std::cout << " - NaN value?: " << ( field == "--" ) << std::endl;
                std::cout << " - column count: " << columnCount << std::endl;
            }
        } while ( field == "--" );

        double readings = std::stod( field );

        if ( isDebugModeActive )
        {
            std::cout << " - readings_value: " << readings << std::endl;
        }

        // 检查branch是否已经存在
        TBranch * branch = tree->GetBranch( columnName_vec[columnCount - 1].c_str() );
        if ( branch == nullptr )
        {
            // 如果branch不存在，创建新的branch
            // 如果branch的名称与通道名称匹配，则将通道名称容器channelNames中的index与readings_vec中的index对应
            for ( size_t vec_Index = 0; vec_Index < channelNames.size(); vec_Index++ )
            {
                if ( channelNames[vec_Index] == columnName_vec[columnCount - 1] )
                {
                    tree->Branch( channelNames[vec_Index].c_str(), &readings_vec[vec_Index], ( channelNames[vec_Index] + "/D" ).c_str() );
                }
            }
        }

        columnCount = 0;
    }

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

        // 创建变量来存储branch的值
        std::vector<size_t> Indices_order{ 0 };

        // 跳过第一行
        for ( size_t i = 0; i < 1; ++i )
        {
            csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        }

        std::string line;

        bool isFirstLine = true;

        // 读取CSV文件的数据
        while ( std::getline( csvFile, line ) )
        {
            if ( isDebugModeActive )
            {
                if ( tree->GetEntries() >= 1000 )
                {
                    break;
                }
            }

            std::istringstream ssIn( line );
            std::string date_string;
            std::string time_string;
            std::string millisecond_string;
            std::string field;

            std::getline( ssIn, date_string, '>' ); // 获取日期

            // 检查并去掉短横线
            if ( !date_string.empty() && date_string.back() == '-' )
            {
                date_string.pop_back();
            }

            // 将日期字符串的格式转换为"yyyy-mm-dd"
            std::replace( date_string.begin(), date_string.end(), '/', '-' );

            std::getline( ssIn, time_string, '.' );         // 获取时间
            std::getline( ssIn, millisecond_string, '\t' ); // 获取毫秒

            // 去除字符串左端的空格
            millisecond_string.erase( millisecond_string.begin(), std::find_if( millisecond_string.begin(), millisecond_string.end(), []( auto ch )
                                                                                {
                                                                                    return !std::isspace( ch );
                                                                                } ) );

            TDatime datime( ( date_string + " " + time_string ).c_str() );
            timestamp = datime.Convert() + std::stod( millisecond_string ) / 1000.0;

            if ( isDebugModeActive )
            {
                std::cout << " - date_string: " << date_string + " " + time_string << std::endl;
                std::cout << " - TDatime: " << datime.AsString() << std::endl;
                std::cout << " - millisecond: " << millisecond_string << std::endl;
                std::cout << " - timestamp: " << timestamp << std::endl;
            }

            std::generate( readings_vec.begin(), readings_vec.end(), []()
                           {
                               return NAN;
                           } );

            do
            {
                std::getline( ssIn, field, '\t' ); // 获取测量值

                columnCount++;

                // 检查并去掉换行符
                if ( !field.empty() && ( field.back() == '\n' || field.back() == '\r' ) )
                {
                    field.pop_back();
                }

                if ( isDebugModeActive )
                {
                    std::cout << " - readings_string: " << field << std::endl;
                    std::cout << " - NaN value?: " << ( field == "--" ) << std::endl;
                    std::cout << " - column count: " << columnCount << std::endl;
                }
            } while ( field == "--" );

            double readings = std::stod( field );

            if ( isDebugModeActive )
            {
                std::cout << " - readings_value: " << readings << std::endl;
            }

            if ( isFirstLine == true )
            {
                // 检查branch是否已经存在
                TBranch * branch = tree->GetBranch( columnName_vec[columnCount - 1].c_str() );
                if ( branch == nullptr )
                {
                    // 如果branch不存在，抛出异常
                    std::cerr << "Error: Branch " << columnName_vec[columnCount - 1] << " does not exist." << std::endl;
                }
                else
                {
                    // 如果branch存在，检查branch的名称是否与通道名称匹配
                    if ( std::string( branch->GetName() ) != columnName_vec[columnCount - 1] )
                    {
                        std::cerr << "Error: Branch " << branch->GetName() << " does not match channel name " << columnName_vec[columnCount - 1] << "." << std::endl;
                    }

                    // 如果branch的名称与通道名称匹配，继续
                    if ( isDebugModeActive )
                    {
                        std::cout << " - Branch " << branch->GetName() << " matches channel name " << columnName_vec[columnCount - 1] << "." << std::endl;
                    }

                    // 如果branch的名称与通道名称匹配，则将通道名称容器channelNames中的index与amplitudes中的index对应
                    for ( size_t vec_Index = 0; vec_Index < channelNames.size(); vec_Index++ )
                    {
                        if ( channelNames[vec_Index] == columnName_vec[columnCount - 1] )
                        {
                            Indices_order[0] = vec_Index;
                        }
                    }
                }

                isFirstLine = false;
            }

            readings_vec[Indices_order[0]] = readings;

            columnCount = 0;

            // 填充TTree
            tree->Fill();
        }
    }

    // 检查TTree是否包含任何条目
    if ( tree->GetEntries() == 0 )
    {
        std::clog << "Warning: TTree " << tree->GetName() << " does not contain any entries." << std::endl;
    }

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}

// 定义一个函数，用于将CelsiusData格式的数据转换为TTree格式
void convert2TTree::convertCelsiusData2TTree( const std::string & csvDirPath, const std::string & rootFileName ) const
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "Centigrade_Humidity", "Centigrade and Humidity" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {
        std::cout << " - Input Celsius Data directory read: " << csvDirPath << std::endl;
        std::cout << " - Output ROOT file created: " << rootFileName << std::endl;
    }

    // 创建一个变量来存储时间戳
    double timestamp = 0.0;

    // 创建变量来存储温度和湿度
    double centigrade = 0.0;
    double humidity = 0.0;

    // 创建时间戳branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );

    // 创建温度和湿度branch
    tree->Branch( "centigrade", &centigrade, "centigrade/D" );
    tree->Branch( "humidity", &humidity, "humidity/D" );

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

        // 创建一个变量用于检查"MM/DD/YY"作为标志出现的次数
        size_t count_MM_DD_YY = 0;

        // 跳过数据文件头部的描述信息，直至到达数据的第一行，以"MM/DD/YY"作为标志出现
        std::string line;
        while ( std::getline( csvFile, line ) )
        {
            if ( line.find( "MM/DD/YY" ) != std::string::npos )
            {
                count_MM_DD_YY++;
                break;
            }
        }
        // "MM/DD/YY"标志会出现两次，第二次出现的地方就是数据的第一行
        while ( std::getline( csvFile, line ) )
        {
            if ( line.find( "MM/DD/YY" ) != std::string::npos )
            {
                count_MM_DD_YY++;
                break;
            }
        }

        // 检查"MM/DD/YY"标志是否出现了两次
        if ( count_MM_DD_YY != 2 )
        {
            std::cerr << "Error: The number of occurrences of the string \"MM/DD/YY\" is not equal to 2 in file: " << csvFilePath << "." << std::endl;
            continue;
        }

        // 读取CSV文件的数据
        while ( std::getline( csvFile, line ) )
        {
            std::istringstream ssIn( line );
            std::string date_string;
            std::string time_string;
            std::string centigrade_string;
            std::string humidity_string;

            std::getline( ssIn, date_string, ',' ); // 获取日期

            // 将日期字符串的格式从"MM/DD/YY"转换为"yyyy-mm-dd"
            date_string = "20" + date_string.substr( 6, 2 ) + "-" + date_string.substr( 0, 2 ) + "-" + date_string.substr( 3, 2 );

            std::getline( ssIn, time_string, ',' ); // 获取时间

            // 去除字符串左端的空格
            time_string.erase( time_string.begin(), std::find_if( time_string.begin(), time_string.end(), []( auto ch )
                                                                  {
                                                                      return !std::isspace( ch );
                                                                  } ) );

            TDatime datime( ( date_string + " " + time_string ).c_str() );
            timestamp = datime.Convert();

            if ( isDebugModeActive )
            {
                std::cout << " - date_string: " << date_string + " " + time_string << std::endl;
                std::cout << " - TDatime: " << datime.AsString() << std::endl;
                std::cout << " - timestamp: " << timestamp << std::endl;
            }

            std::getline( ssIn, centigrade_string, ',' ); // 获取温度测量值

            std::getline( ssIn, humidity_string, ',' ); // 跳过TemperatureStatus
            std::getline( ssIn, humidity_string, ',' ); // 获取湿度测量值

            if ( isDebugModeActive )
            {
                std::cout << " - centigrade_string: " << centigrade_string << std::endl;
                std::cout << " - humidity_string: " << humidity_string << std::endl;
            }

            centigrade = std::stod( centigrade_string );
            humidity = std::stod( humidity_string );

            if ( isDebugModeActive )
            {
                std::cout << " - centigrade: " << centigrade << std::endl;
                std::cout << " - humidity: " << humidity << std::endl;
            }

            // 填充TTree
            tree->Fill();

            centigrade = 0.0;
            humidity = 0.0;
        }
    }

    // 检查TTree是否包含任何条目
    if ( tree->GetEntries() == 0 )
    {
        std::clog << "Warning: TTree " << tree->GetName() << " does not contain any entries." << std::endl;
    }

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}
}
