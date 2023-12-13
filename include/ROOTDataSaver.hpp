#pragma once

#ifndef ROOTDATASAVER_HPP
#define ROOTDATASAVER_HPP

#include <memory> // C++ header

#include <TFile.h> // ROOT header
#include <TTree.h>

#include "markdownTableDataExtractor.hpp" // my header

namespace TTREEIO
{

class RootDataSaver
{
public:
    RootDataSaver( const MDFIO::markdownTableDataExtractor & extractor );
    void saveToRootFile( const std::string & filename );

private:
    const MDFIO::markdownTableDataExtractor & m_extractor;

private:
    std::vector<std::string> channelNames_string_vec;
    std::vector<std::string> pinNumbers_string_vec;
    std::vector<std::string> detectorNames_string_vec;
    std::vector<std::string> detectorComments_string_vec;

    std::vector<double> temperature_d_vec;
    std::vector<double> temperature_err_d_vec;

    std::vector<double> resistance_ch7_d_vec;
    std::vector<double> resistance_ch8_d_vec;
    std::vector<double> resistance_ch9_d_vec;
    std::vector<double> resistance_ch10_d_vec;
    std::vector<double> resistance_ch11_d_vec;
    std::vector<double> resistance_ch12_d_vec;
};

} // namespace TTREEIO

#endif // ROOTDATASAVER_HPP

