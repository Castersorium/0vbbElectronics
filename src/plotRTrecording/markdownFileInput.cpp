#include "markdownFileInput.hpp"

//#define DEBUGGING
//#define DEBUGGINGVERBOSE

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
            tableContent_string_vec.push_back( line );
        }
    }
    file.close();
}

void markdownFileInput::printTableContent() const
{
    for ( const auto & line : tableContent_string_vec )
    {
        std::cout << line << std::endl;
    }
}

void markdownFileInput::readGeneralInformation()
{
    // ����ͨ�����ơ����ű�š�̽�������ƺ�ע�Ͷ���ǰ���У��ڶ����Ƿָ�����
    if ( tableContent_string_vec.size() >= 5 )
    {
        std::istringstream channelStream( tableContent_string_vec[0] );
        // �����ڶ��У��ָ����У�
        std::istringstream pinStream( tableContent_string_vec[2] );
        std::istringstream detectorStream( tableContent_string_vec[3] );
        std::istringstream commentStream( tableContent_string_vec[4] );

        std::string token;
        bool isCommentSection = false; // ���ڸ����Ƿ񵽴�ע�Ͳ���
        while ( std::getline( channelStream, token, '|' ) )
        {
            token = trim( token ); // ʹ���µ�trim����ȥ��ͷβ�ո�
            if ( !token.empty() )
            {
                channelNames_string_vec.push_back( token );
            }
        }
        // ���Էָ����У������д���
        while ( std::getline( pinStream, token, '|' ) )
        {
            token = trim( token ); // ʹ���µ�trim����ȥ��ͷβ�ո�
            if ( !token.empty() && token.find_first_not_of( " \t\r" ) != std::string::npos )
            {
                pinNumbers_string_vec.push_back( token );
            }
        }
        while ( std::getline( detectorStream, token, '|' ) )
        {
            token = trim( token ); // ʹ���µ�trim����ȥ��ͷβ�ո�
            // ȷ���ַ�����Ϊ���Ұ����ǿհ��ַ�
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
                    // ֻ����ע�Ͳ��ֲ���ӵ�detectorComments_string_vec
                    // ȷ���ַ�����Ϊ���Ұ����ǿհ��ַ�
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

void markdownFileInput::printGeneralInformation( unsigned int channel_num ) const
{
    std::cout << "=======Printing General Information ..." << std::endl;
    // ����ͨ�������ַ�������"CH7", "CH8"��
    std::string channelName = "CH" + std::to_string( channel_num );

    // ����ͨ�����Ƶ�����
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

std::string markdownFileInput::trim( const std::string & str )
{
    size_t first = str.find_first_not_of( " \t\r" );
    if ( first == std::string::npos )
        return "";
    size_t last = str.find_last_not_of( " \t\r" );
    return str.substr( first, ( last - first + 1 ) );
}

void markdownFileInput::readMeasurementEntries()
{
    // ��tableContent_string_vec[5]��ʼ��ȡ��������
    for ( size_t i = 5; i < tableContent_string_vec.size(); ++i )
    {
        std::istringstream entryStream( tableContent_string_vec[i] );
        std::string token;
        std::vector<std::string> entryTokens;

        // ��ÿһ�зָ�ɵ������ַ������洢��entryTokens��
        while ( std::getline( entryStream, token, '|' ) )
        {
            token = trim( token ); // ʹ��trim����ȥ��ͷβ�ո�
            // ȷ���ַ�����Ϊ���Ұ����ǿհ��ַ�
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

        // ���entryTokens�Ĵ�С�Ƿ���ȷ
        if ( entryTokens.size() == static_cast<unsigned long>( 7 ) + channelNames_string_vec.size() )
        {
            date_string_vec.push_back( entryTokens[0] );
            time_string_vec.push_back( entryTokens[1] );
            location_string_vec.push_back( entryTokens[2] );
            interface_string_vec.push_back( entryTokens[3] );
            measuringPosition_string_vec.push_back( entryTokens[4] );
            temperature_string_vec.push_back( entryTokens[5] );
            comment_string_vec.push_back( entryTokens[6] );

            // ���"R [Ohm]"�ֶ��Ƿ����
            if ( entryTokens[7] == "R [Ohm]" )
            {
                // �ӵ�8��token��ʼ����˳���ȡ����ֵ
                for ( size_t j = 0; j < channelNames_string_vec.size(); ++j )
                {
                    std::string resistanceValue = entryTokens[8 + j];
                    // ����ͨ�����ƽ�����ֵ�洢����Ӧ��������
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

void markdownFileInput::printMeasurementEntry( unsigned int entry_num ) const
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

        // ��ӡÿ��ͨ������ֵ��С
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
