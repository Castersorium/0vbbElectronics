# tdms2bin.jl

# 对于第一次使用Julia的用户，需要安装PyCall模块，安装方法如下：
# 1. 输入Pkg.add("PyCall")，安装PyCall模块
# 2. 输入ENV["PYTHON"] = "/usr/bin/python3.10"，设置Python的版本和路径
# 3. 输入Pkg.build("PyCall") ，重新安装PyCall模块
# 4. 重新启动Julia，输入using PyCall，加载PyCall模块

## 加载PyCall模块
using PyCall
np = pyimport("numpy")
npTDMS = pyimport("nptdms")

## 加载Dates模块
using Dates

## 加载Filesystem模块
using Base.Filesystem

## 定义基于北京时间的本地时间戳转换函数
function fromTimestamp(timestamp::Float64)
    LocalTimeShift = Dates.Hour(8)
    return (Dates.unix2datetime(timestamp) + LocalTimeShift)
end

## 定义TDMSdataConverter类
struct TDMSdataConverter
    group_names::Vector{String}
    channel_names::Vector{String}
    channel_start_times::Dict{String,Float64}
    time_track_channel::Dict{String,Vector{Float64}}
    data_channel::Dict{String,Any}
end

## 构造TDMSdataConverter对象
function createTDMSdataConverter(filename::String, isDebug::Bool=false)
    tdms_file = npTDMS.TdmsFile.read(filename)
    group_names = String[]
    channel_names = String[]
    channel_start_times = Dict{String,Float64}()
    time_track_channel = Dict{String,Vector{Float64}}()
    data_channel = Dict{String,Any}()

    # 初始化不为空的group的数量
    non_empty_group_count = 0

    for group in tdms_file.groups()
        group_name = group.name
        ## 检查group_name所属的channels是否为空，如果为空则跳过
        if isempty(group.channels())
            continue
        end

        # 如果group不为空，增加计数器
        non_empty_group_count += 1
        ## 检查不为空的group的数量是否大于2
        if non_empty_group_count >= 2
            error("The number of non-empty groups in the TDMS file is greater than 1.")
        end

        ## 在group_names中添加group_name
        push!(group_names, group_name)
        for channel in group.channels()
            channel_name = channel.name
            ## 在channel_names中添加channel_name
            push!(channel_names, channel_name)
            ## 在channel_start_times中添加channel.properties["wf_start_time"]
            channel_start_times[channel_name] = convert(Float64, (channel.properties["wf_start_time"] - np.datetime64("1970-01-01T00:00:00Z")) / np.timedelta64(1, "s"))
            time_track = channel.time_track()
            time_track_channel[channel_name] = time_track
            ## 在data_channel中添加channel_name和channel.data
            data = channel.data
            data_channel[channel_name] = data
        end
    end
    if isDebug
        println("group_names: ", group_names)
        println("channel_names: ", channel_names)
        println("channel_start_times: ", channel_start_times)
        for (channel_name, time_track) in time_track_channel
            println("time_track_channel[$channel_name]: ", time_track[1:min(end, 10)])
        end
        for (channel_name, data) in data_channel
            println("data_channel[$channel_name]: ", data[1:min(end, 10)])
        end
    end
    return TDMSdataConverter(group_names, channel_names, channel_start_times, time_track_channel, data_channel)
end

