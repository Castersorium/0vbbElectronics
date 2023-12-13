#pragma once

#ifndef MARKDOWNTABLEDATAEXTRACTOR_HPP
#define MARKDOWNTABLEDATAEXTRACTOR_HPP

#include "markdownTableAnalyzer.hpp" // my header

namespace MDFIO
{

class markdownTableDataExtractor
    : public markdownTableAnalyzer
{
public:
    const std::vector<double> & getTemperature() const;
    const std::vector<double> & getTemperature_err() const;

private:
    std::vector<double> temperature_d_vec;
    std::vector<double> temperature_err_d_vec;

public:
    void convertTemperature();

public:
    const std::vector<double> & getResistance_ch7() const;
    const std::vector<double> & getResistance_ch8() const;
    const std::vector<double> & getResistance_ch9() const;
    const std::vector<double> & getResistance_ch10() const;
    const std::vector<double> & getResistance_ch11() const;
    const std::vector<double> & getResistance_ch12() const;

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
