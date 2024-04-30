% 弹出文件夹选择对话框
folderPath = uigetdir('', 'Select folder');
if folderPath == 0
    disp('No folder selected.');
    return;
end

% 获取所选文件夹的名称
[~, folderName, ~] = fileparts(folderPath);

% 获取文件夹下所有的 data.xml 文件路径
xmlFiles = dir(fullfile(folderPath, '**', 'data.xml'));
if isempty(xmlFiles)
    disp('No data.xml files found in the selected folder.');
    return;
end


% 构造输出 CSV 文件的文件名
csvFileName = fullfile(folderPath, sprintf('DAQ_log_%s.csv', folderName));

% 创建输出 CSV 文件
fid = fopen(csvFileName, 'w');
if fid == -1
    error('Failed to open CSV file.');
end

% 写入 CSV 文件表头
fprintf(fid, 'DAQExpress_Project,Name in DAQExpress,Start Time,Duration,Type,Setting Basket,Auto Name,Auto file type,converted name,convert to tdms,convert to csv,convert to ROOT,Comment\n');

% 初始化错误信息列表
errorMessages = {};

% 遍历每个 data.xml 文件并处理
for i = 1:numel(xmlFiles)
    xmlFile = fullfile(xmlFiles(i).folder, xmlFiles(i).name);
    fprintf('Processing file: %s\n', xmlFile); % 输出当前文件路径
    
    try
        xmlData = fileread(xmlFile);
        
        % 提取当前文件的 DAQExpress_Project
        [~, projectDir] = fileparts(fileparts(fileparts(xmlFile)));
        
        % 如果 projectDir 为 "Assets"，则更改为再往前一位的地址
        if strcmp(projectDir, 'Assets')
            [~, projectDir] = fileparts(fileparts(fileparts(fileparts(xmlFile))));
        end

        % 正则表达式模式用于匹配整个区块并提取参数
        pattern = '<EmbeddedDefinitionReference.*?StoragePath="([^"]+)".*?GroupName="([^"]+)".*?Timestamp="([^"]+)".*?</EmbeddedDefinitionReference>.*?</EmbeddedDefinitionReference>.*?</EmbeddedDefinitionReference>';
        matches = regexp(xmlData, pattern, 'tokens');
        % 遍历匹配结果并写入 CSV 文件
        for j = 1:numel(matches)
            storagePath = matches{j}{1};
            groupName = matches{j}{2};
            timestamp = matches{j}{3};
            % 将时间戳字符串转换为 MATLAB datetime 对象
            timestamp = datetime(timestamp, 'InputFormat', 'MM/dd/yyyy HH:mm:ss');
            timestamp = timestamp + hours(8);
            fprintf(fid, '%s,%s,%s,,,,%s,tdms,,0,0,0,0\n', projectDir, groupName, timestamp, storagePath);
        end
    catch exception
        fprintf('Error processing file: %s\n', xmlFile); % 输出错误信息到命令行窗口
        disp(exception.message);
        errorMessages{end+1} = sprintf('Error processing file: %s\n%s', xmlFile, exception.message); % 将错误信息添加到列表
    end
end

% 在 CSV 文件末尾添加错误信息
if ~isempty(errorMessages)
    fprintf(fid, '\n\n');
    fprintf(fid, 'Errors:\n');
    for k = 1:numel(errorMessages)
        fprintf(fid, '%s\n', errorMessages{k});
    end
end

% 关闭 CSV 文件
fclose(fid);
disp(['CSV file created successfully: ', csvFileName]);
