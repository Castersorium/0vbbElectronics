#!/bin/bash

# Check if 'uproot' is installed
check_uproot(){
    if  pip show uproot &> /dev/null;
    then
        echo "Uproot is installed, running program..."
        echo
        python extractXLS.py
    else
        echo "Uproot is not installed, please try:"
        echo "pip intall uproot"
        echo "This could to read the configuration file from excel"
        exit 1
    fi
}

if [ "$1" != "-nex" ]; then # nex = no need to extract
    check_uproot
fi

echo "Now saving and drawing the VA Graph in a root file"
make debug
../../build/electronicsData/*.exe
