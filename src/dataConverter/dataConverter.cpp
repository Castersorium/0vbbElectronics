#include <iostream> // C++ header
#include <memory>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header

int main()
{
    // 在这里，你可以控制你需要使用的所有函数
    // 例如，你可以调用convertTDMS2TTree函数来转换数据
    std::unique_ptr< TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // 打开debug模式
    myConverter->setDebug( true );

    // 调用convertTDMS2TTree函数来转换数据
    myConverter->convertTDMS2TTree( "data.tdms", "data.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
