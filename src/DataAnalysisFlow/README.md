# Data Processing Workflows

## For Electronic R&D Workflow
- **`run.m`**: Use this as the default running C++ script.

### From LOG to LOG
- **`LS_mergy.py`**: Merge multiple LS logs into one log.

### From XLSX to FIG
- **`IVcurve.m`**: Draw I-V curve using load curve.

### Keyu_RoverT - Temperature and Resistance Relationship
- **`Keyu_RoverT`** (file1): Reads a single file with the required format: Temperature (T), Resistance (R).
- **`Keyu_RoverT`** (file1, file2): Reads two LS files with the default format, order: Temperature (T), Resistance (R).

### Keyu_getLCTR - LC Resistance Measurement
- **`Keyu_getLCTR()`**: Measures LC resistance and outputs to the default file `LC_TR.log`.
- **`Keyu_getLCTR(file.mat, channel, chi_th, T)`**: Measures resistance from a single file and channel at a given temperature.
- **`Keyu_getLCTR(file.mat, channel, chi_th)`**: Measures resistance from multiple files and a single channel, automatically detecting temperature based on filenames (*PID*mK*).

### Keyu_SNR - Peak Detection in LC
- **`Keyu_SNR`**: Detects peaks in LC, requires setting period, stability time, etc.


## For Octopus Workflow

### From TDMS to BIN
- **`Convert_tdms2bin.py`**: Converts the TDMS file to a binary file.

### From BIN to BIN
- **`Mergy_bin.py`**: Merges binary files together.

### From ROOT to FIG
- **`Heater_draw.py`**: Draw deposited energy amplitudes and find the zero energy point for the heater.
- **`octopus_2D.cxx`**: Draw a 2D plot.
- **`octopus_cutflow.cxx`**: Check the cut flow of our cut criteria.
- **`octopus_pro.cxx`**: Plot the variable with the specified criteria.
- **`octopus_2in1.cxx`**: Compare two variables in one plot.
- **`octopus_shape.cxx`**: Draw two histograms directly from ROOT in one plot.
- **`octopus_rdtime.cxx`**: Determine the rise time/decay time of a specific region from a histogram.

## For Kangkang's R&D Workflow
- **`run.m`**: Use this as the default running C++ script.

### From DIR to ROOT
- **`Kangkang_Trigger.py`**: Trigger data stream using Kangkang's code.

### From ROOT to FIG
- **`draw_standard.cxx`**: Compare the same variable in two ROOT files with the same name.
- **`draw_multi.cxx`**: Compare the same variable in three ROOT files with the same name.
- **`draw_compare.cxx`**: Compare the same variable in two ROOT files with different names, together with the diff-hist, initially aiming to compare Kangkang's two trigger ROOT files.
