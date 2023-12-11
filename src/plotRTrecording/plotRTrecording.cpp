#include <iostream> // C++ header

#include "markdownTableDataExtractor.hpp" // my header

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
    MDFIO::markdownTableDataExtractor  mdfInput;
    mdfInput.readMarkdownTable( filePath );
    mdfInput.readGeneralInformation();
    mdfInput.readMeasurementEntries();
    mdfInput.convertTemperature();
    mdfInput.convertStringToDouble();

    // 打印通道9的基本信息
    //mdfInput.printGeneralInformation( 9 );

    // 打印第一个测量条目的信息
    //mdfInput.printMeasurementEntry( 0 );

    // 调用函数打印所有double向量中的数据结果
    mdfInput.printAllDoubleVectors();

    return 0;
}
