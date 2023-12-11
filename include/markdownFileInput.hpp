#pragma once

#ifndef MARKDOWNFILEINPUT_HPP
#define MARKDOWNFILEINPUT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

namespace MDFIO
{

class markdownFileInput
{
protected:
    std::vector<std::string> tableContent;

public:
    void readMarkdownTable( const std::string & filename );
    void printTableContent() const;
};

} // namespace MDFIO

#endif // MARKDOWNFILEINPUT_HPP