## 将TDMS数据转换为二进制文件并记录日志
function toBinary(converter::TDMSdataConverter, output_dir::String, original_filename::String)
    max_file_size = 1_000_000_000  # 每个文件的最大大小（1GB）
    original_basename = basename(original_filename)  # 获取原始文件名（不含路径）
    log_filename = joinpath(output_dir, "conversion_log_$original_basename.txt")
    open(log_filename, "w") do log_file
        write(log_file, "Conversion Log\n")
        write(log_file, "==============\n")
        write(log_file, "Original File: $original_filename\n")
        write(log_file, "==============================\n")

        # 遍历每个通道
        for (index, channel_name) in enumerate(converter.channel_names)
            data = converter.data_channel[channel_name]
            intervals = converter.time_track_channel[channel_name][2] - converter.time_track_channel[channel_name][1]
            sampling_frequency = 1.0f0 / intervals
            start_time = fromTimestamp(converter.channel_start_times[channel_name])
            date_str = Dates.format(start_time, "YYYYmmdd")
            time_str = Dates.format(start_time, "HHMMSS")

            # 格式化通道编号，作为文件名的一部分
            CHN = lpad(string(index), 3, '0')

            # 应用数据转换公式并转换为 UInt32 类型
            converted_data = UInt32.(round.(((data .+ 10.0) .* (2^32 - 1) ./ 20.0)))  # 先四舍五入再转换

            total_samples = length(converted_data)
            bytes_per_sample = sizeof(converted_data[1])
            samples_per_file = div(max_file_size, bytes_per_sample)
            num_files = ceil(Int, total_samples / samples_per_file)

            data_pointer = 1

            for PTN in 0:(num_files-1)
                PTN_str = lpad(string(PTN), 3, '0')
                filename = joinpath(output_dir, "000000_$(date_str)T$(time_str)_$(CHN)_$(PTN_str).bin")
                start_index = data_pointer
                end_index = min(data_pointer + samples_per_file - 1, total_samples)
                data_slice = converted_data[start_index:end_index]
                open(filename, "w") do bin_file
                    # Construct the header
                    # 1. 第一个 32 位整数：(Endianness << 8) + NBits
                    write(bin_file, UInt32(0x6C20))

                    # 2. 第二个 32 位浮点数：采样频率 (先转换为 Float32 再 reinterpret 为 UInt32)
                    write(bin_file, reinterpret(UInt32, Float32(sampling_frequency)))

                    # 3. 第三个 32 位浮点数：ADC 全范围 20.0 V
                    write(bin_file, reinterpret(UInt32, 20.0f0))

                    write(bin_file, data_slice)
                end

                data_pointer = end_index + 1
                println("已完成文件：$filename 的写入")
                if data_pointer > total_samples
                    break
                end
            end

            # 在日志文件中记录信息
            write(log_file, "Channel: $channel_name\n")
            write(log_file, "Start Time: $start_time\n")
            write(log_file, "File: $(date_str)T$(time_str)_$(CHN)_000.bin\n")
            write(log_file, "\n")

            println("已完成通道：$channel_name 的处理")
        end
    end
end

## 测试TDMSdataConverter类
# converter = createTDMSdataConverter("/home/shihongfu/temp/FDUdata/tdmsData/记录-2024-04-30 044136 472.tdms", true)

## 测试toBinary函数
# toBinary(converter, "/home/shihongfu/temp/FDUdata/binData/", "/home/shihongfu/temp/FDUdata/tdmsData/记录-2024-04-30 044136 472.tdms")

using ProgressMeter

## 遍历指定目录下的所有TDMS文件，并将其转换为CSV文件，保存到指定目录下
function convertTDMS2CSV(tdms_dir::String, csv_dir::String, isDebug::Bool=false)
    tdms_files = filter(x -> endswith(x, ".tdms"), readdir(tdms_dir))
    p = Progress(length(tdms_files), 1)  # 初始化进度条
    for tdms_file in tdms_files
        # 如果文件名包含"decimation"，跳过这个文件
        if occursin("decimation", tdms_file)
            continue
        end

        if isDebug
            println("Converting ", tdms_file, " to CSV file.")
        end
        converter = TDMSdataConverter(joinpath(tdms_dir, tdms_file), isDebug)
        toCSV(converter, joinpath(csv_dir, splitext(tdms_file)[1] * ".csv"))
        next!(p)  # 更新进度条
    end
end

## 批量转换TDMS文件为bin文件
function convertTDMS2Bin(tdms_dir::String, bin_dir::String, isDebug::Bool=false)
    tdms_files = filter(x -> endswith(x, ".tdms"), readdir(tdms_dir))
    p = Progress(length(tdms_files), 1)  # 初始化进度条
    for tdms_file in tdms_files
        # 如果文件名包含"decimation"，跳过这个文件
        if occursin("decimation", tdms_file)
            continue
        end

        if isDebug
            println("Converting ", tdms_file, " to bin file.")
        end
        converter = createTDMSdataConverter(joinpath(tdms_dir, tdms_file), isDebug)
        toBinary(converter, bin_dir, joinpath(tdms_dir, tdms_file))
        next!(p)  # 更新进度条
    end
end

## 运行convertTDMS2Bin函数
# 确保有足够的参数传入
if length(ARGS) < 2
    println("Usage: julia tdms2bin.jl <tdmsData path> <binData path>")
    exit(1)
end

tdmsData_path = ARGS[1]
binData_path = ARGS[2]

convertTDMS2Bin(tdmsData_path, binData_path)

## 退出Julia
exit()
