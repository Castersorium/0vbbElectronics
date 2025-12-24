#针对只有一个道的tdms文件
import time
import numpy as np
from nptdms import TdmsFile
import os

def process_tdms_file(fname):
    """处理TDMS文件并转换为BIN2格式"""
    fnewname = os.path.splitext(fname)[0]
    
    # 计时开始
    start = time.process_time()
    
    # 读取TDMS文件
    try:
        tdms_file = TdmsFile.read(fname)
    except Exception as e:
        print(f"Error reading TDMS file {fname}: {e}")
        return
    
    # 计时结束，打印文件读取时间
    end = time.process_time()
    print(f'Time to read the file {fname}: {end - start} Seconds')
    
    # 获取组和通道信息
    all_groups = tdms_file.groups()
    if len(all_groups) == 0:
        print(f"No groups found in {fname}")
        return
    
    all_group_channels = all_groups[0].channels()
    if len(all_group_channels) == 0:
        print(f"No channels found in {fname}")
        return
    
    # 处理数据并生成BIN2文件
    data_heat = all_group_channels[0]
    print(f"Make a new File: {fnewname}_keyu.BIN2")
    with open(fnewname + ".BIN2", 'wb') as fileBIN2_heat:
        num = 0
        print('Total: %d' % (len(data_heat) // 2000000))
        tot_num = len(data_heat) // 20000
        converted_heat = np.zeros(20000)

        for num in range(0, tot_num):
            if num % 100 == 0:
                print(num // 100)
            #converted_heat = ((data_heat[20000 * num:20000 * (num + 1)] + 10.0) * (2**15) / 5).astype(np.dtype('<u4'))
            converted_heat = ((data_heat[20000 * num:20000 * (num + 1)] + 10.0) * (2**24) / 20).astype(np.dtype('<u4'))
            txt_heat = converted_heat.tobytes()
            fileBIN2_heat.write(txt_heat)
    print(f"File {fnewname}.BIN2 created successfully.\n")


def find_and_process_tdms_files(root_folder):
    """遍历给定目录，找到所有的.tdms文件并处理"""
    for root, dirs, files in os.walk(root_folder):
        for file in files:
            #if file.endswith("keyu.tdms"):
            if file.endswith("587.tdms"):
                tdms_path = os.path.join(root, file)
                print(f"Found TDMS file: {tdms_path}")
                process_tdms_file(tdms_path)

# 设置要遍历的根目录
#root_directory = '/mnt/d/RUNs_data_analysis/RUN2409/Converted_Data/tdms'  # 修改为你的根目录
root_directory = '/mnt/d/RUNs/RUN2410/BKG_RUN2410_2_2/未命名项目/NI Project Data'
find_and_process_tdms_files(root_directory)

