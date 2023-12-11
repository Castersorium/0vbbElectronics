#include <iostream> // C++ header

#include "markdownFileInput.hpp" // my header

int main( int argc, char * argv[] )
{
    // ����Ƿ��ṩ���ļ�·��
    if ( argc < 2 )
    {
        std::cerr << "Usage: " << argv[0] << " <markdown_file_path>" << std::endl;
        return EXIT_FAILURE;
    }

    // ��ȡ�ļ�·��
    std::string filePath = argv[1];

    // ����markdownFileInput���ʵ������ȡ�ļ�
    MDFIO::markdownFileInput mdfInput;
    mdfInput.readMarkdownTable( filePath );
    mdfInput.readGeneralInformation();
    // ��ӡͨ��9�Ļ�����Ϣ
    mdfInput.printGeneralInformation( 9 );

    return 0;
}
