%用法1
%Keyu_RoverT用于画温度(mK)/阻值(ohm)关系
%Keyu_RoverT(file1)从单文件从读取要求格式：T，R,
%Keyu_RoverT(file1,file2)从LS两文件读取，默认格式，顺序：T，R
%用法2
%Keyu_getLCTR()用于测量LC的阻值，输出文件默认'LC_TR.log'
%Keyu_getLCTR(file.mat,channel,chi_th,T) 单文件，单通道，给定温度，测阻值
%Keyu_getLCTR(file.mat,channel,chi_th)多文件，单通道，支持文件名*PID*mK*自动寻找温度
%用法3
%Keyu_SNR 寻找LC中的peak，需要设定周期/稳定时间等



%从Blueforth原始数据中画出阻值与温度关系
 Keyu_RoverT('C:\Users\sky\Desktop\napp\实验\BF-log\25-05-05\CH6T.log', 'C:\Users\sky\Desktop\napp\实验\BF-log\25-05-05\CH9R.log');
% Keyu_RoverT('keyutemp/CH7 T.log', 'keyutemp/CH10 R.log');
% Keyu_RoverT('keyutemp/CH7 T.log', 'keyutemp/CH11 R.log');
% Keyu_RoverT('keyutemp/CH7 T.log', 'keyutemp/CH12 R.log');
   %Keyu_RoverT2('keyutemp/CH7 T.log', 'keyutemp/CH9 R.log');
   %Keyu_RoverT2('keyutemp/CH7 T.log', 'keyutemp/CH10 R.log');
   %Keyu_RoverT2('keyutemp/CH7 T.log', 'keyutemp/CH11 R.log');
   %Keyu_RoverT2('keyutemp/CH7 T.log', 'keyutemp/CH12 R.log');
   %Keyu_RoverT2('keyutemp/CH7 T.log', 'keyutemp/CH6 T.log');


%从LC中获取阻值，温度，并画出
%Keyu_getLCTR('D:\RUNs\RUN2410\LCS_2\*41B3X1N002*.mat',8,0.9);
%Keyu_getLCTR('.\matdata\USTC*.mat',8,0.9);
%Keyu_getLCTR('D:\RUNs\RUN2410\LCS_2\LC26_SB43_T7PID35p0mK_41B3X1N002_20241128.mat',8,0.9,35);
%Keyu_RoverT('LC_TR.log');
%Keyu_RoverT2('LC_TR.log');
