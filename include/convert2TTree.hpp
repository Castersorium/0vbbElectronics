#pragma once

#ifndef CONVERT2TTREE_HPP
#define CONVERT2TTREE_HPP

namespace TTREEIO
{
class convert2TTree
{
public:
    convert2TTree() : isDebugModeActive( false ), startDate( "24-04-01" ), endDate( "24-04-01" ) {}  // 构造函数，初始化isDebugModeActive为false
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // 设置debug状态
    void setDateInterval( const std::string & start, const std::string & end ) { startDate = start; endDate = end; }  // 设置日期范围
    // 定义一个函数，用于将NIDAQCSV格式的数据转换为TTree格式
    void convertNIDAQCSV2TTree( const std::string & csvDirPath, const std::string & rootFileName ) const;
    // 定义一个函数，用于将BlueforsLog格式的数据转换为TTree格式
    void convertBlueforsTemperatureLog2TTree( const std::string & BlueforsLogDirPath, const std::string & rootFileName ) const;

protected:
    bool isDebugModeActive;  // debug状态

private:
    std::string startDate;
    std::string endDate;
};
}

#endif // CONVERT2TTREE_HPP
