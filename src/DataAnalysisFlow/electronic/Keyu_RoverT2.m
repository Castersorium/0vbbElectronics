function Keyu_RoverT2(varargin)
% Check the number of input arguments
if nargin == 1
    % If there is one argument, read the file and extract data
    file = varargin{1};  % Get the file name
    data1 = readmatrix(file);  % Read the file
    T = data1(:, 1);  % Select the first column as temperature
    R = data1(:, 2);  % Select the second column as resistance
    T = T/1000;
    disp('Data from the one file:T,R');
elseif nargin == 2
    % If there are two arguments, read both files and extract data
    file1 = varargin{1};  % Get the first file name
    file2 = varargin{2};  % Get the second file name

    % Read the first file
    data1 = readmatrix(file1);  % Read the first file
    T = data1(:, 3);  % Select the third column as temperature

    % Read the second file
    data2 = readmatrix(file2);  % Read the second file
    R = data2(:, 3);  % Select the third column as resistance

    disp('Data from two files: T,R');
else
    error('Invalid number of input arguments. Use either 1 or 2 arguments.');
end


% Find the indices where R = 0
indices_to_remove = (R == 0) | (T == 0);  % 使用按位或运算符
% Remove the data corresponding to R = 0 from both T and R
T(indices_to_remove) = [];  % Remove corresponding temperature values
R(indices_to_remove) = [];  % Remove corresponding resistance values


% 2. 绘制 R vs T
figure;  % 创建一个新的图形窗口
plot(T, R, '-o', 'LineWidth', 2);  % 绘制 R vs T
xlabel('Temperature (T)/K');  % X轴标签
ylabel('Resistance (R)/ohm');  % Y轴标签
title('Resistance vs Temperature');  % 图标题
grid on;  % 添加网格

% 3. 绘制 ln(R) vs 1/sqrt(T)
% 使用 ln(R) 和 1/sqrt(T) 进行绘制
x_data = 1 ./ sqrt(T);  % 1/sqrt(T)
y_data = log(R);  % ln(R)


%indices_to_remove2 = (x_data <150) | (x_data>200);  % 使用按位或运算符
%indices_to_remove2 = (x_data <4) | (x_data>7);  % 使用按位或运算符
%indices_to_remove2 = (x_data <3) | (x_data>6);  % 使用按位或运算符
 indices_to_remove2 = (x_data < 0);  % 使用按位或运算符
 % Remove the data corresponding to R = 0 from both T and R
 x_data(indices_to_remove2) = [];  % Remove corresponding temperature values
 y_data(indices_to_remove2) = [];  % Remove corresponding resistance values


%[para,chi_square]=Keyu_Ployfit(x_data,y_data,0.9);
%[fitresult, gof]=Keyu_Ployfit(x_data,y_data,0.99);
[fitresult, gof] = fit(x_data, y_data, 'poly1');
%[fitresult, gof] = fit(xData, yData, 'poly1');
chi_square = gof.rsquare;
coeffs = coeffvalues(fitresult);
para = coeffs(1)^2;
para2 = exp(coeffs(2));
disp(fitresult);
disp(gof);


figure;
hold on;  % 保持当前图形
plot(x_data, y_data, 'o', 'LineWidth', 2);  % 绘制 ln(R) vs 1/sqrt(T)
plot(fitresult, 'r-');  % 绘制拟合曲线，'r-' 表示红色线
%legend('show', 'Location', 'southeast');
legend(['Data'], ...
    ['T0= ' num2str(para) newline ...
    'R0= ' num2str(para2) newline ...
    'sqrt(R)= ' num2str(chi_square)]);

hold off;


% 显示拟合参数
%disp(['拟合的斜率 (slope): ', num2str(p(1))]);
%disp(['拟合的截距 (intercept): ', num2str(p(2))]);

end
