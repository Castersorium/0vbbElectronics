#include <iostream> // C++ header
#include <memory>
#include <filesystem>
#include <string>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header
#include "TTreePlotter.hpp"
#include "flagHandler.hpp"

int main( int argc, char * argv[] )
{

    // 检查是否提供了文件路径
    if ( argc < 8 )
    {
        std::cout << "Usage: " << argv[0] << " <input NIDAQCSV directory> <output ROOT directory> <output plot directory> <input BLUEFORS_LOG directory> <input MultimeterData directory> <input R_BaseData directory> <input CelsiusData directory> -flags" << std::endl;
        std::cout << "  Mainflags:" << std::endl;
        std::cout << "      -ROOT           Convert all data to ROOT files" << std::endl;
        std::cout << "      -Plot           Plot all data" << std::endl;
        std::cout << "  flags:" << std::endl;
        std::cout << "      -NIROOT         Convert NIDAQ data to ROOT files" << std::endl;
        std::cout << "      -BFROOT         Convert BlueFors thermometer data to ROOT files" << std::endl;
        std::cout << "      -MultiROOT      Convert Multimeter data to ROOT files" << std::endl;
        std::cout << "      -RBaseROOT      Convert R_Base data to ROOT files" << std::endl;
        std::cout << "      -LabTempROOT    Convert Lab temperature data to ROOT files" << std::endl;
        std::cout << "      -NIPlot         plot NIDAQ data" << std::endl;
        std::cout << "      -NIFFTPlot      plot NIDAQ FFT" << std::endl;
        std::cout << "      -BFPlot         plot BlueFors data" << std::endl;
        std::cout << "      -MultiPlot      plot Multimeter data" << std::endl;
        std::cout << "      -RBasePlot      plot R_Base data" << std::endl;
        std::cout << "      -LabTempPlot    plot Lab temperature" << std::endl;
        return 1;
    }

    // 从命令行参数中获取文件路径
    std::filesystem::path NIDAQcsvDirPath = argv[1];
    std::filesystem::path rootDirPath = argv[2];
    std::filesystem::path plotDirPath = argv[3];
    std::filesystem::path BlueforsLogDirPath = argv[4];
    std::filesystem::path MultimeterDataDirPath = argv[5];
    std::filesystem::path R_BaseDataDirPath = argv[6];
    std::filesystem::path CelsiusDataDirPath = argv[7];

    // 检查路径是否存在且是一个目录
    if ( !std::filesystem::exists( NIDAQcsvDirPath ) || !std::filesystem::is_directory( NIDAQcsvDirPath ) )
    {
        std::cout << "Error: NI DAQ CSV directory " << NIDAQcsvDirPath << " does not exist or is not a directory." << std::endl;
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
    if ( !std::filesystem::exists( BlueforsLogDirPath ) || !std::filesystem::is_directory( BlueforsLogDirPath ) )
    {
        std::cout << "Error: BLUEFORS_LOG directory " << BlueforsLogDirPath << " does not exist or is not a directory." << std::endl;
        return 1;
    }
    if ( !std::filesystem::exists( MultimeterDataDirPath ) || !std::filesystem::is_directory( MultimeterDataDirPath ) )
    {
        std::cout << "Error: MultimeterDataDirPath directory " << MultimeterDataDirPath << " does not exist or is not a directory." << std::endl;
        return 1;
    }
    if ( !std::filesystem::exists( R_BaseDataDirPath ) || !std::filesystem::is_directory( R_BaseDataDirPath ) )
    {
        std::cout << "Error: R_BaseDataDirPath directory " << R_BaseDataDirPath << " does not exist or is not a directory." << std::endl;
        return 1;
    }
    if ( !std::filesystem::exists( CelsiusDataDirPath ) || !std::filesystem::is_directory( CelsiusDataDirPath ) )
    {
        std::cout << "Error: CelsiusDataDirPath directory " << CelsiusDataDirPath << " does not exist or is not a directory." << std::endl;
        return 1;
    }

    flagHandler( NIDAQcsvDirPath, rootDirPath, plotDirPath, BlueforsLogDirPath, MultimeterDataDirPath, R_BaseDataDirPath, CelsiusDataDirPath, argc, argv );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
