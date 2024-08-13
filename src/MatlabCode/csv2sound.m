clear;
[fileName, filePath] = uigetfile('*.csv', 'Select csv file');
if fileName == 0
    disp('No file selected.');
    return;
end

% 读取CSV文件
data = readmatrix(fullfile(filePath, fileName));

% 假设第一列是时间，第二列是电位
time = data(5:end, 1);
voltage = data(5:end, 2);

% 计算采样率（假设时间数据是等间隔的）
% sampling_rate = 1 / (time(8) - time(7));
sampling_rate = 5000;

% 将电位数据标准化到[-1, 1]的范围
% voltage_normalized = voltage / max(abs(voltage));
voltage_normalized = (voltage - mean(voltage)) / (max(voltage) - min(voltage));
% 播放音频
% sound(voltage_normalized, sampling_rate);

% 生成音频文件
audiowrite('output.wav', voltage_normalized, sampling_rate);

disp('音频文件已生成: output.wav');
