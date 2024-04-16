#include <iostream> // C++ header
#include <memory>
#include <filesystem>
#include <string>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header
#include "TTreePlotter.hpp"

int main( int argc, char * argv[] )
{
    // 检查是否提供了文件路径
    if ( argc < 4 )
    {
        std::cout << "Usage: " << argv[0] << " <input CSV directory> <output ROOT directory> <output plot directory>" << std::endl;
        return 1;
    }

    // 从命令行参数中获取文件路径
    std::filesystem::path csvDirPath = argv[1];
    std::filesystem::path rootDirPath = argv[2];
    std::filesystem::path plotDirPath = argv[3];

    // 检查路径是否存在且是一个目录
    if ( !std::filesystem::exists( csvDirPath ) || !std::filesystem::is_directory( csvDirPath ) )
    {
        std::cout << "Error: CSV directory " << csvDirPath << " does not exist or is not a directory." << std::endl;
        return 1;
    }
    if ( !std::filesystem::exists( rootDirPath ) || !std::filesystem::is_directory( rootDirPath ) )
    {
        std::cout << "Error: ROOT directory " << rootDirPath << " does not exist or is not a directory." << std::endl;
        return 1;
    }
    if ( !std::filesystem::exists( plotDirPath ) || !std::filesystem::is_directory( plotDirPath ) )
    {
        std::cout << "Error: Plot directory " << plotDirPath << " does not exist or is not a directory." << std::endl;
        return 1;
    }

    // 创建convert2TTree的实例
    std::unique_ptr<TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // 打开debug模式
    //myConverter->setDebug(true);

    // 转换CSV文件到ROOT文件
    myConverter->convertCSV2TTree( csvDirPath.string(), rootDirPath.string() + "/data.root" );

    // 创建TTreePlotter的实例
    std::unique_ptr<TTREEIO::TTreePlotter> myPlotter = std::make_unique<TTREEIO::TTreePlotter>();

    // 打开debug模式
    myPlotter->setDebug(true);
    myPlotter->setAmpHistoBinWidth(0.01);
    myPlotter->setTimeWindow(2.0);

    // 从ROOT文件创建TGraphErrors并保存到ROOT文件
    myPlotter->createGraphFromTree( rootDirPath.string() + "/data.root", plotDirPath.string() + "/plot.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
