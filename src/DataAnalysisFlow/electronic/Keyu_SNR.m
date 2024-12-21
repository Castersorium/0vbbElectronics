% 正负电压法pro
% 用于处理LC信号，1.将信号分段，提取中间稳定部分 2.获取每段信号的峰与本底情况

% 符号变化检测函数
function indices = find_sign_changes(data, threshold)
    indices = [1; find(abs(diff(sign(data))) > threshold) + 1; length(data)];
end

% 读取CSV文件
filename = 'LC12_60Gohm_1cmLMO.csv'; % 替换为实际的文件名
%filename = '10Gohm.csv';
data = readtable(filename);

% 提取时间和LMO数据
time = data{:, 1};
lmo = data{:, 2};

%0.1 not in 60
vol = [ 0.1 0.3 0.5 0.7 0.9 1 3 5 10 12 14 16 18 20 22 24 26 28 30 35 40 45];

% 设置每组的最小长度和稳定时间
segment_length = 200;%每组长度
%stable_time1 = 10; % 前稳定时间
stable_time1 = 150; % 前稳定时间
stable_time2 = 5; % 后稳定时间
stable_time3 = 30; 
freq = 5000;%频率，单位为Hz
% 定义你想要插入的数组
% 选择指定时间范围内的数据
%valid_indices = (time >= 560 & time <= 9550); % 替换为所需时间范围
valid_indices = (time >= 10 & time <= 9550); % 替换为所需时间范围60Gohm
%valid_indices = (time >= 10 & time <= 9500); % 替换为所需时间范围10Gohm
time = time(valid_indices);
lmo = lmo(valid_indices);

% 符号变化检测
threshold = 1e-6; % 可调整的阈值
sign_changes = find_sign_changes(lmo, threshold); % 调用自定义函数

% 分组边界
group_start = sign_changes(1:end-1);
group_end = sign_changes(2:end) - 1;

% 保留满足长度要求的分组
valid_groups = (time(group_end) - time(group_start) >= segment_length*0.8);
group_start = group_start(valid_groups);
group_end = group_end(valid_groups);

% 处理相邻分组间的时间间隔
new_group_start = group_start(1); % 初始化第一个组的起始时间
new_group_end = group_end(1);     % 初始化第一个组的结束时间
%group_start_final = new_group_start; % 新的最终起始时间数组
group_start_final = new_group_end - stable_time1*freq; % 新的最终起始时间数组
group_end_final = new_group_end- stable_time2*freq;     % 新的最终结束时间数组

for i = 2:length(group_start)
    % 计算当前组的起始时间和前一组的结束时间之间的时间差
    time_diff = time(group_start(i)) - time(new_group_end);
    
    if time_diff > segment_length
        % 如果时间差大于 segment_length，则插入一个新的组
        % 新组的时间区间是这两个组的时间差
        new_group_start = new_group_end;
        %new_group_end = new_group_start + segment_length - 1;
        new_group_end = group_start(i);        
        %group_start_final = [group_start_final; new_group_start + stable_time1*freq];
        group_start_final = [group_start_final; new_group_end - stable_time1*freq];
        group_end_final = [group_end_final; new_group_end - stable_time3*freq];
    end
    
    % 继续处理下一个有效组
    %group_start_final = [group_start_final; group_start(i) + stable_time1*freq];
    group_start_final = [group_start_final; group_end(i) - stable_time1*freq];
    group_end_final = [group_end_final; group_end(i) - stable_time2*freq];
    new_group_end = group_end(i); % 更新结束时间
end

%绘图：分组数据
figure;
hold on;
colors = parula(length(group_start_final)); % 使用 parula 调色板
for i = 1:length(group_start_final)
    % 检查是否索引超出范围
    if group_start_final(i) > length(time) || group_end_final(i) > length(time)
        warning('索引超出范围，跳过第 %d 组', i);
        continue;
    end
    group_data_time = time(group_start_final(i):group_end_final(i));
    group_data_lmo = lmo(group_start_final(i):group_end_final(i));
    %plot(group_data_time, group_data_lmo, '-o', 'Color', colors(i, :), 'LineWidth', 1.5);
    plot(group_data_time, group_data_lmo, 'Color', colors(i, :));
    % 添加标注
    text(group_data_time(end), group_data_lmo(end), sprintf('组 %d', i), 'Color', colors(i, :), 'FontSize', 9);

