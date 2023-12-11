# Electronics testing for 0vbb experiment

## data



### `Resistance_measurement.md`

- Table which recorded measurement conditions and other details
- Currently all of the data was acquired through Milan electronics

1. [2023Nov](https://github.com/Castersorium/0vbbElectronics/blob/master/data/2023Nov/Resistance_measurement.md)
2. [2023Dec](https://github.com/Castersorium/0vbbElectronics/blob/master/data/2023Dec/Resistance_measurement.md)(latest)

### `*.txt`

- Column1: V_bol(V)
- Column2: I_bol(A)

### `*.xlsx`

- Raw Data generated by Matlab GUI
- The result is stored in above *.txt file

### electronics

- Saved stream data for electronics testing.
- Data are acquired by NI DAQ. Raw data format was converted to `*.csv`, which are in general friendly to most analysis tools.

#### about `README.md`

In `README.md`, usually saving the front-end configuration.

Here is the list of testing done:

- chassis_test -> data20231207
    - This is a noise measurement after restoring the internal PCB wiring of the electronics chassis.
    - We selected a rated resistor with a fixed resistance of 1 MOhm and connected to the input of the front-end through direct insertion.
    - The purpose is to test the noise level brought only by the chassis in a situation similar to real data acquisition.
    - We did not enable a Bessel filter in order to see the true baseline fluctuations.
    - Detailed configuration: [README.md](https://github.com/Castersorium/0vbbElectronics/blob/master/data/electronics/streamdata/chassis_test/data20231207/README.md)

## doc

## output

- Graphs and Fit results. 
- For newest result, you can check [RTCurve_20-100mK.png](https://github.com/Castersorium/0vbbElectronics/blob/master/output/2023Nov/RTCurve_20-100mK.png) or [RTCurve_20-50mK.png](https://github.com/Castersorium/0vbbElectronics/blob/master/output/2023Nov/RTCurve_20-50mK.png)

### `RTCurve.xlsx`

- Preliminary results for temperature fit of RT Curve

## src

### `plotGraph.C`

- Draw the plots from data
- Use linear fit $$V = R_\mathrm{bol} \cdot I + C$$
- Results are stored in output
- The excluded range that was not fit was due to electronics saturation

### `plotSinGraph.C`

- Draw the plots from data
- The data is fluctuated caused by **PID** controlling phase
- Fit the result with the function $$V = R_\mathrm{bol} \cdot I + Amp \cdot I \cdot \sin(\omega I + \varphi) + C$$
- Results are stored in output
- $\max{R_\mathrm{bol}}=R_\mathrm{bol} + Amp$, at the lowest temperature
- $\min{R_\mathrm{bol}}=R_\mathrm{bol} - Amp$, at the highest temperature

## directory tree
<!-- TREE START -->
```
.
├── README.md
├── build
│   └── plotRTrecording
│       └── plotRTrecording.exe
├── data
│   ├── 2023Dec
│   │   ├── Resistance_measurement.md
│   │   └── Resistance_measurement.root
│   ├── 2023Nov
│   │   ├── 20p0mK
│   │   │   ├── 19T20.txt
│   │   │   ├── 20231109T175700.xlsx
│   │   │   ├── 3x1NTD.txt
│   │   │   └── 3x3NTD.txt
│   │   ├── 22p2mK
│   │   │   ├── 19T20.txt
│   │   │   ├── 20231107T093800.xlsx
│   │   │   ├── 3x1NTD.txt
│   │   │   └── 3x3NTD.txt
│   │   ├── 29p9mK
│   │   │   ├── 19T20.txt
│   │   │   ├── 20231107T184800.xlsx
│   │   │   ├── 3x1NTD.txt
│   │   │   └── 3x3NTD.txt
│   │   ├── 39p9mK_PIDbad
│   │   │   ├── 19T20.txt
│   │   │   ├── 20231108T140000.xlsx
│   │   │   ├── 3x1NTD.txt
│   │   │   └── 3x3NTD.txt
│   │   └── Resistance_measurement.md
│   └── electronics
│       └── streamdata
│           └── chassis_test
│               └── data20231207
│                   ├── MatlabGUI_config2023-12-07.png
│                   ├── README.md
│                   └── frontEndConfig.json
├── doc
│   ├── doc.tmp
│   ├── prefix
│   │   └── COM_configure.md
│   ├── slides
│   │   ├── 电子学测试20231026_20231026211355.pdf
│   │   ├── 电子学测试20231110_20231110143147.pdf
│   │   └── 电子学测试20231208_20231208115638.pdf
│   └── test.sh
├── include
│   ├── ROOTDataSaver.hpp
│   ├── markdownFileInput.hpp
│   ├── markdownTableAnalyzer.hpp
│   └── markdownTableDataExtractor.hpp
├── output
│   └── 2023Nov
│       ├── 20p0mK
│       │   ├── allGraphs.pdf
│       │   └── outputFit.root
│       ├── 22p2mK
│       │   ├── allGraphs.pdf
│       │   └── outputFit.root
│       ├── 29p9mK
│       │   ├── allGraphs.pdf
│       │   └── outputFit.root
│       ├── 39p9mK_PIDbad
│       │   ├── 19T20@38-42mK.jpg
│       │   ├── allGraphs.pdf
│       │   ├── output.root
│       │   ├── outputFit.root
│       │   ├── outputFuAnQuXian.root
│       │   └── testFit.root
│       ├── RTCurve.xlsx
│       ├── RTCurve_20-100mK.png
│       ├── RTCurve_20-50mK.png
│       └── RTCurve_20-50mK.svg
├── photo
│   └── logo_cupid_alpha_beta_transparent.png
├── src
│   ├── CMakeLists.txt
│   ├── CMakePresets.json
│   ├── out
│   ├── plotGraph.C
│   ├── plotRTrecording
│   │   ├── CMakeLists.txt
│   │   ├── ROOTDataSaver.cpp
│   │   ├── markdownFileInput.cpp
│   │   ├── markdownTableAnalyzer.cpp
│   │   ├── markdownTableDataExtractor.cpp
│   │   └── plotRTrecording.cpp
│   └── plotSinGraph.C
└── wrapUp.sh

27 directories, 62 files
```
<!-- TREE END -->

# END
