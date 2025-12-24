import os

def merge(root_directory, output_directory, file_types, folder_filter):
    """
    搜索给定目录下，名称包含指定字符的文件夹（例如包含 "abc"），并合并特定类型的 .bin2 文件。
    
    Args:
        root_directory (str): 根目录路径。
        output_directory (str): 合并文件的输出目录。
        file_types (list): 需要合并的文件类型（例如 ['A.bin2', 'B.bin2', 'C.bin2']）。
        folder_filter (str): 需要匹配的文件夹名称中的字符（例如 "abc"）。
    """
    # 确保输出目录存在
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    
    # 初始化一个字典存储不同类型文件的路径
    file_groups = {file_type: [] for file_type in file_types}
    
    # 遍历第一层子文件夹，并筛选包含 folder_filter 的文件夹
    for folder in os.listdir(root_directory):
	# 判断文件夹名称是否包含指定字符 
        #if "875mK" in folder:
        if folder_filter in folder:
            folder_path = os.path.join(root_directory, folder)
            if not os.path.isdir(folder_path):
                continue  # 跳过非文件夹
            print(f"{folder_path}")
            
            # 遍历符合条件文件夹中的文件
            for root, dirs, files in os.walk(folder_path):
                for file in files:
                    for file_type in file_types:
                        if file == file_type:
                            file_groups[file_type].append(os.path.join(root, file))
    
    # 合并每种类型的文件
    for file_type, files in file_groups.items():
        if not files:
            print(f"未找到符合条件的 {file_type} 文件，跳过合并。")
            continue
        
        output_file = os.path.join(output_directory, f"merged_{file_type}")
        print(f"正在合并 {file_type} 文件，共 {len(files)} 个，输出到 {output_file}")
        
        # 合并文件
        with open(output_file, 'wb') as outfile:
            for file_path in files:
                with open(file_path, 'rb') as infile:
                    outfile.write(infile.read())
    
    print("所有指定类型的文件合并完成！")

# 示例用法
folder_filter = 'Heater'  # 文件夹名称中必须包含的字符
#root_directory = '/mnt/d/RUNs_data_analysis/RUN2410/Converted_Data/tdms/BKG_RUN2410_2_2/'  # 根目录路径
root_directory = '/mnt/d/RUNs_data_analysis/Heater3'
output_directory = "/mnt/d/trigger/analysis/Heater3"       # 输出目录路径
#file_types = ['1cmLMO.BIN2', 'LD.BIN2']  # 需要合并的文件类型
file_types = ['1cmLMO.bin']  # 需要合并的文件类型

merge(root_directory, output_directory, file_types, folder_filter)