end
hold off;
xlabel('时间 (秒)');
ylabel('LMO 数据值');
title(sprintf('分组数据图 (%d 个分组)', length(group_start_final)));
grid on;

%测试图：符号变化点
% figure;
% plot(time, lmo, '-b'); % 绘制 lmo 数据
% hold on;
% scatter(time(sign_changes), lmo(sign_changes), 'ro', 'filled'); % 标记符号变化点
% xlabel('时间 (秒)');
% ylabel('LMO 数据值');
% title('LMO 数据及符号变化点');
% grid on;
% hold off;

% 初始化存储变量
num_segments = length(group_start_final);
pulse_to_rms = zeros(num_segments, 1);  % 脉冲峰值/RMS
pulse_peak = zeros(num_segments, 1);   % 脉冲峰值
rms_baseline = zeros(num_segments, 1); % 本底RMS
mean_baseline = zeros(num_segments, 1); % 基线均值

% 计算脉冲峰值和RMS
for i = 1:num_segments
    % 当前电压段数据
    current_data = lmo(group_start_final(i):group_end_final(i));
    if mod(i, 2) == 0
        current_data = -current_data; % 偶数段数据取反
    end

    % 计算基线数据：排除明显脉冲影响
    baseline_data = current_data(current_data < (mean(current_data) + 2 * std(current_data)));
    std_baseline(i) = std(current_data);
    rms_baseline(i) = rms(baseline_data); % 计算RMS
    mean_baseline(i) = mean(baseline_data); % 计算均值

    % 阈值筛选
    %potential_peaks = current_data(current_data > (mean(current_data) + 2 * std(current_data)));
    %mean_peaks(i) = mean(potential_peaks);
    % 使用平滑处理
    smoothed_data = movmean(current_data, 5); % 滑动窗口大小为3
    % 找到显著峰值
    threshold = mean(current_data) + 5 * std(current_data); % 自定义阈值
    % 检查是否有数据大于或等于 threshold
    if any(smoothed_data >= threshold)
        % 如果有，执行 findpeaks
        [pks, locs] = findpeaks(smoothed_data, 'MinPeakHeight', threshold);
        % 计算 pks 的均值和标准差
        mean_pks = mean(pks);
        std_pks = std(pks);
        % 筛选符合条件的峰值
        filtered_pks = pks(pks > (mean_pks - 2 * std_pks) & pks < (mean_pks + 2 * std_pks));

        %pks_without_max = pks(pks < max(pks)); % 排除最大峰值
        %mean_peaks(i) = mean(pks_without_max);
        
        % 使用 polyfit 拟合常数，指定阶数为 0（常数拟合）
        p = polyfit(locs, pks, 0);
        % p(1) 即为拟合的常数值
        %mean_peaks(i) = p(1);
        
        %mean_peaks(i) = mean(pks(pks < (mean(pks) + 2 * std(pks))));
        %mean_peaks(i)= pks(pks > (mean(pks) - 2 * std(pks)) & pks < (mean(pks) + 2 * std(pks)));  % 这里的 pks 表示每个 pks 的值
        % 计算筛选后的峰值的均值
        if ~isempty(filtered_pks)
            mean_peaks(i) = mean(filtered_pks);  % 计算并保存筛选后的峰值均值
        else
            mean_peaks(i) = 0;  % 如果没有符合条件的峰值，返回 NaN
        end


    else
        % 如果没有，输出一个提示或设置 pks 和 locs 为空
        pks_without_max = []; % 排除最大峰值
        mean_peaks(i) = 0;
    end

    %[pks, locs] = findpeaks(smoothed_data, 'MinPeakHeight', threshold);
    %pks_without_max = pks(pks < max(pks)); % 排除最大峰值
    %mean_peaks(i) = mean(pks_without_max);

    % 计算脉冲峰值
    if ~isempty(pks_without_max)
        pulse_peak(i) = mean_peaks(i) - mean_baseline(i);
    else
        pulse_peak(i) = 0; % 如果没有剩余峰值
    end

    % 计算脉冲峰值/RMS
    if rms_baseline(i) ~= 0
        pulse_to_rms(i) = pulse_peak(i) / rms_baseline(i);
    else
        pulse_to_rms(i) = 0; % 避免除以0
    end
