#pragma once

#ifndef TTREEPLOTTER_HPP
#define TTREEPLOTTER_HPP

#include <string> // C++ header

namespace TTREEIO
{
class TTreePlotter
{
public:
    // ���캯������ʼ��isDebugModeActiveΪfalse��timeWindowΪ1.0��xBinWidthΪ0.02��xMinΪ-10��xMaxΪ10
    TTreePlotter() : isDebugModeActive( false ), timeWindow( 1.0 ), xBinWidth( 0.02 ), xMin( -10.0 ), xMax( 10.0 ) {}
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // ����debug״̬
    void setAmpHistoBinWidth( double newBinWidth ) { xBinWidth = newBinWidth; }  // ���÷���ֱ��ͼ��bin���
    void setTimeWindow( double newTimeWindow ) { timeWindow = newTimeWindow; }  // �����¼��������ڵķ�Χ
    // ��TTree����TGraphErrors�����浽ROOT�ļ��ķ���
    void createGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath );

protected:
    bool isDebugModeActive;  // debug״̬

private:
    double timeWindow;  // �¼��������ڵķ�Χ
    double xBinWidth;  // ����ֱ��ͼ��bin���
    double xMin;  // ����ֱ��ͼ����Сֵ
    double xMax;  // ����ֱ��ͼ�����ֵ
};
}

#endif // TTREEPLOTTER_HPP
