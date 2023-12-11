#pragma once

#ifndef MARKDOWNTABLEDATAEXTRACTOR_HPP
#define MARKDOWNTABLEDATAEXTRACTOR_HPP

#include "markdownTableAnalyzer.hpp" // my header

namespace MDFIO
{

class markdownTableDataExtractor
    : public markdownTableAnalyzer
{
private:
    std::vector<double> temperature_d_vec;
    std::vector<double> temperature_err_d_vec;

public:
    void convertTemperature();

private:
    std::vector<double> resistance_ch7_d_vec;
    std::vector<double> resistance_ch8_d_vec;
    std::vector<double> resistance_ch9_d_vec;
    std::vector<double> resistance_ch10_d_vec;
    std::vector<double> resistance_ch11_d_vec;
    std::vector<double> resistance_ch12_d_vec;

public:
    void convertStringToDouble();
    void printAllDoubleVectors() const;

private:
    void printDoubleVector( const std::vector<double> & vec, const std::string & name ) const;
};

} // namespace MDFIO

#endif // MARKDOWNTABLEDATAEXTRACTOR_HPP
