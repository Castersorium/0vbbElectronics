import csv

def split_csv(input_file, output_file1, output_file2):
    with open(input_file, 'r', newline='', encoding='utf-8') as infile, \
         open(output_file1, 'w', newline='', encoding='utf-8') as outfile1, \
         open(output_file2, 'w', newline='', encoding='utf-8') as outfile2:
        
        reader = csv.reader(infile)
        writer1 = csv.writer(outfile1)
        writer2 = csv.writer(outfile2)
        
        for i, row in enumerate(reader):
            if i % 2 == 0:  # 奇数行（从0开始计数）
                writer1.writerow(row)
            else:           # 偶数行
                writer2.writerow(row)
            
            # 每处理10000行打印一次进度
            if i % 10000 == 0:
                print(f"已处理 {i} 行...")

if __name__ == "__main__":
    input_csv = "LD_4D4V_305x8Gain_LMO_12V_305x32Gain_4GohmLoad.csv"   # 替换为你的输入文件路径
    output_csv1 = "1.csv"
    output_csv2 = "2.csv"
    
    print("开始处理文件...")
    split_csv(input_csv, output_csv1, output_csv2)
    print("处理完成！")
