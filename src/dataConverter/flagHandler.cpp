#include <iostream> // C++ header
#include <memory>
#include <filesystem>
#include <string>
#include <unordered_map>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header
#include "TTreePlotter.hpp"
#include "flagHandler.hpp"

void flagHandler(
    const std::filesystem::path & NIDAQcsvDirPath,
    const std::filesystem::path & rootDirPath,
    const std::filesystem::path & plotDirPath,
    const std::filesystem::path & BlueforsLogDirPath,
    const std::filesystem::path & MultimeterDataDirPath,
    const std::filesystem::path & R_BaseDataDirPath,
    const std::filesystem::path & CelsiusDataDirPath,
    int argc, char * argv[] )
{

    // // 打印所有命令行参数
    // std::cout << "Command line arguments:" << std::endl;
    // for (int i = 0; i < argc; ++i) {
    //     std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    // }

    // 定义flag
    std::unordered_map<std::string, bool> flags = {
        {"-ROOT", false},
        {"-NIROOT", false},
        {"-BFROOT", false},
        {"-MultiROOT", false},
        {"-R_BaseROOT", false},
        {"-LabTempROOT", false},

        {"-Plot", false},
        {"-NIPlot", false},
        {"-NIFFTPlot", false},
        {"-BFPlot", false},
        {"-MultiPlot", false},
        {"-R_BasePlot", false},
        {"-LabTempPlot", false},
    };

    // 解析flag
    for ( int i = 8; i < argc; ++i )
    {
        std::string arg = argv[i];
        // 去除任何潜在的空格或不可见字符
        arg.erase( remove_if( arg.begin(), arg.end(), isspace ), arg.end() );
        // std::cout << "Parsing flag: " << arg << std::endl; // 添加调试输出
        if ( flags.find( arg ) != flags.end() )
        {
            flags[arg] = true;
        }
        else
        {
            // std::cout << "Unknown flag: " << arg << std::endl;
        }
    }

    // 打印所有flag的状态
    std::cout << "Flag status:" << std::endl;
    for ( const auto & flag : flags )
    {
        std::cout << flag.first << ": " << ( flag.second ? "true" : "false" ) << std::endl;
    }

    // 创建convert2TTree的实例
    std::unique_ptr<TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // 打开debug模式
    //myConverter->setDebug( true );

    std::string DateNIDAQ = NIDAQcsvDirPath.filename().string();
    std::string outputNIData = rootDirPath.string() + "/NIDAQ_data_" + DateNIDAQ + ".root";
    std::string outputNIPlot = plotDirPath.string() + "/NIDAQ_plot_" + DateNIDAQ + ".root";
    std::string outputNIFFT = plotDirPath.string() + "/NIDAQ_FFT_plot_" + DateNIDAQ + ".root";

    if ( flags["-ROOT"] || flags["-NIROOT"] )
    {
        // std::cout << "Entering -ROOT or -NIROOT block" << std::endl;
        // 转换NIDAQCSV文件到ROOT文件
        myConverter->convertNIDAQCSV2TTree( NIDAQcsvDirPath.string(), outputNIData );
    }

    if ( flags["-ROOT"] || flags["-BFROOT"] )
    {
        myConverter->setDateInterval( "24-04-01", "24-05-30" );
        // 转换BlueforsTemperatureLog文件到ROOT文件
        myConverter->convertBlueforsTemperatureLog2TTree( BlueforsLogDirPath.string(), rootDirPath.string() + "/BLUEFORS_Temperature_data.root" );
        // 转换BlueforsPressureLog文件到ROOT文件
        myConverter->convertBlueforsPressureLog2TTree( BlueforsLogDirPath.string(), rootDirPath.string() + "/BLUEFORS_Pressure_data.root" );
        // 转换BlueforsFlowLog文件到ROOT文件
        myConverter->convertBlueforsFlowLog2TTree( BlueforsLogDirPath.string(), rootDirPath.string() + "/BLUEFORS_Flow_data.root" );
    }

    if ( flags["-ROOT"] || flags["-MultiROOT"] )
    {
        // 打开debug模式
        //myConverter->setDebug( true );
        // 转换MultimeterData文件到ROOT文件
        myConverter->convertMultimeterData2TTree( MultimeterDataDirPath.string(), rootDirPath.string() + "/Multimeter_data.root" );
    }

    if ( flags["-ROOT"] || flags["-R_BaseROOT"] )
    {
        // 打开debug模式
        //myConverter->setDebug( true );
        // 转换R_BaseData文件到ROOT文件
        myConverter->convertR_BaseData2TTree( R_BaseDataDirPath.string(), rootDirPath.string() + "/R_Base_data.root" );
    }

    if ( flags["-ROOT"] || flags["-LabTempROOT"] )
    {
        // 转换CelsiusData文件到ROOT文件
        myConverter->convertCelsiusData2TTree( CelsiusDataDirPath.string(), rootDirPath.string() + "/Celsius_data.root" );
    }

    if ( flags["-Plot"] || flags["-NIPlot"] || flags["-NIFFTPlot"] || flags["-BFPlot"] || flags["-MultiPlot"] || flags["-R_BasePlot"] || flags["-LabTempPlot"] )
    {
        // 创建TTreePlotter的实例
        std::unique_ptr<TTREEIO::TTreePlotter> myPlotter = std::make_unique<TTREEIO::TTreePlotter>();

        // 打开debug模式
        // myPlotter->setDebug( true );
        if ( flags["-Plot"] || flags["-NIPlot"] )
        {
            myPlotter->setAmpHistoBinWidth( 0.01 );
            myPlotter->setTimeWindow( 2.0 );

            // 从ROOT文件创建TGraphErrors并保存到ROOT文件
            myPlotter->createNIDAQGraphFromTree( outputNIData, outputNIPlot );
        }

        if ( flags["-Plot"] || flags["-NIFFTPlot"] )
        {
            myPlotter->setAmpHistoBinWidth( 0.001 ); // 设置采样间隔
            myPlotter->setFFTTimeWindow( 2.0 );
            myPlotter->setTimeWindow( 4.0 );

            myPlotter->createNIDAQFFTFromTree( outputNIData, outputNIFFT );
        }

        if ( flags["-Plot"] || flags["-BFPlot"] )
        {
            myPlotter->setTimeWindow( 300.0 );

            // 从ROOT文件创建TGraphErrors并保存到ROOT文件
            myPlotter->createBlueforsTemperatureGraphFromTree( rootDirPath.string() + "/BLUEFORS_Temperature_data.root", plotDirPath.string() + "/BLUEFORS_Temperature_plot.root" );
            // 从ROOT文件创建TGraphErrors并保存到ROOT文件
            myPlotter->createBlueforsPressureGraphFromTree( rootDirPath.string() + "/BLUEFORS_Pressure_data.root", plotDirPath.string() + "/BLUEFORS_Pressure_plot.root" );
            // 从ROOT文件创建TGraphErrors并保存到ROOT文件
            myPlotter->createBlueforsFlowGraphFromTree( rootDirPath.string() + "/BLUEFORS_Flow_data.root", plotDirPath.string() + "/BLUEFORS_Flow_plot.root" );
        }

        if ( flags["-Plot"] || flags["-MultiPlot"] )
        {
            myPlotter->setTimeWindow( 3000.0 );

            myPlotter->createMultimeterGraphFromTree( rootDirPath.string() + "/Multimeter_data.root", plotDirPath.string() + "/Multimeter_plot.root" );
        }

        if ( flags["-Plot"] || flags["-R_BasePlot"] )
        {
            // 打开debug模式
            myPlotter->setDebug( true );
            myPlotter->setTimeWindow( 3000.0 );

            myPlotter->createR_BaseGraphFromTree( rootDirPath.string() + "/R_Base_data.root", plotDirPath.string() + "/R_Base_plot.root" );
        }

        if ( flags["-Plot"] || flags["-LabTempPlot"] )
        {
            myPlotter->setTimeWindow( 3000.0 );

            // 从ROOT文件创建TGraphErrors并保存到ROOT文件
            myPlotter->createCelsiusGraphFromTree( rootDirPath.string() + "/Celsius_data.root", plotDirPath.string() + "/Celsius_plot.root" );
        }
    }
}
