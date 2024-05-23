#include <iostream>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

// 解析文件名，提取日期和时间部分
std::tm parseFilename(const std::string &filename)
{
    // 调整正则表达式以匹配文件名中的日期和时间部分
    std::regex filenameRegex(R"(记录-(\d{4})-(\d{2})-(\d{2}) (\d{2})(\d{2})(\d{2}))");
    std::smatch match;
    if (std::regex_search(filename, match, filenameRegex))
    {
        std::tm tm = {};
        tm.tm_year = std::stoi(match[1]) - 1900;
        tm.tm_mon = std::stoi(match[2]) - 1;
        tm.tm_mday = std::stoi(match[3]);
        tm.tm_hour = std::stoi(match[4]);
        tm.tm_min = std::stoi(match[5]);
        tm.tm_sec = std::stoi(match[6]);
        return tm;
    }
    throw std::runtime_error("Filename does not match the expected pattern.");
}

// 增加小时数
std::tm addHours(const std::tm &tm, int hours)
{
    std::time_t time = std::mktime(const_cast<std::tm *>(&tm));
    time += hours * 3600; // 增加小时数
    std::tm *newTm = std::localtime(&time);
    return *newTm;
}

// 格式化文件夹名称
std::string formatFolderName(const std::tm &tm)
{
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

// 创建文件夹并移动文件
void createAndMoveFiles(const fs::path &directory)
{
    for (const auto &entry : fs::directory_iterator(directory))
    {
        if (entry.is_regular_file())
        {
            std::string filename = entry.path().filename().string();
            try
            {
                std::tm fileTm = parseFilename(filename);
                fileTm = addHours(fileTm, 8);
                std::string folderName = formatFolderName(fileTm);

                fs::path newFolder = directory / folderName;
                if (!fs::exists(newFolder))
                {
                    fs::create_directory(newFolder);
                }
                fs::rename(entry.path(), newFolder / filename);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error processing file " << filename << ": " << e.what() << std::endl;
                std::cerr << "Regex pattern did not match for filename: " << filename << std::endl;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <directory>" << std::endl;
        return 1;
    }

    fs::path directory = argv[1];
    if (!fs::exists(directory) || !fs::is_directory(directory))
    {
        std::cerr << "The specified path is not a directory or does not exist." << std::endl;
        return 1;
    }

    try
    {
        createAndMoveFiles(directory);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Files have been successfully categorized." << std::endl;
    return 0;
}
