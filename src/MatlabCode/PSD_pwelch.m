clear;
% Get the filenames and path of the selected CSV files
[filenames, path] = uigetfile('*.csv', 'Select CSV files', 'MultiSelect', 'on');

% If the user clicked cancel, return
if isequal(filenames,0)
   return;
end
% If only one file was selected, convert it to a cell array
if ~iscell(filenames)
    filenames = {filenames};
end

% Initialize cell arrays
legends = cell(1,length(filenames)); 
psds = cell(1,length(filenames));
fres = cell(1,length(filenames));
datas = cell(1,length(filenames));
% psd_ds = cell(1,length(filenames));
% rms = cell(1,2*length(filenames));

% Set parameters for the PSD calculation
% sen = 1.026;    % Sensitivity in V/g
% g = 9.79;       % m/s2
wint = 10;       % Window time in s
% gain = 100.122;       % Default PCB gain
% gain = 1;   %Default gain
% gain = 412;       % Italy DAQ
% gain = 1648;       % Italy DAQ
% gain = 206*6;    %MIB206*6
% gain = 206*7;    %MIB206*7
% gain = 206*8;    %MIB206*8
% gain = 206*30;    %MIB206*30
% gain = 206*100;    %MIB206*100
gain = 206*17;
% gain = 206*4;
% gain = 206*2;
% gain = 1700;    %FRBOX
% rmsf0 = 0.2; %rms REG1 begin frequency
% rmsf1 = 40; %rms REG1 end frequency
% rmsf2 = 2500; %rms REG2 end frequency


% Loop over selected files
for i = 1:length(filenames)
% for i = 1:4
    % Get the current filename and read in the data
%     filename = filenames{1};
    filename = filenames{i};
    data = readmatrix(fullfile(path, filename));

    % Remove the first 4 rows of data and reshape into a vector
%     fs = 1/data(2,2);
   if i == 1
    gain = 206*8;    %MIB206*8
    data = data(5:end, 1:2); 
   else
      gain = 206*4;    %MIB206*8
       data = data(5:end, 3:4); 
   end
%     data = data(5:end, :); 
    time = data(:,1);
    data = data(:,2);
%     data = data(:,4);
%     data = data(:,6);
%     data = data(:,8);
    datas{i} = data;

    %calculate fs
    fs = round(1/(time(220)-time(219)));


    
%     % Set the gain based on the filename
%     if contains(filename,"1gain")
%         gain = 1; %set the gain to 1 if the filename contains "1gain"
%     elseif contains(filename,"10gain")
%         gain = 10.003; %set the gain to 10.003 if the filename contains "10gain"
%     elseif contains(filename,"100gain")
%         gain = 100.122; %set the gain to 100.122 if the filename contains "100gain"
%     end
    data = data/gain;

    % Define the window length and overlap
    win_len = wint * fs; 
    overlap = win_len/2; % Half-window overlap

    % Compute the PSD using the Welch method with Hanning window
    [psd, f] = pwelch(data, hann(win_len), overlap,win_len, fs);
%     [psd, f] = pwelch(data, rectwin(win_len), overlap,win_len, fs);
    psd = sqrt(psd);

%     % Compute the PSD_d and rms
%     psd_d = psd*g./(2*pi*f).^2;
%     REG1_b = rmsf0 * wint + 1;
%     REG1_e = rmsf1 * wint + 1;
%     REG2_e = rmsf2 * wint + 1;
%     rmsl = sqrt(sum(psd_d(REG1_b:REG1_e).^2)/wint);
%     rmsh = sqrt(sum(psd_d(REG1_e+1:REG2_e).^2)/wint);
% 
%     %give value to the cell arrays
%     rms{2*i-1} = rmsl;
%     rms{2*i} = rmsh;
    psds{i} = psd;
    fres{i} = f ;
%     psd_ds{i} = psd_d;

    % Set legend for current file
    [~, name, ~] = fileparts(filename);
    legends{i} = name;

    % Save data as .mat file
    sub_dir = 'matdata';
        if ~exist(sub_dir, 'dir')
            mkdir(sub_dir);
        end
%     save(fullfile(sub_dir, strcat(filename, '.mat')), 'f','data','psd', 'psd_d', 'rmsl','rmsh');
    save(fullfile(sub_dir, strcat(filename, '.mat')), 'f','data','psd');

end

% plot_psd(fres, psds, 'a',wint,fs,legends);
% plot_psd(fres, psd_ds, 'd',wint,fs,legends);
plot_psd(fres, psds, 'v',wint,fs,legends);
