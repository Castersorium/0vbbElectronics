function plot_psd(fres, psds, mode,wint,fs,legends)
    % Create a new figure window
    figure;    
    for i=1:length(legends)
    plot(fres{i}, psds{i}); % Plot the power spectrum density
    hold on;
    end
    set(gca, 'XScale', 'log');
    set(gca, 'YScale', 'log');
    if strcmp(mode, 'a')
        ylabel_str = "$PSD\left[V/\sqrt{Hz}\right]$";
    elseif strcmp(mode,'d')
        ylabel_str = "$PSD of displacement\left[m/\sqrt{Hz}\right]$";
    elseif strcmp(mode,'v')
        ylabel_str = "$PSD\left[V/\sqrt{Hz}\right]$";
    end
 
    legend(legends,'FontSize',24,'Location','northeast','Interpreter','none'); % Plot the legend with all file names
    xlabel("Frequency (Hz)","FontSize",24); % x-axis label
    ylabel(ylabel_str,"Interpreter","latex","FontSize",24); % y-axis label
    xlim([1/wint fs/2]);
    grid on; % show grid
    title("Power Spectrum Density"); % plot title
    set(gca,'FontSize',24); % set font size of axis numbers
    hold off;
end