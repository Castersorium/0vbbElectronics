#include <iostream> // C++ header
#include <memory>
#include <filesystem>
#include <string>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header

int main( int argc, char * argv[] )
{
    // 检查是否提供了文件路径
    if ( argc < 3 )
    {
        std::cout << "Usage: " << argv[0] << " <input CSV file> <output ROOT file>" << std::endl;
        return 1;
    }

    // 从命令行参数中获取文件路径
    std::filesystem::path csvFilePath = argv[1];
    std::filesystem::path rootFilePath = argv[2];

    // 检查路径是否存在且是一个目录
    if ( !std::filesystem::exists( csvFilePath ) || !std::filesystem::is_directory( csvFilePath ) )
    {
        std::cout << "Error: CSV path " << csvFilePath << "does not exist or is not a directory." << std::endl;
        return 1;
    }
    if ( !std::filesystem::exists( rootFilePath ) || !std::filesystem::is_directory( rootFilePath ) )
    {
        std::cout << "Error: ROOT path " << rootFilePath << "does not exist or is not a directory." << std::endl;
        return 1;
    }
    std::string csvFilePath_str = csvFilePath.string();
    std::string rootFilePath_str = rootFilePath.string();

    // 创建convert2TTree的实例
    std::unique_ptr< TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // 打开debug模式
    myConverter->setDebug( true );

    // 遍历目录下的所有文件
    //for ( const auto & entry : std::filesystem::directory_iterator( csvFilePath ) )
    //{
    //    std::cout << "File path:" << entry.path() << std::endl;
    //    // 调用convertCSV2TTree函数来转换数据
    //    myConverter->convertCSV2TTree( "data.csv", rootFilePath_str + "./data.root" );
    //}
    myConverter->convertCSV2TTree( csvFilePath_str + "记录-2024-04-02 091354 803.csv", rootFilePath_str + "./data.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
