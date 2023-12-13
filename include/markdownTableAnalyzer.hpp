#pragma once

#ifndef MARKDOWNTABLEANALYZER_HPP
#define MARKDOWNTABLEANALYZER_HPP

#include <algorithm> // C++ header

#include "markdownFileInput.hpp" // my header

namespace MDFIO
{

// 派生类：负责分析Markdown表格数据
class markdownTableAnalyzer
    : public markdownFileInput
{
public:
    size_t getChannelNames_string_vec_size() const;
    size_t getPinNumbers_string_vec_size() const;
    size_t getDetectorNames_string_vec_size() const;
    size_t getDetectorComments_string_vec_size() const;

public:
    const std::vector<std::string> & getChannelNames() const;
    const std::vector<std::string> & getPinNumbers() const;
    const std::vector<std::string> & getDetectorNames() const;
    const std::vector<std::string> & getDetectorComments() const;

private:
    std::vector<std::string> channelNames_string_vec;
    std::vector<std::string> pinNumbers_string_vec;
    std::vector<std::string> detectorNames_string_vec;
    std::vector<std::string> detectorComments_string_vec;

public:
    void readGeneralInformation();
    void printGeneralInformation( unsigned int channel_num ) const;

private:
    std::string trim( const std::string & str );

protected:
    std::vector<std::string> date_string_vec;
    std::vector<std::string> time_string_vec;
    std::vector<std::string> location_string_vec;
    std::vector<std::string> interface_string_vec;
    std::vector<std::string> measuringPosition_string_vec;
    std::vector<std::string> temperature_string_vec;
    std::vector<std::string> comment_string_vec;

protected:
    std::vector<std::string> resistance_ch7_string_vec;
    std::vector<std::string> resistance_ch8_string_vec;
    std::vector<std::string> resistance_ch9_string_vec;
    std::vector<std::string> resistance_ch10_string_vec;
    std::vector<std::string> resistance_ch11_string_vec;
    std::vector<std::string> resistance_ch12_string_vec;

public:
    void readMeasurementEntries();
    void printMeasurementEntry( unsigned int entry_num ) const;
};

} // namespace MDFIO

#endif // MARKDOWNTABLEANALYZER_HPP
