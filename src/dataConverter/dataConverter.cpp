#include <iostream> // C++header

#include <TFile.h> // ROOT header
#include <TTree.h>

// ����һ�����������ڽ�TDMS��ʽ������ת��ΪTTree��ʽ
void convertTDMS2TTree( const char * tdmsFileName, const char * rootFileName )
{
    // ���������Ҫ��Ӵ�����ʵ�����ݵ�ת��
    // ���磬�������Ҫ��TDMS�ļ�����ȡ���ݣ�Ȼ������д�뵽ROOT��TTree��
}

int main()
{
    // ���������Կ�������Ҫʹ�õ����к���
    // ���磬����Ե���convertTDMS2TTree������ת������
    convertTDMS2TTree( "data.tdms", "data.root" );

    std::cout << "Hello, my project." << std::endl;

    return 0;
}
