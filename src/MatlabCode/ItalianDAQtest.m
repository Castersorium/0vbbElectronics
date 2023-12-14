% Clear the workspace
clear;

% Get the filenames and path of the selected Excel files
[filename, path] = uigetfile('*.xlsx', 'Select .xlsx file');

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
    % Extract data for the current channel
    channel_indices = channel:num_channels:size(data, 1);
    channel_data = data(channel_indices, :);
    
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
    coefficients = polyfit(channel_current, channel_voltage, 1);
    fitted_voltage = polyval(coefficients, channel_current);
    
    % Plot the linear fit
    plot(channel_current,fitted_voltage, '-r', 'LineWidth', 1.5, 'DisplayName', 'Linear Fit');

    % Calculate R-square value
    y_fit = polyval(coefficients, channel_current);
    y_mean = mean(channel_voltage);
    ss_total = sum((channel_voltage - y_mean).^2);
    ss_residual = sum((channel_voltage - y_fit).^2);
    r_square = 1 - ss_residual / ss_total;
    
    % Create a legend for the subplot
    legend('show', 'Location', 'southeast');
    legend(['Channel ' num2str(channel)], ['R (\Omega): ' num2str(coefficients(1)) newline ...
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