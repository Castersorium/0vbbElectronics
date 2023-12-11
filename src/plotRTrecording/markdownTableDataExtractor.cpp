#include <stdexcept> // C++ header
#include <limits>

#include "markdownTableDataExtractor.hpp" // my header

namespace MDFIO
{

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
        resistance_ch7_d_vec.push_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch8_string_vec )
    {
        resistance_ch8_d_vec.push_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch9_string_vec )
    {
        resistance_ch9_d_vec.push_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch10_string_vec )
    {
        resistance_ch10_d_vec.push_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch11_string_vec )
    {
        resistance_ch11_d_vec.push_back( stringToDouble( str ) );
    }
    for ( const auto & str : resistance_ch12_string_vec )
    {
        resistance_ch12_d_vec.push_back( stringToDouble( str ) );
    }
}

void markdownTableDataExtractor::printAllDoubleVectors() const
{
    printDoubleVector( resistance_ch7_d_vec, "CH7 " );
    printDoubleVector( resistance_ch8_d_vec, "CH8 " );
    printDoubleVector( resistance_ch9_d_vec, "CH9 " );
    printDoubleVector( resistance_ch10_d_vec, "CH10" );
    printDoubleVector( resistance_ch11_d_vec, "CH11" );
    printDoubleVector( resistance_ch12_d_vec, "CH12" );
}

void markdownTableDataExtractor::printDoubleVector( const std::vector<double> & vec, const std::string & name ) const
{
    std::cout << name << " Resistance Values [Ohm]: {";
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
