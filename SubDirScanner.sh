#!/bin/bash

# 检查是否提供了baseDir作为命令行参数
if [ -z "$1" ]; then
  echo "Usage: $0 <base_directory>"
fi

# 读取命令行参数
baseDir="$1"

./build/dataConverter/dataConverter.exe \
    "$baseDir" \
    /mnt/d/storage/research/data/dataConverterTest/ROOT \
    /mnt/d/storage/research/data/dataConverterTest/Plots \
    /mnt/d/storage/research/data/dataConverterTest/BFTemp \
    /mnt/d/storage/research/data/dataConverterTest/Multimeter \
    /mnt/d/storage/research/data/dataConverterTest/LabTemp \
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
            /mnt/d/storage/research/data/dataConverterTest/ROOT \
            /mnt/d/storage/research/data/dataConverterTest/Plots \
            /mnt/d/storage/research/data/dataConverterTest/BFTemp \
            /mnt/d/storage/research/data/dataConverterTest/Multimeter \
            /mnt/d/storage/research/data/dataConverterTest/LabTemp \
            -NIROOT \
            -NIPlot \
            -NIFFTPlot \
    )
done