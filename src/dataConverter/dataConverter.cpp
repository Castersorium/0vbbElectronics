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
    if ( argc < 3 )
    {
        std::cout << "Usage: " << argv[0] << " <input CSV file> <output ROOT file>" << std::endl;
        return 1;
    }

    // �������в����л�ȡ�ļ�·��
    std::filesystem::path csvFilePath = argv[1];
    std::filesystem::path rootFilePath = argv[2];

    // ���·���Ƿ��������һ��Ŀ¼
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

    // ����convert2TTree��ʵ��
    std::unique_ptr< TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // ��debugģʽ
    myConverter->setDebug( true );

    // ����Ŀ¼�µ������ļ�
    //for ( const auto & entry : std::filesystem::directory_iterator( csvFilePath ) )
    //{
    //    std::cout << "File path:" << entry.path() << std::endl;
    //    // ����convertCSV2TTree������ת������
    //    myConverter->convertCSV2TTree( "data.csv", rootFilePath_str + "./data.root" );
    //}
    myConverter->convertCSV2TTree( csvFilePath_str + "��¼-2024-04-02 091354 803.csv", rootFilePath_str + "./data.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
