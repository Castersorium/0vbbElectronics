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
    // ����ͨ�����ơ����ű�š�̽�������ƺ�ע�Ͷ���ǰ���У��ڶ����Ƿָ�����
    if ( tableContent.size() >= 5 )
    {
        std::istringstream channelStream( tableContent[0] );
        // �����ڶ��У��ָ����У�
        std::istringstream pinStream( tableContent[2] );
        std::istringstream detectorStream( tableContent[3] );
        std::istringstream commentStream( tableContent[4] );

        std::string token;
        bool isCommentSection = false; // ���ڸ����Ƿ񵽴�ע�Ͳ���
        while ( std::getline( channelStream, token, '|' ) )
        {
            token = trim( token ); // ʹ���µ�trim����ȥ��ͷβ�ո�
            if ( !token.empty() )
            {
                channelNames.push_back( token );
            }
        }
        // ���Էָ����У������д���
        while ( std::getline( pinStream, token, '|' ) )
        {
            token = trim( token ); // ʹ���µ�trim����ȥ��ͷβ�ո�
            if ( !token.empty() && token.find_first_not_of( " \t\r" ) != std::string::npos )
            {
                pinNumbers.push_back( token );
            }
        }
        while ( std::getline( detectorStream, token, '|' ) )
        {
            token = trim( token ); // ʹ���µ�trim����ȥ��ͷβ�ո�
            // ȷ���ַ�����Ϊ���Ұ����ǿհ��ַ�
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
            token = trim( token ); // ʹ���µ�trim����ȥ��ͷβ�ո�
            // ȷ���ַ�����Ϊ��
            if ( !token.empty() )
            {
                // ����Ѿ���ע�Ͳ��֣�����������һ��"Comment"
                if ( isCommentSection || token == "Comment" )
                {
                    if ( token == "Comment" && !isCommentSection )
                    {
                        isCommentSection = true; // ��������һ��"Comment"ʱ�����Ϊע�Ͳ��ֿ�ʼ
                        continue; // �������"Comment"����
                    }
                    // ֻ����ע�Ͳ��ֲ���ӵ�detectorComments
                    // ȷ���ַ�����Ϊ���Ұ����ǿհ��ַ�
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
    // ����ͨ�������ַ�������"CH7", "CH8"��
    std::string channelName = "CH" + std::to_string( channel_num );

    // ����ͨ�����Ƶ�����
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
