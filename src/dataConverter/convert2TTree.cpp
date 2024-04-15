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
// 定义一个函数，用于将CSV格式的数据转换为TTree格式
void convert2TTree::convertCSV2TTree( const std::string & csvDirPath, const std::string & rootFileName )
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {
        std::cout << " - Input CSV directory read: " << csvDirPath << std::endl;
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
}
