#pragma once

#ifndef FLAG_HANDLER_HPP
#define FLAG_HANDLER_HPP

#include <filesystem>

void flagHandler(
    const std::filesystem::path &NIDAQcsvDirPath,
    const std::filesystem::path &rootDirPath,
    const std::filesystem::path &plotDirPath,
    const std::filesystem::path &BlueforsLogDirPath,
    const std::filesystem::path &MultimeterDataDirPath,
    const std::filesystem::path &CelsiusDataDirPath,
    int argc, char *argv[]);

#endif