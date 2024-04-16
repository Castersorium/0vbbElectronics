#include <stdexcept> // C++ header
#include <limits>

#include "markdownTableDataExtractor.hpp" // my header

namespace MDFIO
{

// 返回temperature_d_vec的引用
const std::vector<double> & markdownTableDataExtractor::getTemperature() const
{
    return temperature_d_vec;
}

// 返回temperature_err_d_vec的引用
const std::vector<double> & markdownTableDataExtractor::getTemperature_err() const
{
    return temperature_err_d_vec;
}

const std::vector<double> & markdownTableDataExtractor::getResistance_ch7() const
{
    return resistance_ch7_d_vec;
}

const std::vector<double> & markdownTableDataExtractor::getResistance_ch8() const
{
    return resistance_ch8_d_vec;
}

const std::vector<double> & markdownTableDataExtractor::getResistance_ch9() const
{
    return resistance_ch9_d_vec;
}

const std::vector<double> & markdownTableDataExtractor::getResistance_ch10() const
{
    return resistance_ch10_d_vec;
}

const std::vector<double> & markdownTableDataExtractor::getResistance_ch11() const
{
    return resistance_ch11_d_vec;
}

const std::vector<double> & markdownTableDataExtractor::getResistance_ch12() const
{
    return resistance_ch12_d_vec;
}

void markdownTableDataExtractor::convertTemperature()
{
    for ( const auto & str : temperature_string_vec )
    {
        double temperature;
        double temperature_err;

        if ( str == "---" )
        {
            temperature = std::numeric_limits<double>::quiet_NaN();
            temperature_err = std::numeric_limits<double>::quiet_NaN();
        }
        else if ( str.find( "room" ) != std::string::npos )
        {
            temperature = 295;
            temperature_err = 2;
        }
        else if ( str.find( "T0: " ) != std::string::npos )
        {
            std::string value_str = str.substr( str.find( "T0: " ) + 4 );
            temperature = std::stod( value_str );
            temperature_err = 1;
        }
        else if ( str.find( "T5: " ) != std::string::npos )
        {
            std::string value_str = str.substr( str.find( "T5: " ) + 4 );
            temperature = std::stod( value_str );
            if ( value_str.find( "m" ) != std::string::npos )
            {
                temperature *= 1e-3;
            }
            temperature_err = 0.1 * temperature;
        }
        else if ( str.find( "T6: " ) != std::string::npos )
        {
            std::string value_str = str.substr( str.find( "T6: " ) + 4 );
            temperature = std::stod( value_str );
            if ( value_str.find( "m" ) != std::string::npos )
            {
                temperature *= 1e-3;
            }
            temperature_err = 0.02 * temperature;
        }
        else
        {
            // 如果字符串不符合任何已知模式，可以抛出异常或者处理为未定义
            temperature = std::numeric_limits<double>::quiet_NaN();
            temperature_err = std::numeric_limits<double>::quiet_NaN();
        }

        temperature_d_vec.emplace_back( temperature );
        temperature_err_d_vec.emplace_back( temperature_err );
    }
}

void markdownTableDataExtractor::convertStringToDouble()
{
    // 定义一个lambda函数来转换字符串为double
    auto stringToDouble = []( std::string str )-> double
        {
            if ( str == "---" )
            {
                return std::numeric_limits<double>::quiet_NaN(); // 处理未定义的值
            }
            try
            {
                double value = std::stod( str );
                if ( str.find( "k" ) != std::string::npos )
                {
                    value *= 1e3;
                }
                else if ( str.find( "M" ) != std::string::npos )
                {
                    value *= 1e6;
                }
                else if ( str.find( "G" ) != std::string::npos )
                {
                    value *= 1e9;
                }
                return value;
            }
            catch ( const std::invalid_argument & e )
            {
                std::cerr << "Invalid argument: " << e.what() << std::endl;
                return std::numeric_limits<double>::quiet_NaN();
            }
            catch ( const std::out_of_range & e )
            {
                std::cerr << "Out of range: " << e.what() << std::endl;
                return std::numeric_limits<double>::quiet_NaN();
            }
        };

    // 使用lambda函数转换每个通道的字符串向量为double向量
    for ( const auto & str : resistance_ch7_string_vec )
    {
        resistance_ch7_d_vec.emplace_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch8_string_vec )
    {
        resistance_ch8_d_vec.emplace_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch9_string_vec )
    {
        resistance_ch9_d_vec.emplace_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch10_string_vec )
    {
        resistance_ch10_d_vec.emplace_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch11_string_vec )
    {
        resistance_ch11_d_vec.emplace_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch12_string_vec )
    {
        resistance_ch12_d_vec.emplace_back( stringToDouble( str ) );
    }
}

void markdownTableDataExtractor::printAllDoubleVectors() const
{
    printDoubleVector( temperature_d_vec, "Temperature              [K]" );
    printDoubleVector( temperature_err_d_vec, "Temperature uncertainty  [K]" );
    printDoubleVector( resistance_ch7_d_vec, "CH7  Resistance Values [Ohm]" );
    printDoubleVector( resistance_ch8_d_vec, "CH8  Resistance Values [Ohm]" );
    printDoubleVector( resistance_ch9_d_vec, "CH9  Resistance Values [Ohm]" );
    printDoubleVector( resistance_ch10_d_vec, "CH10 Resistance Values [Ohm]" );
    printDoubleVector( resistance_ch11_d_vec, "CH11 Resistance Values [Ohm]" );
    printDoubleVector( resistance_ch12_d_vec, "CH12 Resistance Values [Ohm]" );
}

void markdownTableDataExtractor::printDoubleVector( const std::vector<double> & vec, const std::string & name ) const
{
    std::cout << name << ": {";
    for ( size_t i = 0; i < vec.size(); ++i )
    {
        std::cout << vec[i];
        if ( i < vec.size() - 1 )
        {
            std::cout << ",\t";
        }
    }
    std::cout << "}" << std::endl;
}

} // namespace MDFIO

