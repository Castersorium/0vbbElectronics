load('20250506T110007.mat');        % 加载 .mat 文件
writetable(LoadCurveTable, 'output.xlsx');  % 如果 data 是数值数组