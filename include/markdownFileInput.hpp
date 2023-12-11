#pragma once

#ifndef MARKDOWNFILEINPUT_HPP
#define MARKDOWNFILEINPUT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace MDFIO
{

// 基类：负责读取Markdown表格内容
class markdownFileInput
{
protected:
    std::vector<std::string> tableContent_string_vec;

public:
    void readMarkdownTable( const std::string & filename );
    void printTableContent() const;
};

} // namespace MDFIO

#endif // MARKDOWNFILEINPUT_HPP
