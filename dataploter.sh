#!/usr/bin/bash
make clean
sleep 2
make dataConverter
./build/dataConverter/dataConverter.exe ./data/NIDAQ_csv/Run2404/test/ ./output/ROOTdatatest/ ./output/plots/ ./data/BLUEFORS_LOG/ ./data/multimeterdata/Run2404/
