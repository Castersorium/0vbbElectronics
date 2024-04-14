#pragma once

#ifndef CONVERT2TTREE_HPP
#define CONVERT2TTREE_HPP

namespace TTREEIO
{
class convert2TTree
{
public:
    convert2TTree() : isDebugModeActive( false ) {}  // ���캯������ʼ��isDebugModeActiveΪfalse
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // ����debug״̬
    // ����һ�����������ڽ�CSV��ʽ������ת��ΪTTree��ʽ
    void convertCSV2TTree( const std::string csvFileName, const std::string rootFileName );

protected:
    bool isDebugModeActive;  // debug״̬
};
}

#endif // CONVERT2TTREE_HPP
