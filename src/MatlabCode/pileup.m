% MATLAB script to plot the pile-up probability vs event rate

% Define the rise time and fall time
rise_time = 5.7452; % in milliseconds
fall_time = 14.5; % in milliseconds

% Total signal duration
signal_duration = rise_time + fall_time; % in milliseconds

% Define the range of event rates
event_rates = linspace(0, 100, 1000); % in events per second

% Calculate the pile-up probability for each event rate
pileup_probabilities = 1 - exp(-event_rates * signal_duration * 1e-3);

% Plot the pile-up probability vs event rate
figure;
plot(event_rates, pileup_probabilities, 'r-', 'LineWidth', 2);
xlabel('Event Rate (Hz)','FontSize',35);
ylabel('Pile-up Probability','FontSize',35);
title('Pile-up Probability vs Event Rate','FontSize',45);
grid on;
set(gca, 'FontSize', 24); 
% Display the maximum pile-up probability for reference
max_pileup_prob = max(pileup_probabilities);
disp(['Maximum Pile-up Probability: ', num2str(max_pileup_prob)]);