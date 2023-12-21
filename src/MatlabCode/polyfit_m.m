clear;

gain = 218 * 1.95 * 4;

% Get the filenames and path of the selected CSV files
[filenames, path] = uigetfile('*.csv', 'Select CSV files', 'MultiSelect', 'on');

% If the user clicked cancel or no files were selected, return
if isequal(filenames,0)
   return;
end

% If only one file is selected, convert it to a cell array for consistent processing
if ~iscell(filenames)
    filenames = {filenames};
end

% Loop through each selected file
for i = 1:numel(filenames)
    filename = filenames{i};
    
    % Set R_load value based on csv filename
    if contains(filename, '40G')
        R_load = 4 * 10^10;
    elseif contains(filename, '20G')
        R_load = 2 * 10^10;
    elseif contains(filename, '4G')
        R_load = 4 * 10^9;
    elseif contains(filename, '2G')
        R_load = 2 * 10^9;
    elseif contains(filename, '1G')
        R_load = 1 * 10^9;
    elseif contains(filename, '200M')
        R_load = 2 * 10^8;
    else
        R_load = 2 * 10^10;
    end

    % Read the CSV file
    data = readmatrix(fullfile(path, filename));

    % import the data
    V_bias = data(5:end,4);
    V_out = data(5:end,2);
    I_bias = V_bias * 10^9 / R_load;
    V_NTD = V_out * 10^9 / gain;
    x_range = [min(I_bias) max(I_bias)];

    % Draw the figure with x axis V_bias (V) and y axis V_out
    figure('Position', [0 0 1920 1080]);
    scatter(I_bias, V_NTD/10^6,'filled','SizeData',5);
    xlabel('I_{bias} (nA)');
    ylabel('V_{NTD} (mV)');
    grid on;
    set(gca,'FontSize',24);
    title(filename,'FontSize',24);

    x = I_bias(I_bias >= x_range(1) & I_bias <= x_range(2));
    y = V_NTD(I_bias >= x_range(1) & I_bias <= x_range(2));

    % Linear fit the data and show the fit parameters in a box
    p = polyfit(x, y, 4);

    % Calculate the R^2
    y_fit = polyval(p, x);
    y_resid = y - y_fit;
    SSresid = sum(y_resid.^2);
    SStotal = (length(y)-1) * var(y);
    rsq = 1 - SSresid/SStotal;
    rsq_adj = 1 - SSresid/SStotal * (length(y)-1)/(length(y)-length(p));

    % Print the result
    text(0.05, 0.95, sprintf('V_{NTD}=%.1fI_{bias}^4+%.1fI_{bias}^3+%.1fI_{bias}^2+%.4fI_{bias}+%.1f \nR_{NTD}: %.4f \nR^2: %.4f \nR_{adj}^2: %.4f', p(1)/10^6, p(2)/10^6, p(3)/10^6, p(4)/10^6, p(5)/10^6, p(4), rsq, rsq_adj), 'Units', 'normalized', 'HorizontalAlignment', 'left', 'VerticalAlignment', 'top', 'BackgroundColor', 'w', 'EdgeColor', 'k', 'FontSize', 18);

    % Plot the fitted line
    hold on;
    fplot(@(x) (p(1)*x.^4+p(2)*x.^3+p(3)*x.^2+p(4)*x+p(5))/10^6, x_range, 'r','LineWidth',2);
    legend('Data', 'Linear Fit', 'FontSize', 24);
    hold off;

    % Save the figure in .png form in a subdir called polyfit_output.
    % The name of the output png is the name of the origin .csv file.
    sub_dir = 'polyfit_output';
    if ~exist(sub_dir, 'dir')
        mkdir(sub_dir);
    end
    [pathstr,name,ext] = fileparts(filename);
    output_filename = fullfile(pathstr, 'polyfit_output', [name '.png']);
    saveas(gcf, output_filename);
    
    % Close the current figure to continue with the next file
    close(gcf);
    
    % Print the result for the current file
    fprintf('%s - %.4f\n', filename, p(4));
end