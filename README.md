# Electronics testing for 0vbb experiment

## data

### `*.txt`

- Column1: V_bol(V)
- Column2: I_bol(A)

### `*.xlsx`

- Raw Data generated by Matlab GUI
- The result is stored in above *.txt file

## doc

### [Resistance_measurement.md](https://github.com/Castersorium/0vbbElectronics/blob/master/doc/Resistance_measurement.md)

- Table which recorded measurement conditions and other details
- Currently all of the data was acquired through Milan electronics

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
├── data
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
│   │   └── 39p9mK_PIDbad
│   │       ├── 19T20.txt
│   │       ├── 20231108T140000.xlsx
│   │       ├── 3x1NTD.txt
│   │       └── 3x3NTD.txt
│   └── electronics
│       └── streamdata
│           └── chassis_test
│               └── data20231207
│                   └── README.md
├── doc
│   ├── Resistance_measurement.md
│   ├── doc.tmp
│   └── test.sh
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
├── src
│   ├── plotGraph.C
│   ├── plotGraph_C.d
│   ├── plotGraph_C.so
│   ├── plotGraph_C_ACLiC_dict_rdict.pcm
│   └── plotSinGraph.C
└── wrapUp.sh

18 directories, 43 files
```
<!-- TREE END -->

# END
