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

% Initialize cell array for legends
legends = cell(1,length(filenames)); 
% Loop over selected files
for i = 1:length(filenames)
    % Get the current filename and read in the data
    filename = filenames{i};
    data = readmatrix(fullfile(path, filename));

    % Remove the first 4 rows of data and reshape into a vector
    time = data(5:end, 1);
    data = data(5:end, 2);
    fs = round(1/(time(10)-time(9)));

    % Set parameters for the PSD calculation
    wint = 1;       % Window time in s
    overlap = 0; % Overlap
    window_size = wint*fs; % Window size in samples
    nfft = 2^nextpow2(window_size); % Number of FFT points
    f = (0:nfft/2-1)*fs/nfft; % Frequency vector

    % Apply Hanning window to data
    window = hann(window_size);
    window = window./sqrt(mean(window.^2)); % normalize Hanning window
    data_windowed = buffer(data, window_size, overlap, 'nodelay');
    data_windowed = data_windowed .* window;

    % Compute power spectrum density
    psd = zeros(nfft/2, size(data_windowed, 2));
    for j = 1:size(data_windowed, 2)
        fft_data = fft((data_windowed(:,j) - mean(data_windowed(:,j))), nfft);
        psd(:,j) = abs(fft_data(1:nfft/2)).^2 / (fs*nfft);
        psd(2:end-1,j) = 2*psd(2:end-1,j);

    end
    psd = mean(psd, 2);
    psd = sqrt(psd);
    
    % Plot PSD
    loglog(f, sqrt(psd)); % Plot the power spectrum density
    % Set legend for current file
    [~, name, ~] = fileparts(filename);
    legends{i} = name;

    hold on;
end

% Plot legend with all file names
legend(legends,'FontSize',18,'Location','northeast','Interpreter','none'); % Plot the legend with all file names

xlabel("$Frequency (Hz)$","Interpreter","latex"); % x-axis label
ylabel("$PSD\left[g/\sqrt{Hz}\right]$","Interpreter","latex"); % y-axis label
grid on; % show grid
title("Power Spectrum Density"); % plot title

hold off;
