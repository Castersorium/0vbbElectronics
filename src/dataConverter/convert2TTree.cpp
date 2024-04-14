#include <iostream> // C++ header

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "convert2TTree.hpp" // my header

namespace TTREEIO
{
// ����һ�����������ڽ�TDMS��ʽ������ת��ΪTTree��ʽ
void convert2TTree::convertTDMS2TTree( const char * tdmsFileName, const char * rootFileName )
{
    // ����һ���µ�ROOT�ļ�
    TFile * file = new TFile( rootFileName, "RECREATE" );

    // ����һ���µ�TTree
    TTree * tree = new TTree( "tree", "A tree with two branches" );

    // ���isDebugModeActive״̬Ϊtrue����ӡ��һ����Ϣ
    if ( isDebugModeActive )
    {
        std::cout << " - Output root file created: " << rootFileName << std::endl;
    }

    // �����������洢branch��ֵ
    double timestamp = 0;
    double amplitude = 0;

    // ��������branch
    tree->Branch( "timestamp", &timestamp, "timestamp/D" );
    tree->Branch( "amplitude", &amplitude, "amplitude/D" );

    // ���������Ҫ��Ӵ�������ȡTDMS�ļ�������
    // Ȼ�󣬶���ÿһ�����ݣ�����Ҫ����timestamp��amplitude��ֵ��Ȼ�����TTree
    // ���磺
    // timestamp = ...;  // ����timestamp��ֵ
    // amplitude = ...;  // ����amplitude��ֵ
    // tree->Fill();     // ���TTree

    // ��TTreeд�뵽ROOT�ļ���
    tree->Write();

    // �ر�ROOT�ļ�
    delete file;
}
}
