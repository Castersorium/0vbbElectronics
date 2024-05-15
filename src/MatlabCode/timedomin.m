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

legends = cell(1,length(filenames)); 

% Create a new figure window
figure; 

for i = 1:length(filenames)
    filename = filenames{i};
    data = readmatrix(fullfile(path, filename));
    data = data(5:end, :);

%     % Set parameters for the PSD calculation
%     sen = 1.026;    % Sensitivity in V/g
%     gain = 10.003;       % Default gain
%     fs = 10000;     % Sampling frequency
%     % Set the gain based on the filename
%     if contains(filename,"1gain")
%         gain = 1; %set the gain to 1 if the filename contains "1gain"
%     elseif contains(filename,"10gain")
%         gain = 10.003; %set the gain to 10.003 if the filename contains "10gain"
%     elseif contains(filename,"100gain")
%         gain = 100.122; %set the gain to 100.122 if the filename contains "100gain"
%     end

%     data(:,2) = data(:,2)/(gain*sen);
%     data(:,2) = data(:,2)*1000; 

%     if i == 1
%         data(:,2) = data(:,2)-906;
%     end
    % plot 
    plot(data(:,1),data(:,2),LineWidth=2);
%     scatter(data(:,1),data(:,2),'filled','SizeData',5);
%     scatter(data(:,1),data(:,6),'filled','SizeData',5);
    % set(gca, 'XScale', 'log');
%     set(gca, 'YScale', 'log');
%     set(gca, 'ZScale', 'log');
%     xlim([0 3600]);
    [~, name, ~] = fileparts(filename);
    legends{i} = name;
    hold on;

end
    legend(legends,'FontSize',24,'Location','northeast','Interpreter','none'); % Plot the legend with all file names
    xlabel('Time (s)','FontSize',24);
    ylabel('Amplitude (V)','FontSize',24);
%     ylabel('Acceleration (g)');
    title(name,'Interpreter','none','FontSize',24);
    grid on;
    hold off;

