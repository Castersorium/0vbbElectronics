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
        std::cout << "Usage: " << argv[0] << " <input TDMS file> <output ROOT file>" << std::endl;
        return 1;
    }

    // �������в����л�ȡ�ļ�·��
    std::filesystem::path tdmsFilePath = argv[1];
    std::filesystem::path rootFilePath = argv[2];

    // ���·���Ƿ��������һ��Ŀ¼
    if ( !std::filesystem::exists( tdmsFilePath ) || !std::filesystem::is_directory( tdmsFilePath ) )
    {
        std::cout << "Error: TDMS path " << tdmsFilePath << "does not exist or is not a directory." << std::endl;
        return 1;
    }
    if ( !std::filesystem::exists( rootFilePath ) || !std::filesystem::is_directory( rootFilePath ) )
    {
        std::cout << "Error: ROOT path " << rootFilePath << "does not exist or is not a directory." << std::endl;
        return 1;
    }
    std::string rootFilePath_str = rootFilePath.string();

    // ����convert2TTree��ʵ��
    std::unique_ptr< TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // ��debugģʽ
    myConverter->setDebug( true );

    // ����Ŀ¼�µ������ļ�
    for ( const auto & entry : std::filesystem::directory_iterator( tdmsFilePath ) )
    {
        std::cout << "File path:" << entry.path() << std::endl;
        // ����convertTDMS2TTree������ת������
        myConverter->convertTDMS2TTree( "data.tdms", rootFilePath_str + "./data.root" );
    }

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
