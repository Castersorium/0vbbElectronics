#include "markdownTableAnalyzer.hpp" // my header

//#define DEBUGGING
//#define DEBUGGINGVERBOSE

namespace MDFIO
{

size_t markdownTableAnalyzer::getChannelNames_string_vec_size() const
{
    return channelNames_string_vec.size();
}

size_t markdownTableAnalyzer::getPinNumbers_string_vec_size() const
{
    return pinNumbers_string_vec.size();
}

size_t markdownTableAnalyzer::getDetectorNames_string_vec_size() const
{
    return detectorNames_string_vec.size();
}

size_t markdownTableAnalyzer::getDetectorComments_string_vec_size() const
{
    return detectorComments_string_vec.size();
}

// 返回channelNames_string_vec的引用
const std::vector<std::string> & markdownTableAnalyzer::getChannelNames() const
{
    return channelNames_string_vec;
}

// 返回pinNumbers_string_vec的引用
const std::vector<std::string> & markdownTableAnalyzer::getPinNumbers() const
{
    return pinNumbers_string_vec;
}

// 返回detectorNames_string_vec的引用
const std::vector<std::string> & markdownTableAnalyzer::getDetectorNames() const
{
    return detectorNames_string_vec;
}

// 返回detectorComments_string_vec的引用
const std::vector<std::string> & markdownTableAnalyzer::getDetectorComments() const
{
    return detectorComments_string_vec;
}

void markdownTableAnalyzer::readGeneralInformation()
{
    // 假设通道名称、引脚编号、探测器名称和注释都在前五行，第二行是分隔符行
    if ( tableContent_string_vec.size() >= 5 )
    {
        std::istringstream channelStream( tableContent_string_vec[0] );
        // 跳过第二行（分隔符行）
        std::istringstream pinStream( tableContent_string_vec[2] );
        std::istringstream detectorStream( tableContent_string_vec[3] );
        std::istringstream commentStream( tableContent_string_vec[4] );

        std::string token;
        bool isCommentSection = false; // 用于跟踪是否到达注释部分
        while ( std::getline( channelStream, token, '|' ) )
        {
            token = trim( token ); // 使用新的trim函数去除头尾空格
            if ( !token.empty() )
            {
                channelNames_string_vec.push_back( token );
            }
        }
        // 忽略分隔符行，不进行处理
        while ( std::getline( pinStream, token, '|' ) )
        {
            token = trim( token ); // 使用新的trim函数去除头尾空格
            if ( !token.empty() && token.find_first_not_of( " \t\r" ) != std::string::npos )
            {
                pinNumbers_string_vec.push_back( token );
            }
        }
        while ( std::getline( detectorStream, token, '|' ) )
        {
            token = trim( token ); // 使用新的trim函数去除头尾空格
            // 确保字符串不为空且包含非空白字符
            if ( !token.empty() && token.find_first_not_of( " \t\r" ) != std::string::npos )
            {
                detectorNames_string_vec.push_back( token );
#ifdef DEBUGGINGVERBOSE
                std::cout << "detectorNames_string_vec pushed back: \"" << token << "\"" << std::endl;
#endif // DEBUGGINGVERBOSE
            }
        }
        while ( std::getline( commentStream, token, '|' ) )
        {
            token = trim( token ); // 使用新的trim函数去除头尾空格
            // 确保字符串不为空
            if ( !token.empty() )
            {
                // 如果已经在注释部分，或者遇到第一个"Comment"
                if ( isCommentSection || token == "Comment" )
                {
                    if ( token == "Comment" && !isCommentSection )
                    {
                        isCommentSection = true; // 当遇到第一个"Comment"时，标记为注释部分开始
                        continue; // 跳过这个"Comment"标题
                    }
                    // 只有在注释部分才添加到detectorComments_string_vec
                    // 确保字符串不为空且包含非空白字符
                    if ( token.find_first_not_of( " \t\r" ) != std::string::npos )
                    {
                        detectorComments_string_vec.push_back( token );
#ifdef DEBUGGINGVERBOSE
                        std::cout << "detectorComments_string_vec pushed back: \"" << token << "\"" << std::endl;
#endif // DEBUGGINGVERBOSE
                    }
                }
            }
        }
    }
}

void markdownTableAnalyzer::printGeneralInformation( unsigned int channel_num ) const
{
    std::cout << "=======Printing General Information ..." << std::endl;
    // 构建通道名称字符串，如"CH7", "CH8"等
    std::string channelName = "CH" + std::to_string( channel_num );

    // 查找通道名称的索引
    auto it = std::find( channelNames_string_vec.begin(), channelNames_string_vec.end(), channelName );
    if ( it != channelNames_string_vec.end() )
    {
        unsigned int index = std::distance( channelNames_string_vec.begin(), it );
        if ( index < channelNames_string_vec.size() )
        {
            std::cout << "Channel : " << channelNames_string_vec[index] << std::endl;
            std::cout << "DB25-Pin: " << pinNumbers_string_vec[index] << std::endl;
            std::cout << "Detector: " << detectorNames_string_vec[index] << std::endl;
            std::cout << "Comment : " << detectorComments_string_vec[index] << std::endl;
        }
        else
        {
            std::cerr << "Invalid channel number." << std::endl;
        }
    }
    else
    {
        std::cerr << "Channel " << channelName << " not found." << std::endl;
    }
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
}

std::string markdownTableAnalyzer::trim( const std::string & str )
{
    size_t first = str.find_first_not_of( " \t\r" );
    if ( first == std::string::npos )
        return "";
    size_t last = str.find_last_not_of( " \t\r" );
    return str.substr( first, ( last - first + 1 ) );
}

void markdownTableAnalyzer::readMeasurementEntries()
{
    // 从tableContent_string_vec[5]开始读取测量数据
    for ( size_t i = 5; i < tableContent_string_vec.size(); ++i )
    {
        std::istringstream entryStream( tableContent_string_vec[i] );
        std::string token;
        std::vector<std::string> entryTokens;

        // 将每一行分割成单独的字符串并存储在entryTokens中
        while ( std::getline( entryStream, token, '|' ) )
        {
            token = trim( token ); // 使用trim函数去除头尾空格
            // 确保字符串不为空且包含非空白字符
            if ( token.find_first_not_of( " \t\r" ) != std::string::npos )
            {
                entryTokens.push_back( token );
#ifdef DEBUGGINGVERBOSE
                std::cout << "entryTokens pushed back: \"" << token << "\"" << std::endl;
#endif // DEBUGGINGVERBOSE
            }
        }

#ifdef DEBUGGING
        std::cout << "entryTokens size: \"" << entryTokens.size() << "\"" << std::endl;
        std::cout << "channelNames_string_vec size: \"" << channelNames_string_vec.size() << "\"" << std::endl;
        std::cout << "checking size: \"" << static_cast<unsigned long>( 7 ) + channelNames_string_vec.size() << "\"" << std::endl;
#endif // DEBUGGING

        // 检查entryTokens的大小是否正确
        if ( entryTokens.size() == static_cast<unsigned long>( 7 ) + channelNames_string_vec.size() )
        {
            date_string_vec.push_back( entryTokens[0] );
            time_string_vec.push_back( entryTokens[1] );
            location_string_vec.push_back( entryTokens[2] );
            interface_string_vec.push_back( entryTokens[3] );
            measuringPosition_string_vec.push_back( entryTokens[4] );
            temperature_string_vec.push_back( entryTokens[5] );
            comment_string_vec.push_back( entryTokens[6] );

            // 检查"R [Ohm]"字段是否存在
            if ( entryTokens[7] == "R [Ohm]" )
            {
                // 从第8个token开始，按顺序读取电阻值
                for ( size_t j = 0; j + 1 < channelNames_string_vec.size(); ++j )
                {
                    std::string resistanceValue = entryTokens[8 + j];
                    // 根据通道名称将电阻值存储到相应的向量中
                    if ( channelNames_string_vec[j + 1] == "CH7" )
                    {
                        resistance_ch7_string_vec.push_back( resistanceValue );
                    }
                    else if ( channelNames_string_vec[j + 1] == "CH8" )
                    {
                        resistance_ch8_string_vec.push_back( resistanceValue );
                    }
                    else if ( channelNames_string_vec[j + 1] == "CH9" )
                    {
                        resistance_ch9_string_vec.push_back( resistanceValue );
                    }
                    else if ( channelNames_string_vec[j + 1] == "CH10" )
                    {
                        resistance_ch10_string_vec.push_back( resistanceValue );
                    }
                    else if ( channelNames_string_vec[j + 1] == "CH11" )
                    {
                        resistance_ch11_string_vec.push_back( resistanceValue );
                    }
                    else if ( channelNames_string_vec[j + 1] == "CH12" )
                    {
                        resistance_ch12_string_vec.push_back( resistanceValue );
                    }
                }
            }
            else
            {
                std::cerr << "Missing \"R [Ohm]\" field at line " << i << std::endl;
            }
        }
        else
        {
            std::cerr << "Invalid entry format at line " << i << std::endl;
        }
    }
}

void markdownTableAnalyzer::printMeasurementEntry( unsigned int entry_num ) const
{
    std::cout << "=======Printing Measurement Entry #" << entry_num << " ..." << std::endl;
    if ( entry_num < date_string_vec.size() )
    {
        std::cout << "Date              : " << date_string_vec[entry_num] << std::endl;
        std::cout << "Time              : " << time_string_vec[entry_num] << std::endl;
        std::cout << "Location          : " << location_string_vec[entry_num] << std::endl;
        std::cout << "Interface         : " << interface_string_vec[entry_num] << std::endl;
        std::cout << "Measuring Position: " << measuringPosition_string_vec[entry_num] << std::endl;
        std::cout << "Temperature [K]   : " << temperature_string_vec[entry_num] << std::endl;
        std::cout << "Comment           : " << comment_string_vec[entry_num] << std::endl;

        // 打印每个通道的阻值大小
        std::cout << "-----------Resistance Values:" << std::endl;
        if ( entry_num < resistance_ch7_string_vec.size() )
        {
            std::cout << "CH7  [Ohm]: " << resistance_ch7_string_vec[entry_num] << std::endl;
        }
        if ( entry_num < resistance_ch8_string_vec.size() )
        {
            std::cout << "CH8  [Ohm]: " << resistance_ch8_string_vec[entry_num] << std::endl;
        }
        if ( entry_num < resistance_ch9_string_vec.size() )
        {
            std::cout << "CH9  [Ohm]: " << resistance_ch9_string_vec[entry_num] << std::endl;
        }
        if ( entry_num < resistance_ch10_string_vec.size() )
        {
            std::cout << "CH10 [Ohm]: " << resistance_ch10_string_vec[entry_num] << std::endl;
        }
        if ( entry_num < resistance_ch11_string_vec.size() )
        {
            std::cout << "CH11 [Ohm]: " << resistance_ch11_string_vec[entry_num] << std::endl;
        }
        if ( entry_num < resistance_ch12_string_vec.size() )
        {
            std::cout << "CH12 [Ohm]: " << resistance_ch12_string_vec[entry_num] << std::endl;
        }
        std::cout << "-----------------------------" << std::endl;
    }
    else
    {
        std::cerr << "Invalid entry number." << std::endl;
    }
    std::cout << "=======================================" << std::endl;
    std::cout << std::endl;
}

} // namespace MDFIO

