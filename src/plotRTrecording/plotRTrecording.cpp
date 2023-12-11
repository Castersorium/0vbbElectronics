#include <iostream> // C++ header

#include "markdownFileInput.hpp" // my header

int main( int argc, char * argv[] )
{
    // 检查是否提供了文件路径
    if ( argc < 2 )
    {
        std::cerr << "Usage: " << argv[0] << " <markdown_file_path>" << std::endl;
        return EXIT_FAILURE;
    }

    // 获取文件路径
    std::string filePath = argv[1];

    // 创建markdownFileInput类的实例并读取文件
    MDFIO::markdownFileInput mdfInput;
    mdfInput.readMarkdownTable( filePath );
    mdfInput.readGeneralInformation();
    mdfInput.readMeasurementEntries();

    // 打印通道9的基本信息
    mdfInput.printGeneralInformation( 9 );

    // 打印第一个测量条目的信息
    mdfInput.printMeasurementEntry( 0 );

    return 0;
}


