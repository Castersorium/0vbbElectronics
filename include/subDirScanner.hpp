#ifndef SUB_DIR_SCANNER_HPP
#define SUB_DIR_SCANNER_HPP

#include <filesystem>

void subDirScan(
    const std::filesystem::path &NIDAQcsvDirPath,
    const std::filesystem::path &rootDirPath,
    const std::filesystem::path &plotDirPath,
    const std::filesystem::path &BlueforsLogDirPath,
    const std::filesystem::path &MultimeterDataDirPath,
    const std::filesystem::path &CelsiusDataDirPath,
    int argc, char *argv[]);

#endif
