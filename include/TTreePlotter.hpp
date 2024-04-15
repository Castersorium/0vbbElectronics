#pragma once

#ifndef TTREEPLOTTER_HPP
#define TTREEPLOTTER_HPP

#include <string> // C++ header

namespace TTREEIO
{
class TTreePlotter
{
public:
    TTreePlotter() : isDebugModeActive( false ) {}  // ���캯������ʼ��isDebugModeActiveΪfalse
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // ����debug״̬
    // ��TTree����TGraphErrors�����浽ROOT�ļ��ķ���
    void createGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath );

protected:
    bool isDebugModeActive;  // debug״̬
};
}

#endif // TTREEPLOTTER_HPP
