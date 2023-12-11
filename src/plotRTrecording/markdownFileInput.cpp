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

} // namespace MDFIO
