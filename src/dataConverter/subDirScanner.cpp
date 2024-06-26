#include "flagHandler.hpp"
#include <iostream>
#include <filesystem>
#include "subDirScanner.hpp"

void subDirScan(
    const std::filesystem::path &baseDirPath,
    const std::filesystem::path &rootDirPath,
    const std::filesystem::path &plotDirPath,
    const std::filesystem::path &BlueforsLogDirPath,
    const std::filesystem::path &MultimeterDataDirPath,
    const std::filesystem::path &CelsiusDataDirPath,
    int argc, char *argv[])
{
    bool foundCSV = false;
    for (const auto &entry : std::filesystem::directory_iterator(baseDirPath))
    {
        if (entry.is_directory())
        {
            std::filesystem::path NIDAQcsvDirPath = entry.path();

            std::cout << "Processing directory: " << NIDAQcsvDirPath << std::endl;

            // 调用 flagHandler 处理每个子文件夹中的数据
            flagHandler(NIDAQcsvDirPath, rootDirPath, plotDirPath, BlueforsLogDirPath, MultimeterDataDirPath, CelsiusDataDirPath, argc, argv);
        }
        else if (entry.is_regular_file() && entry.path().extension() == ".csv")
        {
            foundCSV = true; // Set flag to true if CSV file is found
        }
    }
    if (foundCSV)
    {
        // Call flagHandler only if CSV file is found
        flagHandler(baseDirPath, rootDirPath, plotDirPath, BlueforsLogDirPath, MultimeterDataDirPath, CelsiusDataDirPath, argc, argv);
    }
}