#pragma once

#ifndef TTREEPLOTTER_HPP
#define TTREEPLOTTER_HPP

#include <string> // C++ header

namespace TTREEIO
{
class TTreePlotter
{
public:
    TTreePlotter() : isDebugModeActive( false ) {}  // 构造函数，初始化isDebugModeActive为false
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // 设置debug状态
    // 从TTree创建TGraphErrors并保存到ROOT文件的方法
    void createGraphFromTree( const std::string & rootFilePath, const std::string & outputFilePath );

protected:
    bool isDebugModeActive;  // debug状态
};
}

#endif // TTREEPLOTTER_HPP
