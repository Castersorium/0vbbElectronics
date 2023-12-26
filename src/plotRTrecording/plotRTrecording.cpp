#include <iostream> // C++ header

#include "markdownTableDataExtractor.hpp" // my header
#include "ROOTDataSaver.hpp"
#include "ROOTDataPlotter.hpp"

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

    // 读取默认markdown输入文件
    std::string filePathandName = filePath + "./Resistance_measurement.md";

    // 创建markdownFileInput类的实例并读取文件
    MDFIO::markdownTableDataExtractor  mdfInput;
    mdfInput.readMarkdownTable( filePathandName );
    mdfInput.readGeneralInformation();
    mdfInput.readMeasurementEntries();
    mdfInput.convertTemperature();
    mdfInput.convertStringToDouble();

    // 打印通道9的基本信息
    //mdfInput.printGeneralInformation( 9 );

    // 打印第一个测量条目的信息
    //mdfInput.printMeasurementEntry( 0 );

    // 调用函数打印所有double向量中的数据结果
    //mdfInput.printAllDoubleVectors();

    TTREEIO::RootDataSaver saver( mdfInput );

    // 创建默认root输出文件
    filePathandName = filePath + "./Resistance_measurement.root";

    saver.saveToRootFile( filePathandName );

    // 创建ROOTDataPlotter实例
    TTREEIO::ROOTDataPlotter dataPlotter( filePathandName );

    // 创建并填充图形
    dataPlotter.plotData();

    // 创建默认RTPlot输出文件
    filePathandName = filePath + "./FJYRTPlot.root";

    // 绘制并保存所有图形
    dataPlotter.plotAllGraphs( filePathandName );

    return 0;
}

