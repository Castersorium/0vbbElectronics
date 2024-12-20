% 读取第一个 CSV 文件 (假设文件名为 'file1.csv')
data1 = csvread('PoverR_60Gohm.csv'); % 读取数据
x1 = data1(:, 1); % 第一列是 x 值
y1 = data1(:, 2); % 第二列是 y 值

% 读取第二个 CSV 文件 (假设文件名为 'file2.csv')
data2 = csvread('PoverR_10Gohm.csv'); % 读取数据
x2 = data2(:, 1); % 第一列是 x 值
y2 = data2(:, 2); % 第二列是 y 值

% 将第一个文件的 x 除以 60
x1_new = x1 / 60;
% 将第二个文件的 x 除以 10
x2_new = x2 / 10;

% 合并数据
merged_data = [x1_new, y1; x2_new, y2];

% 按照 x 的值进行排序
[sorted_x, sort_idx] = sort(merged_data(:, 1));  % 获取排序后的 x 和索引
sorted_y = merged_data(sort_idx, 2);  % 使用索引对 y 进行排序

% 绘制排序后的数据
figure;
plot(sorted_x, sorted_y, '-o', 'LineWidth', 2);
xlabel('I (pA)');
ylabel('Amp/RMS');
title('Amp/RMS VS I');
grid on;

% 如果需要保存排序后的数据
%sorted_data = [sorted_x, sorted_y];
%csvwrite('sorted_merged_data.csv', sorted_data);

% 如果你想要在同一个图上绘制两个数据集，也可以：
figure;
hold on; % 保持当前图形
plot(x1_new, y1, '-o', 'LineWidth', 2, 'DisplayName', '60Gohm');
plot(x2_new, y2, '-x', 'LineWidth', 2, 'DisplayName', '10Gohm');
xlabel('I (pA)');
ylabel('Amp/RMS');
title('Amp/RMS VS I');
grid on;
legend show;