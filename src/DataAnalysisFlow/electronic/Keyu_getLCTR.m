function Keyu_getLCTR(varargin)
delete('LC_TR.log');  % 删除文件
    % Define the folder where the .mat files are located
    %folder_path = 'D:\RUNs\RUN2410\LCS_2\';  % 修改为你的文件夹路径
    % Get all .mat files in the folder
    %mat_files = dir(fullfile(folder_path, '*41B3X1N002*.mat'));

if nargin == 2
    file = varargin{1};
    mat_files = dir(fullfile(file));
    channel_number = varargin{2};
    th_chi = 0.9997;
    temp=-1;
elseif nargin == 3
    file = varargin{1};
    mat_files = dir(fullfile(file));
    channel_number = varargin{2};
    th_chi =varargin{3};
    temp=-1;
elseif nargin == 4
    file = varargin{1};
    mat_files = dir(fullfile(file));
    channel_number = varargin{2};
    temp=varargin{3};
    th_chi = varargin{4};
else
    error('Invalid number of input arguments. Use either 1 or 2 arguments.');
end


% Check if there are any .mat files
if isempty(mat_files)
    disp('No .mat files found in the specified folder.');
    return;
end

% Loop through each .mat file
for k = 1:length(mat_files)
    % Get the current file's full path
    file_path = fullfile(mat_files(k).folder, mat_files(k).name);

    if temp < 0
        filename = mat_files(k).name;
        %temp_str = regexp(filename, 'PID(\d+)(p?\.?\d+)?mK', 'tokens');
        temp_str = regexp(filename, 'PID(\d+)(p?\.?\d+)?mK', 'tokens');
        if ~isempty(temp_str)
            if length(temp_str{1}) == 1
                % 如果只有一个元素（即没有 "p" 后缀），则执行相应的操作
                temperature = str2double(temp_str{1}{1});  % 直接提取温度值
            else
                % 如果有两个元素（即包含 "p" 后缀），则提取并处理带有 "p" 的温度值
                temperature = str2double([temp_str{1}{1}, '.', temp_str{1}{2}(2:end)]);  % 提取并转换为数字
            end
        else
            error('KeyuWarning: cannot get temperature, plase set one.');
        end
    else
        temperature = temp;
    end

    % Load the current .mat file
    data = load(file_path);

    % Extract the voltage, current (I), and resistance for each channel
    %voltage = data(:, 28);
    %current = data(:, 29);
    %resistance = data(:, 30);
    
    % 查看表格中的列名
    data_table = data.LoadCurveTable;  % 假设表格的变量名是 'LoadCurveTable'
    % 提取第28, 29, 30列的数据（对应 'V_Input_Inv', 'V_Bol', 'I_Bol'）
    %voltage = data_table.V_Bol;  % 28列：V_Input_Inv
    %current= data_table.I_Bol;        % 29列：V_Bol
    %resistance = data_table.R_Bol;        % 30列：I_Bol

    % Calculate voltage vs. current for each channel
    num_channels = 12; % Assuming you have 12 channels

    % For each channel
    for channel = 1:num_channels
        % Only plot and analyze the selected channels
        if channel ~= channel_number
            continue
        end

        % Extract data for the current channel
        %channel_indices = channel:num_channels:size(data, 1);
        %channel_data = data(channel_indices, :);
        % Extract data for the current channel
        channel_indices = channel:num_channels:height(data_table);  % 用 height(data_table) 获取行数
        channel_data = data_table(channel_indices, :);

        if isempty(channel_data)
            warning('Channel data is empty, skipping to the next iteration.');
            continue;
        end

        channel_voltage = channel_data.V_Bol;  % 假设列名是 'V_Input_Inv'
        channel_current = channel_data.I_Bol;       % 假设列名是 'V_Bol'

        [~, ~, para] = Keyu_Ployfit(channel_current,channel_voltage, th_chi);
        %disp(fitresult);
        %disp(gof);


        % 输出拟合的斜率
        disp(['File: ', mat_files(k).name, ' - Channel: ', num2str(channel)]);
        disp(['拟合的斜率 (slope): ', num2str(para)]);
        disp('*********************');

        % 打开文件 output.log（如果文件不存在，则创建文件）
        fileID = fopen('LC_TR.log', 'a');  % 'a' 表示以追加模式打开文件
        %fileID = fopen('output.log', 'w');  % 'w' 表示以覆盖
        % 将文件名和斜率输出到文件
        %fprintf(fileID, 'File: %s - Slope: %f\n', mat_files(k).name, coefficients);
        fprintf(fileID, '%f,%f\n', temperature, para);

        % 关闭文件
        fclose(fileID);
    end



%     % 设置log文件路径和输出Excel文件路径
% logFilePath = 'LC_TR.log';  % 修改为你的log文件路径
% excelFilePath = 'LC_TR.xlsx';  % 输出Excel文件路径
% 
% % 读取log文件，假设文件中以逗号分隔
% data = readtable(logFilePath, 'Delimiter', ',', 'ReadVariableNames', false);
% 
% 
% % 对第一列的数据进行除以10的操作
% data{:, 1} = data{:, 1} / 1000;  % 这里假设第一列是数值列
% 
% % 将数据写入Excel文件
% writetable(data, excelFilePath, 'WriteVariableNames', false);
% 
% disp('Data has been written to Excel successfully!');
% 


end

