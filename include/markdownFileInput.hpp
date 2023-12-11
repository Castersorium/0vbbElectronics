#pragma once

#ifndef MARKDOWNFILEINPUT_HPP
#define MARKDOWNFILEINPUT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace MDFIO
{

class markdownFileInput
{
protected:
    std::vector<std::string> tableContent_string_vec;

public:
    void readMarkdownTable( const std::string & filename );
    void printTableContent() const;

protected:
    std::vector<std::string> channelNames_string_vec;
    std::vector<std::string> pinNumbers_string_vec;
    std::vector<std::string> detectorNames_string_vec;
    std::vector<std::string> detectorComments_string_vec;

public:
    void readGeneralInformation();
    void printGeneralInformation( unsigned int channel_num ) const;

private:
    std::string trim( const std::string & str );

private:
    std::vector<std::string> date_string_vec;
    std::vector<std::string> time_string_vec;
    std::vector<std::string> location_string_vec;
    std::vector<std::string> interface_string_vec;
    std::vector<std::string> measuringPosition_string_vec;
    std::vector<std::string> temperature_string_vec;
    std::vector<std::string> comment_string_vec;

private:
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

#endif // MARKDOWNFILEINPUT_HPP

