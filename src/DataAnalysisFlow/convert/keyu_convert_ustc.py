import numpy as np
import struct

def hex_str_to_signed_int(hex_str):
    """将16进制字符串转换为有符号32位整数"""
    value = int(hex_str, 16)
    return value

def convert_csv_to_bin_with_header(csv_path, bin_path, max_lines=None):
    # 读取CSV文件（每行是一个16进制字符串）
    with open(csv_path, 'r') as f:
        hex_lines = f.read().splitlines() 
        #if max_lines is not None:
        #    hex_lines = hex_lines[:max_lines]

    # 转换为十进制有符号整数
    #decimal_data = np.array([hex_str_to_signed_int(h.strip()) for h in hex_lines], dtype=np.int32)
    decimal_data = np.array([hex_str_to_signed_int(h.strip()) for h in hex_lines]).astype(np.int32)

    # 转换为电压值
    voltage = (decimal_data.astype(np.float64) - 2**31) * 2.5 / (2**31)

  # 电压转为整数：converted_data = ((V + 10) * (2^15) / 5)
    converted_data = ((voltage + 2.5) * (2**31) / 5.0).astype(np.dtype('<u4'))  # 小端32位无符号整数

    # 写入bin文件，包含header
    with open(bin_path, 'wb') as fileBIN2:
        # Header 1: 32位无符号整数 0x00006C20
        #fileBIN2.write(struct.pack('<I', 0x00006C20))
        fileBIN2.write(struct.pack('<I', 0x00006C20))
        # Header 2: 32位浮点数 采样频率 5000.0
        fileBIN2.write(struct.pack('<f', 5000.0))
        # Header 3: 32位浮点数 ADC全范围 20.0
        fileBIN2.write(struct.pack('<f', 5.0))
        # 写入数据体
        fileBIN2.write(converted_data.tobytes())


    print(f"转换完成：{csv_path} → {bin_path}，共写入 {len(voltage)} 个数据点")

# 示例用法
convert_csv_to_bin_with_header("LD_4D4V_305x8Gain_4GohmLoad.csv", "LD_12h.bin")
#convert_csv_to_bin_with_header("output_1.csv", "channel1_6h.bin")
#convert_csv_to_bin_with_header("output_2.csv", "channel2_6h.bin")
#convert_csv_to_bin_with_header("output_1.csv", "000001_20250510T000175_001_000.bin")
