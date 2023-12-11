#include "markdownFileInput.hpp"

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
    // 假设通道名称、引脚编号、探测器名称和注释都在前四行
    if ( tableContent.size() >= 4 )
    {
        std::istringstream channelStream( tableContent[0] );
        std::istringstream pinStream( tableContent[1] );
        std::istringstream detectorStream( tableContent[2] );
        std::istringstream commentStream( tableContent[3] );

        std::string token;
        while ( std::getline( channelStream, token, '|' ) )
        {
            channelNames.push_back( token );
        }
        while ( std::getline( pinStream, token, '|' ) )
        {
            pinNumbers.push_back( token );
        }
        while ( std::getline( detectorStream, token, '|' ) )
        {
            detectorNames.push_back( token );
        }
        while ( std::getline( commentStream, token, '|' ) )
        {
            detectorComments.push_back( token );
        }
    }
}

void markdownFileInput::printGeneralInformation( unsigned int channel_num ) const
{
    if ( channel_num < channelNames.size() )
    {
        std::cout << "Channel: " << channelNames[channel_num] << std::endl;
        std::cout << "DB25-Pin: " << pinNumbers[channel_num] << std::endl;
        std::cout << "Detector: " << detectorNames[channel_num] << std::endl;
        std::cout << "Comment: " << detectorComments[channel_num] << std::endl;
    }
    else
    {
        std::cerr << "Invalid channel number." << std::endl;
    }
}

} // namespace MDFIO
