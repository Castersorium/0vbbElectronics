#include <iostream> // C++ header

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header

namespace TTREEIO
{
// 定义一个函数，用于将TDMS格式的数据转换为TTree格式
void convert2TTree::convertTDMS2TTree( const std::string tdmsFileName, const std::string rootFileName )
{
    // 创建一个新的ROOT文件
    TFile * file = new TFile( rootFileName.c_str(), "RECREATE" );

    // 创建一个新的TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // 如果isDebugModeActive状态为true，打印出一条消息
    if ( isDebugModeActive )
    {

        std::cout << " - Input root file read: " << tdmsFileName << std::endl;
        std::cout << " - Output root file created: " << rootFileName << std::endl;
    }

    // 创建变量来存储branch的值
    double timestamp = 0;
    double amplitude = 0;

    // 创建两个branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );
    tree->Branch( "amplitude", &amplitude, "amplitude/D" );

    // 在这里，你需要添加代码来读取TDMS文件的数据
    // 然后，对于每一条数据，你需要更新timestamp和amplitude的值，然后填充TTree
    // 例如：
    // timestamp = ...;  // 更新timestamp的值
    // amplitude = ...;  // 更新amplitude的值
    // tree->Fill();     // 填充TTree

    // 将TTree写入到ROOT文件中
    tree->Write();

    // 关闭ROOT文件
    delete file;
}
}
