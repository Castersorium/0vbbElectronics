#include "markdownFileInput.hpp"

//#define DEBUGGING

namespace MDFIO
{

void markdownFileInput::readMarkdownTable( const std::string & filename )
{
    std::ifstream file( filename );
    std::string line;
    while ( std::getline( file, line ) )
    {
        // Check if the line contains a Markdown table separator
        if ( line.find( '|' ) != std::string::npos )
        {
            tableContent.push_back( line );
        }
    }
    file.close();
}

void markdownFileInput::printTableContent() const
{
    for ( const auto & line : tableContent )
    {
        std::cout << line << std::endl;
    }
}

void markdownFileInput::readGeneralInformation()
{
    // 假设通道名称、引脚编号、探测器名称和注释都在前五行，第二行是分隔符行
    if ( tableContent.size() >= 5 )
    {
        std::istringstream channelStream( tableContent[0] );
        // 跳过第二行（分隔符行）
        std::istringstream pinStream( tableContent[2] );
        std::istringstream detectorStream( tableContent[3] );
        std::istringstream commentStream( tableContent[4] );

        std::string token;
        bool isCommentSection = false; // 用于跟踪是否到达注释部分
        while ( std::getline( channelStream, token, '|' ) )
        {
            token = trim( token ); // 使用新的trim函数去除头尾空格
            if ( !token.empty() )
            {
                channelNames.push_back( token );
            }
        }
        // 忽略分隔符行，不进行处理
        while ( std::getline( pinStream, token, '|' ) )
        {
            token = trim( token ); // 使用新的trim函数去除头尾空格
            if ( !token.empty() && token.find_first_not_of( " \t\r" ) != std::string::npos )
            {
                pinNumbers.push_back( token );
            }
        }
        while ( std::getline( detectorStream, token, '|' ) )
        {
            token = trim( token ); // 使用新的trim函数去除头尾空格
            // 确保字符串不为空且包含非空白字符
            if ( !token.empty() && token.find_first_not_of( " \t\r" ) != std::string::npos )
            {
                detectorNames.push_back( token );
#ifdef DEBUGGING
                std::cout << "detectorNames pushed back: {" << token << "}" << std::endl;
#endif // DEBUGGING
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
                    // 只有在注释部分才添加到detectorComments
                    // 确保字符串不为空且包含非空白字符
                    if ( token.find_first_not_of( " \t\r" ) != std::string::npos )
                    {
                        detectorComments.push_back( token );
#ifdef DEBUGGING
                        std::cout << "detectorComments pushed back: {" << token << "}" << std::endl;
#endif // DEBUGGING
                    }
                }
            }
        }
    }
}

void markdownFileInput::printGeneralInformation( unsigned int channel_num ) const
{
    // 构建通道名称字符串，如"CH7", "CH8"等
    std::string channelName = "CH" + std::to_string( channel_num );

    // 查找通道名称的索引
    auto it = std::find( channelNames.begin(), channelNames.end(), channelName );
    if ( it != channelNames.end() )
    {
        unsigned int index = std::distance( channelNames.begin(), it );
        if ( index < channelNames.size() )
        {
            std::cout << "Channel: " << channelNames[index] << std::endl;
            std::cout << "DB25-Pin: " << pinNumbers[index] << std::endl;
            std::cout << "Detector: " << detectorNames[index] << std::endl;
            std::cout << "Comment: " << detectorComments[index] << std::endl;
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
}


std::string markdownFileInput::trim( const std::string & str )
{
    size_t first = str.find_first_not_of( " \t\r" );
    if ( first == std::string::npos )
        return "";
    size_t last = str.find_last_not_of( " \t\r" );
    return str.substr( first, ( last - first + 1 ) );
}

} // namespace MDFIO
