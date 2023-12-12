import pandas as pd
import numpy as np
import glob
import uproot

data_path    = "../../data/"
output_path  = "../../output/"
run_name     = "2023Nov/"
temperature = "39p9mK_PIDbad/"

data_path   = data_path   + run_name + temperature
output_path = output_path + run_name + temperature

# Replace 'your_file.xlsx' with the actual path to your Excel file
excel_pattern = "*.xlsx"
excel_file_path = data_path + excel_pattern
excel_file_name = glob.glob(excel_file_path)


output_file = output_path + "ColumnInfo.root"

# Read the Excel file into a DataFrame
df = pd.read_excel(excel_file_name[0])

# Extract specific columns
selected_columns = ['DAQCH', 'Onoff', 'V_Bol', 'I_Bol', 'Pre_gain', 'PGA_gain', 'Tot_gain', 'Bias_res', 'Filt_en','Cut_freq']
extracted_data = df[selected_columns]
filtered_data = extracted_data[extracted_data['Onoff'] == 1]

# Create a new DataFrame to store the extracted values
storing_data = filtered_data.copy()

# Columns have strings and have to extract digits to convert
columns_have_string = ['Pre_gain', 'Cut_freq', 'Bias_res']

# Loop through columns and perform extraction
for column in columns_have_string:
    storing_data[column] = filtered_data[column].str.replace(r'\D', '', regex=True).astype(float)

# Convert the Pandas DataFrame to a dictionary
data_dict = storing_data.to_dict(orient='list')

# Create a TTree and store the data in a ROOT file
with uproot.recreate(output_file) as file:
    # Create a TTree with the name 'channelTree'
    file["channelTree"] = data_dict
print(storing_data)

# Print the channel map and output to a file
show_columns = ['DAQCH', 'Name']
channel_map = df[show_columns]
#show_data=show_data[show_data['DAQCH']<7] #If you need to apply channel cut
channel_map_unique = channel_map.drop_duplicates(subset='DAQCH', keep='first')
print(channel_map_unique)
channel_map_unique.to_csv(output_path+'channels_map.csv', index=False)