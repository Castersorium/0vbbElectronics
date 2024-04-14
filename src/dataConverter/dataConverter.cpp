#include <iostream> // C++ header
#include <memory>

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header

int main()
{
    // ���������Կ�������Ҫʹ�õ����к���
    // ���磬����Ե���convertTDMS2TTree������ת������
    std::unique_ptr< TTREEIO::convert2TTree> myConverter = std::make_unique<TTREEIO::convert2TTree>();

    // ��debugģʽ
    myConverter->setDebug( true );

    // ����convertTDMS2TTree������ת������
    myConverter->convertTDMS2TTree( "data.tdms", "data.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
