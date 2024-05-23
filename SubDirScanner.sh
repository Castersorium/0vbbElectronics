#!/bin/bash

# 检查是否提供了baseDir作为命令行参数
if [ -z "$1" ]; then
  echo "Usage: $0 <RUN_directory>"
fi

# 读取命令行参数
runDir="$1"
baseDir="$runDir/Converted_Data/csv"

./build/dataConverter/dataConverter.exe \
    "$baseDir" \
    "$runDir/Converted_Data/root" \
    "$runDir/plots" \
    "$runDir/BLUEFORS_LOG" \
    "$runDir/Multimeter_data/AC220V" \
    "$runDir/Labtemperature" \
    -BFROOT \
    -BFPlot \
    -MultiROOT \
    -MultiPlot \
    -LabTempROOT \
    -LabTempPlot

# 使用 find 命令遍历子文件夹
find "$baseDir" -mindepth 1 -type d | while read -r dir; do
    (
        echo "Processing directory: $dir"
        
        ./build/dataConverter/dataConverter.exe \
            "$dir" \
            "$runDir/Converted_Data/root" \
            "$runDir/plots" \
            "$runDir/BLUEFORS_LOG" \
            "$runDir/Multimeter_data/AC220V" \
            "$runDir/Labtemperature" \
            -NIROOT \
            -NIPlot \
	        -NIFFTPlot \
    )
done
