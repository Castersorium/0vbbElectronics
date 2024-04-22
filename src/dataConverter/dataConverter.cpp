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
    // ����Ƿ��ṩ���ļ�·��
    if ( argc < 6 )
    {
        std::cout << "Usage: " << argv[0] << " <input NIDAQCSV directory> <output ROOT directory> <output plot directory> <input BLUEFORS_LOG directory> <input MultimeterData directory>" << std::endl;
        return 1;
    }

    // �������в����л�ȡ�ļ�·��
    std::filesystem::path NIDAQcsvDirPath = argv[1];
    std::filesystem::path rootDirPath = argv[2];
    std::filesystem::path plotDirPath = argv[3];
    std::filesystem::path BlueforsLogDirPath = argv[4];
    std::filesystem::path MultimeterDataDirPath = argv[5];

    // ���·���Ƿ��������һ��Ŀ¼
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

    // ����convert2TTree��ʵ��
    std::unique_ptr<TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // ��debugģʽ
    //myConverter->setDebug( true );

    // ת��NIDAQCSV�ļ���ROOT�ļ�
    myConverter->convertNIDAQCSV2TTree( NIDAQcsvDirPath.string(), rootDirPath.string() + "/NIDAQ_data.root" );

    myConverter->setDateInterval( "24-04-01", "24-04-15" );

    // ת��BlueforsTemperatureLog�ļ���ROOT�ļ�
    myConverter->convertBlueforsTemperatureLog2TTree( BlueforsLogDirPath.string(), rootDirPath.string() + "/BLUEFORS_Temperature_data.root" );

    // ת��MultimeterData�ļ���ROOT�ļ�
    myConverter->convertMultimeterData2TTree( MultimeterDataDirPath.string(), rootDirPath.string() + "/Multimeter_data.root" );

    // ����TTreePlotter��ʵ��
    std::unique_ptr<TTREEIO::TTreePlotter> myPlotter = std::make_unique<TTREEIO::TTreePlotter>();

    // ��debugģʽ
    //myPlotter->setDebug( true );

    myPlotter->setAmpHistoBinWidth( 0.01 );
    myPlotter->setTimeWindow( 2.0 );

    // ��ROOT�ļ�����TGraphErrors�����浽ROOT�ļ�
    myPlotter->createNIDAQGraphFromTree( rootDirPath.string() + "/NIDAQ_data.root", plotDirPath.string() + "/NIDAQ_plot.root" );

    myPlotter->setTimeWindow( 300.0 );

    // ��ROOT�ļ�����TGraphErrors�����浽ROOT�ļ�
    myPlotter->createBlueforsTemperatureGraphFromTree( rootDirPath.string() + "/BLUEFORS_Temperature_data.root", plotDirPath.string() + "/BLUEFORS_Temperature_plot.root" );
    
    myPlotter->createMultimeterGraphFromTree( rootDirPath.string() + "/Multimeter_data.root", plotDirPath.string() + "/Multimeter_plot.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
