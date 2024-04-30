clear;
[filenames, path] = uigetfile('*.csv', 'Select CSV files', 'MultiSelect', 'on');
if isequal(filenames, 0)
    % User clicked Cancel
    return;
end
% If only one file was selected, convert it to a cell array
if ~iscell(filenames)
    filenames = {filenames};
end

for i = 1:length(filenames)
    filename = filenames{i};
    [~, name, ~] = fileparts(filename);
    data = readmatrix(fullfile(path, filename));
    fs = 1/data(2,2);  
    %     data = data(5:end, :);
    data = data(5:end, 5:6);

    % Set parameters for the PSD calculation
    % sen = 1.026;    % Sensitivity in V/g
    % g = 9.81;       % m/s2
    wint = 30;       % Window time in s
    % gain = 10.003;       % Default gain
    gain = 412;       % Default gain
    % fs = 10000;     % Sampling frequency


%     % Set the gain based on the filename
%     if contains(filename,"1gain")
%         gain = 1; %set the gain to 1 if the filename contains "1gain"
%     elseif contains(filename,"10gain")
%         gain = 10.003; %set the gain to 10.003 if the filename contains "10gain"
%     elseif contains(filename,"100gain")
%         gain = 100.122; %set the gain to 100.122 if the filename contains "100gain"
%     end
% % Set the fs based on the filename
% % Search for the pattern "(\d+)fs" in the filename
% match = regexp(filename, '(\d+)fs', 'match');

% % If a match is found, extract the digits and convert to a double
% if ~isempty(match)
%     fs_str = match{1}(1:end-2); % Remove the "fs" suffix
%     fs = str2double(fs_str);
% end

    % Calculation
    % data(:,2) = data(:,2)/(gain*sen);
    data(:,2) = data(:,2)/gain;

    [x_3D, y_3D, z_3D] = Mide_Spectrogram(data,fs,wint);
    plot_Spectrogram(name,fs,wint,x_3D,y_3D,z_3D)

end
