#pragma once

#ifndef TTREEPLOTTER_HPP
#define TTREEPLOTTER_HPP

#include <string> // C++ header

namespace TTREEIO
{
class TTreePlotter
{
public:
    // 构造函数，初始化isDebugModeActive为false，timeWindow为1.0，xBinWidth为0.02，xMin为-10，xMax为10
    TTreePlotter() : isDebugModeActive( false ), timeWindow( 1.0 ), xBinWidth( 0.02 ), xMin( -10.0 ), xMax( 10.0 ) {}
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // 设置debug状态
    void setAmpHistoBinWidth( double newBinWidth ) { xBinWidth = newBinWidth; }  // 设置幅度直方图的bin宽度
     // 设置事件采样窗口的范围（单位是秒）
    void setTimeWindow( double newTimeWindow ) { timeWindow = newTimeWindow; } 
    // 从TTree创建TGraphErrors并保存到ROOT文件的方法
    void createNIDAQGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const;
    void createBlueforsTemperatureGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const;
    void createMultimeterGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath ) const;

protected:
    bool isDebugModeActive;  // debug状态

private:
    double timeWindow;  // 事件采样窗口的范围
    double xBinWidth;  // 幅度直方图的bin宽度
    double xMin;  // 幅度直方图的最小值
    double xMax;  // 幅度直方图的最大值

    // 创建一个颜色的枚举类
    enum Color { kRed, kBlue, kGreen, kMagenta, kCyan, kYellow, kBlack, kOrange };
};
}

#endif // TTREEPLOTTER_HPP

