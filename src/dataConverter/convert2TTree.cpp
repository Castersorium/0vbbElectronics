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
// 定义一个函数，用于将CSV格式的数据转换为TTree格式
void convert2TTree::convertCSV2TTree( const std::string csvFileName, const std::string rootFileName )
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {

        std::cout << " - Input csv file read: " << csvFileName << std::endl;
        std::cout << " - Output root file created: " << rootFileName << std::endl;
    }

    // 打开CSV文件
    std::ifstream csvFile( csvFileName );
    if ( !csvFile.is_open() )
    {
        std::cerr << "Can not open csv file: " << csvFileName << std::endl;
        return;
    }

    
    // 读取第一行来确定通道数
    std::string line;
    std::getline( csvFile, line );
    int commaCount = std::count( line.begin(), line.end(), ',' );
    if ( commaCount == 0 )
    {
        std::cerr << "Error: CSV file format is incorrect" << std::endl;
        return;
    }
    int channelCount = commaCount / 2 + 1;

    // 创建变量来存储branch的值
    std::vector<double> amplitudes( channelCount, 0.0 );

    // 创建branch
    for ( int i = 0; i < channelCount; ++i )
    {
        tree->Branch( ( "Amp" + std::to_string( i ) ).c_str(), &amplitudes[i], ( "Amp" + std::to_string( i ) + "/D" ).c_str() );
    }

    // 跳过剩余的前四行
    for ( int i = 0; i < 3; ++i )
    {
        csvFile.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
    }

    // 读取CSV文件的数据
    while ( std::getline( csvFile, line ) )
    {
        std::istringstream ss( line );
        std::string field;

        // 获取时间戳
        std::getline( ss, field, ',' );
        double timestamp = std::stod( field );

        // 获取每个通道的幅度
        for ( int i = 0; i < channelCount; ++i )
        {
            std::getline( ss, field, ',' );
            amplitudes[i] = std::stod( field );

            // 跳过下一个时间戳
            std::getline( ss, field, ',' );
        }

        // 填充TTree
        tree->Fill();
    }

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}
}
