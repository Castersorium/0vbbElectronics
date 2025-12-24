import csv
import os

def process_csv(input_file, output_file):
    with open(input_file, 'r', newline='', encoding='utf-8') as infile, \
         open(output_file, 'w', newline='', encoding='utf-8') as outfile:
        
        reader = csv.reader(infile)
        writer = csv.writer(outfile)
        
        for i, row in enumerate(reader, start=1):
            # 处理每一行，将每个字段的最后一位数字替换为0
            processed_row = []
            for field in row:
                if field and field[-1].isdigit():
                    processed_field = field[:-1] + '0'
                else:
                    processed_field = field
                processed_row.append(processed_field)
            
            writer.writerow(processed_row)
            
            # 打印进度（每10万行）
            if i % 100000 == 0:
                print(f"已处理 {i} 行...")

if __name__ == "__main__":
    input_csv = "input.csv"  # 替换为你的输入文件路径
    output_csv = "output.csv"  # 输出文件路径
    
    print(f"开始处理文件: {os.path.abspath(input_csv)}")
    #process_csv(input_csv, output_csv)
    process_csv("1.csv","output_1.csv")
    process_csv("2.csv","output_2.csv")
    print(f"处理完成！结果已保存到: {os.path.abspath(output_csv)}")
