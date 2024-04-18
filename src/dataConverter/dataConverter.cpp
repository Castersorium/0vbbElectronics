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
    if ( argc < 4 )
    {
        std::cout << "Usage: " << argv[0] << " <input CSV directory> <output ROOT directory> <output plot directory>" << std::endl;
        return 1;
    }

    // �������в����л�ȡ�ļ�·��
    std::filesystem::path csvDirPath = argv[1];
    std::filesystem::path rootDirPath = argv[2];
    std::filesystem::path plotDirPath = argv[3];

    // ���·���Ƿ��������һ��Ŀ¼
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

    // ����convert2TTree��ʵ��
    std::unique_ptr<TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // ��debugģʽ
    //myConverter->setDebug(true);

    // ת��NIDAQCSV�ļ���ROOT�ļ�
    myConverter->convertNIDAQCSV2TTree( csvDirPath.string(), rootDirPath.string() + "/NIDAQ_data.root" );

    // ����TTreePlotter��ʵ��
    std::unique_ptr<TTREEIO::TTreePlotter> myPlotter = std::make_unique<TTREEIO::TTreePlotter>();

    // ��debugģʽ
    //myPlotter->setDebug(true);
    myPlotter->setAmpHistoBinWidth(0.01);
    myPlotter->setTimeWindow(2.0);

    // ��ROOT�ļ�����TGraphErrors�����浽ROOT�ļ�
    myPlotter->createNIDAQGraphFromTree( rootDirPath.string() + "/NIDAQ_data.root", plotDirPath.string() + "/NIDAQ_plot.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
