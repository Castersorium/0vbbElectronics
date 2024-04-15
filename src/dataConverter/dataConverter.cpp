#include <iostream> // C++ header
#include <memory>
#include <filesystem>
#include <string>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header

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

    // ת��CSV�ļ���ROOT�ļ�
    myConverter->convertCSV2TTree( csvDirPath.string(), rootDirPath.string() + "/data.root" );

    // ������������Ļ�ͼ���룬ʹ��plotDirPath��Ϊ���Ŀ¼

    std::cout << "Hello, my project." << std::endl;

    return 0;
}