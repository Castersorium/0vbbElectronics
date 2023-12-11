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
        // 检查该行是否包含 Markdown 表格分隔符
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

} // namespace MDFIO
