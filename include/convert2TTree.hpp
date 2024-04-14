#pragma once

#ifndef CONVERT2TTREE_HPP
#define CONVERT2TTREE_HPP

namespace TTREEIO
{
class convert2TTree
{
public:
    convert2TTree() : isDebugModeActive( false ) {}  // 构造函数，初始化isDebugModeActive为false
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // 设置debug状态
    // 定义一个函数，用于将CSV格式的数据转换为TTree格式
    void convertCSV2TTree( const std::string csvFileName, const std::string rootFileName );

protected:
    bool isDebugModeActive;  // debug状态
};
}

#endif // CONVERT2TTREE_HPP
