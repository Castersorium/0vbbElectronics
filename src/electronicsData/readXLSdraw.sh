#!/bin/bash


# Check if 'uproot' is installed
if  pip show uproot &> /dev/null;
then
    echo "Uproot is installed, running program..."
    echo
    python extractXLS.py
else
    echo "Uproot is not installed, please try:"
    echo "pip intall uproot"
    echo "to continue to read the configuration file from excel"
fi

echo "Now saving and drawing the VA Graph in a root file"
make
../../build/electronicsData/*.exe