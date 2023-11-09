# Electronics testing for 0vbb experiment

<!-- For Tree -->

```
.
├── README.md
├── data
│   ├── 19T20.txt
│   ├── 20231108T140000.xlsx
│   ├── 3x1NTD.txt
│   ├── 3x3NTD.txt
│   └── Resistance_measurement.md
├── doc
│   └── doc.tmp
├── output
│   ├── 19T20.jpg
│   ├── RTCurve.xlsx
│   ├── output.root
│   ├── outputFit.root
│   ├── outputFuAnQuXian.root
│   └── testFit.root
└── src
    └── plotGraph.C
```

## data

### `*.txt`

- Column1: V_bol(V)
- Column2: I_bol(A)

### `*.xlsx`

- Raw Data generated by Matlab GUI

### [Resistance_measurement.md](https://github.com/Castersorium/0vbbElectronics/blob/master/data/Resistance_measurement.md)

## output

Graphs and Fit results

### `RTCurve.xlsx`

- Preliminary results for temperature fit

## src

### `plotGraph.C`

- Draw the plots from data
- The data is fluctuated caused by **PID** controlling phase
- Fit the result with the function $$V = R_\mathrm{bol} \times I + Amp \times I \times \sin(\omega I + \varphi) + C$$
- Results are stored in output
- $\max{R_\mathrm{bol}}=R_\mathrm{bol} + Amp$, at the lowest temperature
- $\min{R_\mathrm{bol}}=R_\mathrm{bol} - Amp$, at the highest temperature

# END