end


% 测试图figure;
% hold on;
% % 直接指定三种颜色（红色、绿色、蓝色）
% for i = 1:length(group_start_final)
%     % 为每组数据指定颜色
%     % 绘制数据
%     plot(group_data_time, group_data_lmo, 'Color', 'r');   
%     % 添加标注
%     text(group_data_time(end), group_data_lmo(end), sprintf('组 %d', i), 'Color', 'r', 'FontSize', 9);
%     % 计算并绘制表示mean_baseline(i) / RMS的横线
%     yline(mean_baseline(i), '--', sprintf('基线均值 %d', i), 'Color', 'm', 'LineWidth', 1.5);
%     yline(th, '--', sprintf('Threshold %d', i), 'Color', 'c', 'LineWidth', 1.5);
%     yline(mean_peaks(i), '--', sprintf('峰值 %d', i), 'Color', 'b', 'LineWidth', 1.5);
%     %yline(std_baseline(i), '--', sprintf('std %d', i), 'Color', 'r', 'LineWidth', 1.5);
% end
% hold off;


% 生成新数组
voltage = zeros(1, length(vol) * 2); % 创建一个两倍长度的零数组
voltage(1:2:end) = -vol;            % 奇数索引位置填充负值
voltage(2:2:end) = vol;             % 偶数索引位置填充正值
% 定义电压索引
%voltage_indices = 1:num_segments;
voltage_indices = voltage;
%voltage_odd = voltage(1:2:end);   % 奇数项
voltage_odd = voltage(2:2:end);   % 奇数项
voltage_even = voltage(2:2:end);  % 偶数项

% 将 voltage_odd 和 pulse_to_rms(1:2:end) 合并成一个矩阵
data_to_save = [voltage_odd', pulse_to_rms(1:2:end)];  % 转置使每列对应一个变量
% 保存到 CSV 文件
csvwrite('save.csv', data_to_save);


% 绘制结果
figure;
subplot(2, 2, 1); % 脉冲峰值/RMS
hold on; % 保持当前图形
%plot(voltage_indices, pulse_to_rms, '-o', 'LineWidth', 2);
plot(voltage_odd, pulse_to_rms(1:2:end), '-o', 'LineWidth', 2, 'DisplayName', 'Odd Indices');
%plot(voltage_even, pulse_to_rms(2:2:end), '-o', 'LineWidth', 2, 'DisplayName', 'Even Indices');
xlabel('Voltage');
ylabel('Pulse Peak / RMS');
title('Pulse Peak / RMS vs Voltage');
grid on;
hold off;

subplot(2, 2, 2); % 脉冲峰值
hold on;
%plot(voltage_indices, pulse_peak, '-o', 'LineWidth', 2);
plot(voltage_odd, pulse_peak(1:2:end), '-o', 'LineWidth', 2, 'DisplayName', 'Odd Indices');
%plot(voltage_even, pulse_peak(2:2:end), '-o', 'LineWidth', 2, 'DisplayName', 'Even Indices');
xlabel('Voltage');
ylabel('Pulse Peak');
title('Pulse Peak vs Voltage');
grid on;
hold off;

subplot(2, 2, 3); % RMS
hold on;
%plot(voltage_indices, rms_baseline, '-o', 'LineWidth', 2);
plot(voltage_odd, rms_baseline(1:2:end), '-o', 'LineWidth', 2, 'DisplayName', 'Odd Indices');
%plot(voltage_even, rms_baseline(2:2:end), '-o', 'LineWidth', 2, 'DisplayName', 'Even Indices');
xlabel('Voltage');
ylabel('RMS');
title('RMS vs Voltage');
grid on;
hold off;

subplot(2, 2, 4); % 基线均值
hold on;
%plot(voltage_indices, mean_baseline, '-o', 'LineWidth', 2);
plot(voltage_odd, abs(mean_baseline(1:2:end)), '-o', 'LineWidth', 2, 'DisplayName', 'Odd Indices');
%plot(voltage_even, mean_baseline(2:2:end), '-o', 'LineWidth', 2, 'DisplayName', 'Even Indices');
xlabel('Voltage');
ylabel('Baseline Mean');
title('Baseline Mean vs Voltage');
grid on;
hold off;
