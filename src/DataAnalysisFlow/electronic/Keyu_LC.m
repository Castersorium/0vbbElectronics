% Clear the workspace
clear;

% Get the filenames and path of the selected Excel files
[filename, path] = uigetfile('D:\RUNs\RUN2410\LCS_2\*.xlsx', 'Select .xlsx file');

% If the user clicked cancel, return
if isequal(filename, 0)
    return;
end

% Load the selected Excel file
data = readmatrix(fullfile(path, filename));

% Extract the voltage, current (I), and resistance for each channel
voltage = data(:, 28);
current = data(:, 29);
resistance = data(:, 30);

% Calculate voltage vs. current for each channel
num_channels = 12; % Assuming you have 12 channels

figure;
num_rows = 3;
num_cols = 4;

for channel = 1:num_channels
    %if channel ~= 7 && channel ~= 9 && channel ~= 11 
    if channel ~= 8 
        continue
    end
    % Extract data for the current channel
    channel_indices = channel:num_channels:size(data, 1);
    channel_data = data(channel_indices, :);
    if isempty(channel_data)
        warning('Channel data is empty, skipping to the next iteration.');
        continue;
    end

    channel_voltage = channel_data(:, 28);
    channel_current = channel_data(:, 29);
    % Create a subplot for the current channel
    subplot(num_rows, num_cols, channel);
    % Plot current vs. voltage for each channel
    scatter(channel_current, channel_voltage, 'filled', 'SizeData', 30, 'DisplayName', ['Channel ' num2str(channel)]);
    grid on;
    grid minor;
    hold on;

    % Linear fit for the current channel
    %coefficients = polyfit(channel_current, channel_voltage, 1);
    %coefficients = polyfit(channel_current, channel_voltage, 2);
    %fitted_voltage = polyval(coefficients, channel_current);
   
    % 排序 current 数据
    sorted_current = sort(channel_current);

    % 计算前10%和后90%的阈值
    threshold_low = sorted_current(floor(1 * length(sorted_current)));
    %threshold_high = sorted_current(ceil(0.2 * length(sorted_current)));

    % 分段索引
    idx1 = channel_current <= threshold_low;  % 前10%
    idx2 = channel_current >= threshold_low; % 后10%

    % 第一段拟合
    coefficients1 = polyfit(channel_current(idx1), channel_voltage(idx1), 2); % 二阶拟合
    fitted_voltage1 = polyval(coefficients1, channel_current(idx1));

    % 第二段拟合
    coefficients2 = polyfit(channel_current(idx2), channel_voltage(idx2), 2); % 一阶拟合
    fitted_voltage2 = polyval(coefficients2, channel_current(idx2));

    % 合并
    plot(channel_current(idx1), fitted_voltage1, '-r', 'LineWidth', 1.5);
    hold on;
    plot(channel_current(idx2), fitted_voltage2, '-g', 'LineWidth', 1.5);

    % Plot the linear fit
    %plot(channel_current,fitted_voltage, '-r', 'LineWidth', 1.5, 'DisplayName', 'Fit');

    % Calculate R-square value
    y_fit = polyval(coefficients1, channel_current(idx1));
    y_mean = mean(channel_voltage(idx1));
    ss_total = sum((channel_voltage(idx1) - y_mean).^2);
    ss_residual = sum((channel_voltage(idx1) - y_fit).^2);
    r_square = 1 - ss_residual / ss_total;


    % Create a legend for the subplot
    legend('show', 'Location', 'southeast');
    legend(['Channel ' num2str(channel)], ['R (\Omega): ' num2str(coefficients1(2)) newline ...
        'R^2: ' num2str(r_square)]);
    
    % Customize the plot...
    title(['Channel ' num2str(channel)], 'FontSize', 15);
    xlabel('Current (A)', 'FontSize', 15);
    ylabel('Voltage (V)', 'FontSize', 15);
    
    % Clear hold to prepare for the next subplot
    hold off;
end

% Create a new figure for Power vs. Resistance plots
figure;
num_rows_power = 3;
num_cols_power = 4;

for channel = 1:num_channels
    % Extract data for the current channel
    channel_indices = channel:num_channels:size(data, 1);
    channel_data = data(channel_indices, :);
    if isempty(channel_data)
        warning('Channel data is empty, skipping to the next iteration.');
        continue;
    end

    channel_current = channel_data(:, 29);
    channel_voltage = channel_data(:, 28);
    channel_resistance = channel_data(:, 30);
    
    % Calculate power for each data point (P = I * V)
    channel_power = channel_current .* channel_voltage;
    
    % Create a subplot for the current channel
    subplot(num_rows_power, num_cols_power, channel);
    % Plot Power vs. Resistance for each channel
    scatter(channel_power, channel_resistance, 'filled', 'SizeData', 30, 'DisplayName', ['Channel ' num2str(channel)]);
    grid on;
    grid minor;
    hold on;

    % Customize the plot...
    title(['Channel ' num2str(channel)], 'FontSize', 15);
    xlabel('Power (W)', 'FontSize', 15);
    ylabel('Resistance (\Omega)', 'FontSize', 15);
    
    % Clear hold to prepare for the next subplot
    hold off;
end