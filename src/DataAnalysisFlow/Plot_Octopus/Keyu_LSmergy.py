import os
import shutil

def merge_log_files(a, b, c):
    # 目标文件用来存放合并后的log内容
    #output_file = os.path.join(a, "merged_logs.txt")
    output_file = os.path.join(a, f"{c}.log")
    
    with open(output_file, "w") as out_file:
        # 遍历a路径下的所有文件夹
        for root, dirs, files in os.walk(a):
            # 只处理文件夹名包含b的文件夹
            if b in os.path.basename(root):
                for file in files:
                    # 只处理文件名包含c的文件
                    if c in file and file.endswith(".log"):
                        file_path = os.path.join(root, file)
                        print(f"Merging file: {file_path}")
                        
                        # 打开并读取该log文件的内容
                        with open(file_path, "r") as in_file:
                            content = in_file.read()
                            # 将内容写入目标合并文件
                            out_file.write(content)
                            #out_file.write("\n" + "="*50 + "\n")  # 分隔符
    print(f"Logs merged into {output_file}")

# 示例调用
#merge_log_files("path/to/folder_a", "folder_b", "file_c")
#merge_log_files("./keyutemp", "24-11", 'CH7 T')
#merge_log_files("./keyutemp", "24-11", 'CH9 R')
#merge_log_files("./keyutemp", "24-11", 'CH10 R')
#merge_log_files("./keyutemp", "24-11", 'CH11 R')
#merge_log_files("./keyutemp", "24-11", 'CH12 R')
merge_log_files("./keyutemp", "24-11", 'CH6 T')

