        % Read the first file
        data1 = readmatrix('keyutemp/CH7 T.log');  % Read the first file
        xdata = data1(:, 3);  % Select the third column as temperature
        
        % Read the second file
        data2 = readmatrix('keyutemp/CH6 T.log');  % Read the second file
        ydata = data2(:, 3);  % Select the third column as resistance


% 给定的数组
targetArray =[0.0087,0.013,0.015,0.0177,0.019,0.021,0.0229,0.025,0.027,0.0292,0.03,0.035,0.0405,0.0455,0.0509,0.08,0.1147,0.1558];

% 初始化存储最接近的值及其对应的 ydata 值
closestX = zeros(size(targetArray));  % 存储最接近的 xdata 值
correspondingY = zeros(size(targetArray));  % 存储对应的 ydata 值

% 找到最接近的 xdata 和对应的 ydata
for i = 1:length(targetArray)
    % 计算目标值与 xdata 中每个值的差的绝对值
    [~, idx] = min(abs(xdata - targetArray(i)));
    closestX(i) = xdata(idx);
    correspondingY(i) = ydata(idx);
end

% 显示结果
disp('最接近的 xdata 值及对应的 ydata 值：');
disp(table(targetArray', closestX', correspondingY', 'VariableNames', {'TargetValue', 'ClosestX', 'CorrespondingY'}));

% 绘制图形
figure;
plot(xdata, ydata, '-o', 'DisplayName', '原始数据');
hold on;
plot(closestX, correspondingY, 'r*', 'DisplayName', '最接近的点');
legend;
xlabel('xdata');
ylabel('ydata');
title('原始数据与最接近的点');
hold off;

% 创建一个包含 xdata 和 ydata 的表格
dataTable = table(closestX', correspondingY', 'VariableNames', {'XData', 'YData'});
% 设置输出Excel文件路径
excelFilePath = 'output_data.xlsx';
% 将数据写入 Excel 文件
writetable(dataTable, excelFilePath);