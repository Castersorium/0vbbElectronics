#pragma once

#ifndef CONVERT2TTREE_HPP
#define CONVERT2TTREE_HPP

namespace TTREEIO
{
class convert2TTree
{
public:
    convert2TTree() : isDebugModeActive( false ), startDate( "24-04-01" ), endDate( "24-04-01" ) {}  // ���캯������ʼ��isDebugModeActiveΪfalse
    void setDebug( bool debugStatus ) { isDebugModeActive = debugStatus; }  // ����debug״̬
    void setDateInterval( const std::string & start, const std::string & end ) { startDate = start; endDate = end; }  // �������ڷ�Χ
    // ����һ�����������ڽ�NIDAQCSV��ʽ������ת��ΪTTree��ʽ
    void convertNIDAQCSV2TTree( const std::string & csvDirPath, const std::string & rootFileName ) const;
    // ����һ�����������ڽ�BlueforsLog��ʽ������ת��ΪTTree��ʽ
    void convertBlueforsTemperatureLog2TTree( const std::string & BlueforsLogDirPath, const std::string & rootFileName ) const;

protected:
    bool isDebugModeActive;  // debug״̬

private:
    std::string startDate;
    std::string endDate;
};
}

#endif // CONVERT2TTREE_HPP
