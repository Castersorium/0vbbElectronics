# 🎉 **Data Processing Workflows** 🎉

## 📝 **Outline**
1. [For Electronic R&D Workflow](#1for-electronic-rd-workflow)

    1.1. [From LOG to LOG](#11-from-log-to-log)  
    1.2. [From XLSX to FIG](#12-from-xlsx-to-fig)  
    1.3. [Keyu_RoverT - Temperature and Resistance Relationship](#13-keyurovert-temperature-and-resistance-relationship)  
    1.4. [Keyu_getLCTR - LC Resistance Measurement](#14-keyugetlctr-lc-resistance-measurement)  
    1.5. [Keyu_SNR - Peak Detection in LC](#15-keyusnr-peak-detection-in-lc)  

2. [For Octopus Workflow](#2for-octopus-workflow)

    2.1. [From TDMS to BIN](#21-from-tdms-to-bin)  
    2.2. [From BIN to BIN](#22-from-bin-to-bin)  
    2.3. [From ROOT to FIG](#23-from-root-to-fig)  

3. [For Kangkang's R&D Workflow](#3for-kangkangs-rd-workflow)

    3.1. [From DIR to ROOT](#31-from-dir-to-root)  
    3.2. [From ROOT to FIG](#32-from-root-to-fig)

---

## 1️⃣ **For Electronic R&D Workflow** ⚡️

- **`run.m`**: Run this as the default C++ script. 🚀

### 1.1 📜 **From LOG to LOG**
- **`LS_mergy.py`**: Merge multiple LS logs into one log. 📂 ➡️ 📄

### 1.2 📊 **From XLSX to FIG**
- **`IVcurve.m`**: Draw the I-V curve using the load curve. 📈

### 1.3 🌡️ **Keyu_RoverT - Temperature and Resistance Relationship**
- **`Keyu_RoverT`** (file1): Reads a single file with the required format: Temperature (T), Resistance (R). 🌡️ 🔌
- **`Keyu_RoverT`** (file1, file2): Reads two LS files with the default format, order: Temperature (T), Resistance (R). 🔄

### 1.4 🛠️ **Keyu_getLCTR - LC Resistance Measurement**
- **`Keyu_getLCTR()`**: Measures LC resistance and outputs to the default file `LC_TR.log`. 🔬
- **`Keyu_getLCTR(file.mat, channel, chi_th, T)`**: Measures resistance from a single file and channel at a given temperature. 📊
- **`Keyu_getLCTR(file.mat, channel, chi_th)`**: Measures resistance from multiple files and a single channel, automatically detecting temperature based on filenames (*PID*mK*). 📂🔍

### 1.5 🎯 **Keyu_SNR - Peak Detection in LC**
- **`Keyu_SNR`**: Detects peaks in LC, requires setting period, stability time, etc. 📈🔎

---

## 2️⃣ **For Octopus Workflow** 🐙

### 2.1 📄 **From TDMS to BIN**
- **`Convert_tdms2bin.py`**: Converts the TDMS file to a binary file. 💾➡️📊

### 2.2 🔄 **From BIN to BIN**
- **`Mergy_bin.py`**: Merges binary files together. 🛠️

### 2.3 🌍 **From ROOT to FIG**
- **`Heater_draw.py`**: Draw deposited energy amplitudes and find the zero energy point for the heater. 🔥📏
- **`octopus_2D.cxx`**: Draw a 2D plot. 📉
- **`octopus_cutflow.cxx`**: Check the cut flow of our cut criteria. ✂️
- **`octopus_pro.cxx`**: Plot the variable with the specified criteria. 📈🔍
- **`octopus_2in1.cxx`**: Compare two variables in one plot. 🔄📊
- **`octopus_shape.cxx`**: Draw two histograms directly from ROOT in one plot. 📊📊
- **`octopus_rdtime.cxx`**: Determine the rise time/decay time of a specific region from a histogram. ⏳
- **`octopus_heater.cxx`**: Use heater to stablize the baseline drift of data stream.

---

## 3️⃣ **For Kangkang's R&D Workflow** 👨‍🔬👩‍🔬

### 3.1 🔧 **From DIR to ROOT**
- **`Kangkang_Trigger.py`**: Trigger data stream using Kangkang's code. ⚙️💻

### 3.2 📉 **From ROOT to FIG**
- **`draw_standard.cxx`**: Compare the same variable in two ROOT files with the same name. 🔍📂
- **`draw_multi.cxx`**: Compare the same variable in three ROOT files with the same name. 🔄📊
- **`draw_compare.cxx`**: Compare the same variable in two ROOT files with different names, together with the diff-hist. Initially aiming to compare Kangkang's two trigger ROOT files. 🔄🔬
