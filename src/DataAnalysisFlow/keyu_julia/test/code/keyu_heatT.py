import matplotlib.pyplot as plt
import numpy as np
import time
import math
import random
import sys
import scipy.signal
from scipy import signal, stats, fft
from scipy.optimize import curve_fit
import uproot
import os
def func_exp(x, a, b, c):
    return a * np.exp(-b * x) + c
#def _trigger(fname):
def process_bin2_file(fname):
    # 路径结尾需要带有文件夹标识符"/"
    #heat_name = 'LD.BIN2'
    #heat_path = '/mnt/d/RUNs_data_analysis/RUN2409/Converted_Data/tdms/NewSB4_shuju_09142024/LD/'
    heat_path, heat_name = os.path.split(fname)
    if not heat_path.endswith("/"):
        heat_path += "/"
    fs = 5000 #Hz

    data_bytes = 4
    offset_points = -20 #negative value usually for triggering light using heat.
    heat_correreject = 0.5 # (0, 1.0)
    heat_paralength = 300 #should be less than half of window length

    threshold = 4.0

    heat_signaltemplate = np.loadtxt(heat_path+'data/signaltemplate.txt',dtype=float)
    heat_signaltemplate /= np.max(heat_signaltemplate)
    heat_diffnoiseps = np.loadtxt(heat_path+'data/diff_nps.txt',dtype=float)
    heat_length = np.size(heat_signaltemplate)
    heat_peakpos = np.argmax(heat_signaltemplate)+1 #sample points
    #heat_length = np.shape(heat_signaltemplate)[0]
    heat_ref_dt = 0
    heat_ref_rt = 0
    for i in reversed(heat_signaltemplate[:heat_peakpos]):
        if i < 0.9:
            if i < 0.1:
                break
            heat_ref_rt+=1
    for i in heat_signaltemplate[heat_peakpos:]:
        if i < 0.9:
            if i < 0.3:
                break
            heat_ref_dt+=1
    print(heat_ref_rt)
    print(heat_ref_dt)
    #在diff 尾端拼接一个0， 默认情况下，axis=0可以不写
    heat_difftemplate = np.concatenate((np.diff(heat_signaltemplate),[0]),axis=0)  

    heat_win = scipy.signal.windows.tukey(heat_length, alpha=heat_peakpos/heat_length, sym=True)
    #计算归一化常数时，不应该使用余弦衰减函数
    heat_OF_norm = heat_length/np.sum(np.abs(np.fft.fft(heat_difftemplate))**2/np.append(heat_diffnoiseps,heat_diffnoiseps[-1]))
    #heat_OF_norm = heat_length/np.sum(np.abs(np.fft.fft(heat_difftemplate)*heat_win)**2/np.append(heat_diffnoiseps,heat_diffnoiseps[-1]))
    print(r'Filtered baseline sigma is: {0:0.2f}'.format(math.sqrt(heat_OF_norm/heat_length)*2**23/10.24))
    #threshold *= math.sqrt(heat_OF_norm/heat_length)*2**23/10.24
    threshold *= math.sqrt(heat_OF_norm/heat_length)
    print(threshold)

    heat_freq = np.fft.fftfreq(heat_length, 1./fs)
    heat_phase = np.cos(heat_freq*2*np.pi*heat_peakpos/fs)-np.sin(heat_freq*2*np.pi*heat_peakpos/fs)*1j
    #heat_freqI = np.arange(heat_length)
    #heat_phase = np.cos(heat_freqI*2*np.pi*heat_peakpos/heat_length)-np.sin(heat_freqI*2*np.pi*heat_peakpos/heat_length)*1j

    heat_OF_f = heat_OF_norm*np.fft.fft(heat_difftemplate).conjugate()*heat_phase/np.append(heat_diffnoiseps, heat_diffnoiseps[-1])
    heat_OF_t = np.real(np.fft.ifft(heat_OF_f))
    heat_OF_2t = np.concatenate((heat_OF_t[0:heat_length//2]*heat_win[heat_length//2:heat_length],np.zeros(heat_length),heat_OF_t[heat_length//2:heat_length]*heat_win[0:heat_length//2]), axis=0)
    heat_OF_2f = np.fft.fft(heat_OF_2t)
    heat_OF_2f[0] = 0

    heat_template_2t = np.concatenate((np.zeros(heat_length//2),heat_win*heat_signaltemplate,np.zeros(heat_length//2)), axis=0)
    heat_OF_template_t = np.real(np.fft.ifft(heat_OF_f*np.fft.fft(heat_difftemplate*heat_win)))
    heat_OF_template_2t = np.real(np.fft.ifft(np.fft.fft(np.concatenate((np.zeros(heat_length//2),heat_win*np.append(np.diff(heat_signaltemplate),0),np.zeros(heat_length//2)), axis=0))*heat_OF_2f))#[heat_length/2:heat_length/2+heat_length]
    '''
    '''
    if heat_peakpos < heat_length//2:
        break_point = 2*heat_length - heat_length//2 + heat_peakpos
    else:
        break_point = - heat_length//2 + heat_peakpos
    heat_OF_template_center = np.concatenate((heat_OF_template_2t[break_point:heat_length*2],heat_OF_template_2t[0:break_point]),axis=0)
    heat_peakvalley_length = abs(np.argmax(heat_OF_template_2t)-np.argmin(heat_OF_template_2t))

    heat_OF_norm = heat_length/np.sum(np.abs(np.fft.fft(heat_difftemplate))**2/np.append(heat_diffnoiseps,heat_diffnoiseps[-1]))
    heat_sigmaL2 = (heat_OF_norm**2)*np.sum((np.abs(np.fft.fft(heat_difftemplate))**2)[heat_length//(2*heat_paralength):heat_length-heat_length//(2*heat_paralength)]/np.append(heat_diffnoiseps,heat_diffnoiseps[-1])[heat_length//(2*heat_paralength):heat_length-heat_length//(2*heat_paralength)]) 
    heat_meantime_template = np.sum(np.arange(heat_length)[heat_peakpos-2*heat_ref_rt:heat_peakpos+4*heat_ref_dt]*heat_signaltemplate[heat_peakpos-2*heat_ref_rt:heat_peakpos+4*heat_ref_dt])
    print(heat_peakvalley_length)
    print(heat_paralength)
    '''
    fig0, ax0 = plt.subplots(nrows=1, ncols=1)
    ax0.plot(np.arange(2*heat_length)/fs, heat_OF_template_2t, c='r', label='OF_2t')
    ax0.plot(np.arange(2*heat_length)/fs, heat_template_2t, c='g', label='Template_2t')
    ax0.plot(np.arange(heat_length)/fs+heat_length/fs/2, heat_OF_template_t, c='b', label='OF_t')
    ax0.legend()
    ax0.grid(True)
    ax0.set_xlabel('Time (s)')
    ax0.set_ylabel('Height')
    #ax.set_title(r'Histogram of Baseline: $\sigma$={0:.2f}keV'.format(abs(popt4[2])))
    plt.show()
    '''

    ##--Trigger Program---------
    ##---get file length
    tot_length = os.path.getsize(heat_path+heat_name)//data_bytes
    ##--sample window length and each step has 1/10 overlap
    heat_samplelength = 100#heat_para_length
    if heat_samplelength < 4*heat_peakvalley_length:
        heat_samplelength = 4*heat_peakvalley_length
        print(r'Automatically set step window length is {0:d} points.'.format(4*heat_peakvalley_length))
    if heat_paralength < 4*heat_peakvalley_length:
        heat_paralength = 4*heat_peakvalley_length
        print(r'Automatically set step parameter length is {0:d} points.'.format(4*heat_peakvalley_length))
    heat_steplength = heat_samplelength-heat_samplelength//10
    tot_num = tot_length//heat_steplength
    print(tot_num)
    heat_file = open(heat_path+heat_name,"r")
    #txt = heat_file.read(2*sample_length)
    #v = np.frombuffer(txt,dtype='<i2')
    #heat_file.seek(data_bytes*10)
    #print(v.astype(int))
    break_i = 0
    heat_trigpos_raw = 0
    heat_trigpos_fil = 0

    #[trigpos(s,1),rawmax,filtmax,baseline,baseline_rms,param,chi,tvl,tvr,pf(1),pf(2),risetime_val,decaytime_val,delayed_amp,meantime_value,pulse_area,f_pulse_area,chi_timedom,filtnoise];
    list_heat_trigpos_fil = []
    list_heat_rawamp = []
    list_heat_filamp = []
    list_heat_baseline  = []
    list_heat_baselineRMS  = []
    list_heat_corre  = []
    list_heat_tv  = []
    list_heat_tvl  = []
    list_heat_tvr  = []
    list_heat_SI = []
    list_heat_baseline_slope  = []
    list_heat_fitted_baseline  = []
    list_heat_fitted_rawamp  = []
    list_heat_rt  = []
    list_heat_dt  = []
    list_heat_delayamp = []
    list_heat_meantime= []
    list_heat_rawarea  = []
    list_heat_filarea = []
    list_heat_corre_narrow = []
    for k in range(000000, tot_num):
        if k%20000 == 0:
            print(r'{0:d}/{1:d} is processing.'.format(k//20000,tot_num//20000))
        heat_file.seek(k*data_bytes*heat_steplength,0)
        ##---use signed int32 to load unsigned int32 data, 
        ##---otherwise, you will have error when computer the diffrential data
        heat_sampledata = np.fromfile(heat_file, dtype=np.dtype('<u4'), count=heat_samplelength).astype(np.dtype('<i4'))
        max_index = np.argmax(heat_sampledata)
        ##--最大值小于一个滤波窗口长度，跳过
        if max_index + 1 + k*heat_steplength < heat_peakpos:
            continue
        if tot_length - (max_index + 1 + k*heat_steplength) < heat_length-heat_peakpos:
            continue
        ##--最大值在截取窗口两端，跳过
        if max_index + 1 == heat_samplelength or max_index == 0:
            continue
        ##--重复触发，跳过
        if max_index + 1 + k*heat_steplength == heat_trigpos_raw:
            continue
        heat_trigpos_raw = max_index + 1 + k*heat_steplength
        
        ##---以得到的最大值位置为中心，截取一个滤波窗口数据进行滤波
        heat_file.seek(data_bytes*(heat_trigpos_raw - heat_peakpos),0)
        heat_data = np.fromfile(heat_file, dtype=np.dtype('<u4'), count=(heat_length)).astype(np.dtype('<i4'))
        heat_diffdata = np.append(np.diff(heat_data),0)
        heat_diffdata_2t = np.concatenate((np.zeros(heat_length//2),heat_win*heat_diffdata,np.zeros(heat_length//2)), axis=0)
        heat_data_OF = np.real(np.fft.ifft(np.fft.fft(heat_diffdata_2t)*heat_OF_2f))

        heat_peakpos_tmp = 0
        heat_peakpos_tmp = np.argmax(heat_data_OF[heat_length//2+heat_peakpos-heat_peakvalley_length//2:heat_length//2+heat_peakpos+heat_peakvalley_length//2])
        if heat_peakpos_tmp + 1 == (heat_peakvalley_length//2)*2 or heat_peakpos_tmp == 0:
            continue
        heat_peakpos_tmp += heat_length//2+heat_peakpos-heat_peakvalley_length//2
        '''
        heat_peakpos_tmp = 0
        heat_peakpos_tmp = np.argmax(heat_data_OF[heat_length//2+heat_peakpos-heat_samplelength//2:heat_length//2+heat_peakpos+heat_samplelength//2])
        if heat_peakpos_tmp + 1 == (heat_samplelength//2)*2 or heat_peakpos_tmp == 0:
            continue
        heat_peakpos_tmp += heat_length//2+heat_peakpos-heat_samplelength//2*2
        '''
        if heat_peakpos_tmp + heat_trigpos_raw - heat_peakpos - heat_length//2 == heat_trigpos_fil:
            continue
        heat_trigpos_fil = heat_peakpos_tmp + heat_trigpos_raw - heat_peakpos-heat_length//2
        
        heat_filamp = heat_data_OF[heat_peakpos_tmp]
        if heat_filamp < threshold:
            continue
        
        if heat_peakpos_tmp < heat_length//2+heat_peakpos:
            break_point = -(heat_peakpos_tmp - heat_length//2-heat_peakpos)
        else:
            break_point = 2*heat_length-(heat_peakpos_tmp - heat_length//2-heat_peakpos)
        heat_OF_template_shifted = np.concatenate((heat_OF_template_2t[break_point:heat_length*2],heat_OF_template_2t[0:break_point]),axis=0)
        heat_corre = np.corrcoef(heat_data_OF[heat_peakpos_tmp-heat_paralength:heat_peakpos_tmp+heat_paralength],heat_OF_template_center[heat_length-heat_paralength:heat_length+heat_paralength])[0,1]
        heat_corre_tmp = np.corrcoef(heat_data_OF[heat_peakpos_tmp-heat_peakvalley_length:heat_peakpos_tmp+heat_peakvalley_length],heat_OF_template_center[heat_length-heat_peakvalley_length:heat_length+heat_peakvalley_length])[0,1]
        #heat_corre_tmp = np.corrcoef(heat_data_OF[heat_peakpos_tmp-heat_peakvalley_length:heat_peakpos_tmp+heat_peakvalley_length], heat_OF_template_center[heat_length-heat_peakvalley_length:heat_length+heat_peakvalley_length])[0,1]
        #if(heat_corre_tmp < 0.8 or abs(heat_corre_tmp-heat_corre) > 0.5):
        #if(abs(heat_corre_tmp-heat_corre) > 0.5):
        #    continue
        if heat_corre < heat_correreject:
            continue
        
        heat_tvl = np.sum((heat_data_OF[heat_peakpos_tmp-heat_paralength:heat_peakpos_tmp]-heat_filamp*heat_OF_template_center[heat_length-heat_paralength:heat_length])**2)/heat_paralength
        heat_tvr = np.sum((heat_data_OF[heat_peakpos_tmp:heat_peakpos_tmp+heat_paralength]-heat_filamp*heat_OF_template_center[heat_length:heat_length+heat_paralength])**2)/heat_paralength
        heat_tv = heat_tvl + heat_tvr
        #heat_tv = np.sum((heat_data_OF[heat_peakpos_tmp-heat_paralength:heat_peakpos_tmp+heat_paralength]-heat_filamp*heat_OF_template_center[heat_length-heat_paralength:heat_length+heat_paralength])**2)/heat_paralength/2
        #heat_data #heat_length*1, peak at heat_peakpos
        #heat_diffdata #heat_length*1, 
        #heat_data_OF #heat_length*2, peak at heat_peakpos_tmp

        heat_baseline = np.mean(heat_data[0:heat_peakpos-4*heat_ref_rt])
        heat_baselineRMS = np.std(heat_data[0:heat_peakpos-4*heat_ref_rt])

        ##---fit baseline
        [heat_baseline_slope,heat_fitted_baseline] = np.polyfit(np.arange(heat_peakpos-4*heat_ref_rt)-heat_peakpos+4*heat_ref_rt+1,heat_data[0:heat_peakpos-4*heat_ref_rt],1)
        #[heat_baseline_slope,heat_fitted_baseline] = np.polyfit(np.arange(heat_peakpos-4*heat_ref_rt),heat_data[0:heat_peakpos-4*heat_ref_rt],1)
        [heat_fitted_rawamp,heat_fitted_baseline] = np.polyfit(heat_signaltemplate[heat_peakpos-2*heat_ref_rt:heat_peakpos+3*heat_ref_dt],heat_data[heat_peakpos-2*heat_ref_rt:heat_peakpos+3*heat_ref_dt],1)
        
        heat_data_norm = heat_data - heat_fitted_baseline
        heat_rawamp = heat_data_norm[heat_peakpos]#-heat_fitted_baseline
        heat_chi2_raw = np.sum((heat_data_norm-heat_fitted_rawamp*heat_signaltemplate)**2)/heat_length
        heat_data_norm = heat_data_norm/heat_rawamp
        
        heat_dt = 0
        heat_rt = 0
        rise_start = 0
        rise_end = 0
        decay_end = 0
        decay_start = 0
        for i in reversed(heat_data_norm[heat_peakpos-4*heat_ref_rt:heat_peakpos]):
            rise_start += 1
            if i < 0.9:
                if i < 0.1:
                    break
                heat_rt+=1
            else:
                rise_end += 1
        if heat_data_norm[heat_peakpos-rise_end+1] == heat_data_norm[heat_peakpos-rise_end]:
            heat_rt += 0.5
        else:
            heat_rt += (0.9 - heat_data_norm[heat_peakpos-rise_end])/(heat_data_norm[heat_peakpos-rise_end+1] - heat_data_norm[heat_peakpos-rise_end])
        if heat_data_norm[heat_peakpos-rise_start+2] == heat_data_norm[heat_peakpos-rise_start+1]:
            heat_rt += 0.5
        else:
            heat_rt += (heat_data_norm[heat_peakpos-rise_start+2]-0.1)/(heat_data_norm[heat_peakpos-rise_start+2] - heat_data_norm[heat_peakpos-rise_start+1])
        
        for i in heat_data_norm[heat_peakpos:heat_peakpos+3*heat_ref_dt]:
            decay_end += 1
            if i < 0.9:
                if i < 0.3:
                    break
                heat_dt+=1
            else:
                decay_start += 1
        heat_dt += (heat_data_norm[heat_peakpos+decay_end-2]-0.3)
        if heat_data_norm[heat_peakpos+decay_end-2] == heat_data_norm[heat_peakpos+decay_end-1]:
            heat_dt += 0.5
        else:
            heat_dt += (heat_data_norm[heat_peakpos+decay_end-2]-0.3)/(heat_data_norm[heat_peakpos+decay_end-2] - heat_data_norm[heat_peakpos+decay_end-1])
        heat_dt += (0.9-heat_data_norm[heat_peakpos+decay_start])
        if heat_data_norm[heat_peakpos+decay_start-1] == heat_data_norm[heat_peakpos+decay_start]:
            heat_dt += 0.5
        else:
            heat_dt += (0.9-heat_data_norm[heat_peakpos+decay_start])/(heat_data_norm[heat_peakpos+decay_start-1] - heat_data_norm[heat_peakpos+decay_start])
        heat_rt /= fs
        heat_dt /= fs

        heat_SI = np.sum((heat_data_OF[heat_peakpos_tmp-heat_paralength:heat_peakpos_tmp+heat_paralength]-heat_filamp*heat_OF_template_center[heat_length-heat_paralength:heat_length+heat_paralength])**2)/(heat_paralength*2-2)/heat_sigmaL2/heat_length**2

        heat_meantime = np.sum(np.arange(heat_length)[heat_peakpos-2*heat_ref_rt:heat_peakpos+4*heat_ref_dt]*heat_data_norm[heat_peakpos-2*heat_ref_rt:heat_peakpos+4*heat_ref_dt])-heat_meantime_template
        heat_delayamp = np.mean(heat_data_norm[heat_peakpos:heat_peakpos+heat_peakvalley_length])
        heat_rawarea = np.trapz(heat_data_norm[heat_peakpos-2*heat_ref_rt:heat_peakpos+4*heat_ref_dt])
        heat_filarea = np.trapz(heat_data_OF[heat_peakpos_tmp-heat_paralength:heat_peakpos_tmp+heat_paralength]/heat_filamp)

        '''
        print('Trigger position:',heat_trigpos_fil)
        print('Baseline:')
        print(heat_data[0])
        print(heat_baseline)
        print(heat_fitted_baseline)
        print(heat_peakpos)
        print(np.argmax(heat_data)+1)
        print('Amplitude:')
        print(heat_fitted_rawamp)
        print(heat_rawamp)
        print(heat_filamp)
        
        print(heat_rt)
        print(heat_dt)
        print(heat_SI)
        print(heat_corre_tmp)
        print(heat_corre)
        break_i += 1
        if break_i == 6:
            break
        '''
        list_heat_trigpos_fil.append(heat_trigpos_fil)
        list_heat_filamp.append(heat_filamp)
        list_heat_baseline.append(heat_baseline)
        list_heat_baselineRMS.append(heat_baselineRMS)
        list_heat_tv.append(heat_tv)
        list_heat_tvr.append(heat_tvr)
        list_heat_baseline_slope.append(heat_baseline_slope)
        list_heat_fitted_rawamp.append(heat_fitted_rawamp)
        list_heat_dt.append(heat_dt )
        list_heat_rawarea.append(heat_rawarea)
        list_heat_filarea.append(heat_filarea)
        list_heat_rawamp.append(heat_rawamp)
        list_heat_corre.append(heat_corre)
        list_heat_tvl.append(heat_tvl)
        list_heat_SI.append(heat_SI)
        list_heat_fitted_baseline.append(heat_fitted_baseline)
        list_heat_rt.append(heat_rt)
        list_heat_delayamp.append(heat_delayamp)
        list_heat_meantime.append(heat_meantime)
        list_heat_corre_narrow.append(heat_corre_tmp)
        

    '''
    fig, ax = plt.subplots(nrows=1, ncols=1)
    ax.plot(np.arange(2*heat_length)/fs, heat_data_OF, c='r', label='OF_data_2t')
    ax.plot(np.arange(2*heat_length)/fs, heat_OF_template_shifted*heat_filamp, c='g', label='OF_template_2t')
    ax.plot(np.arange(heat_length)/fs+heat_length//2/fs, heat_data-heat_data[0], c='b', label='Data_2t')
    #ax.plot(np.arange(heat_length)/fs+heat_length//2/fs, heat_data_norm*heat_filamp, c='b', label='Data_2t')
    #ax.plot(np.arange(2*heat_length)/fs, heat_diffdata, c='b', label='Diffdata_2t')
    ax.legend()
    ax.grid(True)
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Height')
    #ax.set_title(r'Histogram of Baseline: $\sigma$={0:.2f}keV'.format(abs(popt4[2])))
    plt.show()

    '''
    arr_heat_trigpos_fil = np.array(list_heat_trigpos_fil)
    arr_heat_rawamp = np.array(list_heat_rawamp)
    arr_heat_filamp = np.array(list_heat_filamp)
    arr_heat_baseline = np.array(list_heat_baseline)
    arr_heat_baselineRMS = np.array(list_heat_baselineRMS)
    arr_heat_corre = np.array(list_heat_corre)
    arr_heat_tv  = np.array(list_heat_tv)
    arr_heat_tvl = np.array(list_heat_tvl)
    arr_heat_tvr = np.array(list_heat_tvr)
    arr_heat_SI = np.array(list_heat_SI)
    arr_heat_baseline_slope = np.array(list_heat_baseline_slope)
    arr_heat_fitted_baseline = np.array(list_heat_fitted_baseline)
    arr_heat_fitted_rawamp = np.array(list_heat_fitted_rawamp)
    arr_heat_rt = np.array(list_heat_rt)
    arr_heat_dt = np.array(list_heat_dt)
    arr_heat_delayamp = np.array(list_heat_delayamp)
    arr_heat_meantime = np.array(list_heat_meantime)
    arr_heat_rawarea = np.array(list_heat_rawarea)
    arr_heat_filarea = np.array(list_heat_filarea)
    arr_heat_corre_narrow = np.array(list_heat_corre_narrow)

    eventfile = uproot.recreate(heat_path+"data/light.root")
    eventfile["tree1"] = {"trigpos":arr_heat_trigpos_fil,"rawamp":arr_heat_rawamp,"filamp":arr_heat_filamp,"baseline":arr_heat_baseline,"baselineRMS":arr_heat_baselineRMS,"correlation":arr_heat_corre,"tv":arr_heat_tv,"tvl":arr_heat_tvl,"tvr":arr_heat_tvr,"SI":arr_heat_SI,"baseline_slope":arr_heat_baseline_slope,"fitted_baseline":arr_heat_fitted_baseline,"fitted_rawamp":arr_heat_fitted_rawamp,"risetime":arr_heat_rt,"decaytime":arr_heat_dt,"delayamp":arr_heat_delayamp,"meantime":arr_heat_meantime,"rawarea":arr_heat_rawarea,"filarea":arr_heat_filarea,"correlation_narrow":arr_heat_corre_narrow}
    return

#_trigger()


def find_and_process_bin2_files(root_folder):
    """遍历给定目录，找到所有的.tdms文件并处理"""
    for root, dirs, files in os.walk(root_folder):
        for file in files:
             #if file.endswith("LD.BIN2") or file.endswith("LMO.BIN2"):
             #if file.endswith(".BIN2") and file != 'diff_filtereddata.BIN2':
             #if file.endswith("TriggerEvent.root"):
             if file.endswith('.BIN2') and 'data' in dirs:
                bin2_path = os.path.join(root, file)
                print(f"Found BIN2 file: {bin2_path}")
                
                #判断是否已经运行过了
                data_folder_path = os.path.join(root, 'data')
                light_root_path = os.path.join(data_folder_path, 'light.root')
                
                if not os.path.exists(light_root_path):
                    process_bin2_file(bin2_path)
                else:
                    print("Already heatT; Next~")

# 设置要遍历的根目录
#root_directory = '/mnt/d/RUNs_data_analysis/RUN2409/Converted_Data/tdms'  # 修改为你的根目录
#root_directory = '/mnt/d/RUNs_data_analysis/RUN2410/Converted_Data/tdms/BKG_RUN2410_2_2/'
root_directory = './analysis/Heater'
find_and_process_bin2_files(root_directory)

