function [fitresult, gof, para] = Keyu_Ployfit(varargin)

if nargin == 2
    xData = varargin{1};
    yData = varargin{2};
    chi_cut = 0.99;
elseif nargin == 3
    xData = varargin{1};
    yData = varargin{2};
    chi_cut = varargin{3};
else
    error('Invalid number of input arguments. Use either 1 or 2 arguments.');
end


if size(xData, 2) > size(xData, 1)
    xData = xData';  % 转置为列向量
end

if size(yData, 2) > size(yData, 1)
    yData = yData';  % 转置为列向量
end


% 排序 current 数据
%xData = sort(xData);
[fitresult, gof] = fit(xData, yData, 'poly2');
chi_square = gof.rsquare;
coeffs = coeffvalues(fitresult);
para = coeffs(2);

%绘制当前data
figure;
hold on;  % 保持当前图形
plot(xData, yData, 'o', 'LineWidth', 2);  % 绘制 ln(R) vs 1/sqrt(T)
plot(fitresult, 'r-');  % 绘制拟合曲线，'r-' 表示红色线
legend('show', 'Location', 'southeast');
legend(['Data'], ...
    ['R (\Omega)= ' num2str(para) newline ...
    'sqrt(R)= ' num2str(chi_square)]);

hold off;

if chi_square < chi_cut

    % 初始阈值
    threshold_fraction = 0.95;
    % 使用while循环进行迭代，直到拟合结果的斜率系数满足条件
    while true

        threshold_low = xData(floor(threshold_fraction * length(xData)));
        % 分段索引，选出电流小于等于阈值的部分
        idx1 = xData <= threshold_low;
        %xData = xData(xData <= threshold_low);
        %yData = yData(xData <= threshold_low);

        % 第一段数据拟合，进行线性拟合
        [fitresult, gof] = fit(xData(idx1), yData(idx1), 'poly1');
        %[fitresult, gof] = fit(xData, yData, 'poly1');
        chi_square = gof.rsquare;
        coeffs = coeffvalues(fitresult);
        para = coeffs(1);

        % 检查拟合系数
        if chi_square >= chi_cut
            break;
        else
            % 否则，调整前40%阈值的比例，减少0.05，继续下一次拟合
            threshold_fraction = threshold_fraction - 0.05;
            % 如果阈值已经低于某个范围，停止循环避免陷入死循环
            if threshold_fraction <= 0.05
                disp('KeyuWarning:Threshold fraction reached below 0.1, stopping further iterations.');
                break;
            end
        end
    end

    figure;
    hold on;  % 保持当前图形
    plot(xData, yData, 'o', 'LineWidth', 2);  % 绘制 ln(R) vs 1/sqrt(T)
    plot(fitresult, 'r-');  % 绘制拟合曲线，'r-' 表示红色线
    %legend('show', 'Location', 'southeast');
    legend(['Data'], ...
        ['R (\Omega)= ' num2str(para) newline ...
        'sqrt(R)= ' num2str(chi_square)]);
        
    hold off;

end



%varargout{1} = para;  % 第一个输出：一次项系数
%varargout{2} = chi_square;  % 第二个输出：拟合优度