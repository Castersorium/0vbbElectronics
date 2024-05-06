#!/usr/bin/bash
make clean_DATACVT
sleep 2
make dataConverter
./build/dataConverter/dataConverter.exe ./data/NIDAQ_csv/Run2404/ ./output/ROOTdata/ ./output/plots/ ./data/BLUEFORS_LOG/ ./data/multimeterdata/Run2404/
