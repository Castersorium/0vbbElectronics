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
    std::vector<std::string> tableContent;

public:
    void readMarkdownTable( const std::string & filename );
    void printTableContent() const;

protected:
    std::vector<std::string> channelNames;
    std::vector<std::string> pinNumbers;
    std::vector<std::string> detectorNames;
    std::vector<std::string> detectorComments;

public:
    void readGeneralInformation();
    void printGeneralInformation( unsigned int channel_num ) const;

private:
    std::string trim( const std::string & str );
};

} // namespace MDFIO

#endif // MARKDOWNFILEINPUT_HPP
