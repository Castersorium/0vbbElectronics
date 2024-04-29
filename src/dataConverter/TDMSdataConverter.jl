# TDMSdataConverter.jl

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

## 定义基于北京时间的本地时间戳转换函数
function fromTimestamp(timestamp::Float64)
    LocalTimeShift = Dates.Hour(8)
    return (Dates.unix2datetime(timestamp) + LocalTimeShift)
end

## 定义TDMSdataConverter类
struct TDMSdataConverter
    group_names::Vector{String}
    channel_names::Vector{String}
    channel_start_times::Vector{Float64}
    time_track_channel::Dict{String,Vector{Float64}}
    data_channel::Dict{String,Any}
end

## 定义TDMSdataConverter类的构造函数
function TDMSdataConverter(filename::String, isDebug::Bool=false)
    tdms_file = npTDMS.TdmsFile.read(filename)
    ## 检查tdms_file的group的元素数量不得大于2，如果大于2则报出错误，并且退出
    if length(tdms_file.groups()) > 2
        error("The number of groups in the TDMS file is greater than 2.")
    end
    group_names = String[]
    channel_names = String[]
    channel_start_times = Float64[]
    time_track_channel = Dict{String,Vector{Float64}}()
    data_channel = Dict{String,Any}()
    for group in tdms_file.groups()
        group_name = group.name
        ## 检查group_name所属的channels是否为空，如果为空则跳过
        if isempty(group.channels())
            continue
        end
        ## 在group_names中添加group_name
        push!(group_names, group_name)
        for channel in group.channels()
            channel_name = channel.name
            ## 在channel_names中添加channel_name
            push!(channel_names, channel_name)
            ## 在channel_start_times中添加channel.properties["wf_start_time"]
            push!(channel_start_times, convert(Float64, (channel.properties["wf_start_time"] - np.datetime64("1970-01-01T00:00:00Z")) / np.timedelta64(1, "s")))
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
        println("time_track: ", time_track_channel[channel_names[1]][1:10])
        println("data_channel: ", data_channel[channel_names[1]][1:10])
    end
    return TDMSdataConverter(group_names, channel_names, channel_start_times, time_track_channel, data_channel)
end

## 定义一个能够将TDMSdataConverter类转换为CSV文件的函数
function toCSV(converter::TDMSdataConverter, filename::String)
    # 检查converter.channel_names的长度是否大于零
    if length(converter.channel_names) == 0
        error("No channels in the converter.")
    end

    csv_file = open(filename, "w")
    try
        # 写入时间标识行
        write(csv_file, join(["时间标识," * string(fromTimestamp(converter.channel_start_times[i])) for i in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # 写入间隔行
        intervals = [converter.time_track_channel[name][2] - converter.time_track_channel[name][1] for name in converter.channel_names]
        write(csv_file, join(["间隔," * string(intervals[i]) for i in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # 写入通道名称行
        write(csv_file, join(["通道名称,\"" * converter.channel_names[i] * "\"" for i in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # 写入单位行
        write(csv_file, join(["单位,\"V\"" for _ in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # 输出数据
        for i in 1:length(converter.time_track_channel[converter.channel_names[1]])
            write(csv_file, join([string(converter.time_track_channel[converter.channel_names[j]][i]) * "," * string(converter.data_channel[converter.channel_names[j]][i]) for j in 1:length(converter.channel_names)], ","))
            write(csv_file, "\n")
        end
    finally
        close(csv_file)
    end
end

## 测试TDMSdataConverter类
#converter = TDMSdataConverter("/home/shihongfu/temp/FDUdata/tdmsData/记录-2024-04-25 023955 239.tdms")

## 测试toCSV函数
#toCSV(converter, "/home/shihongfu/temp/FDUdata/csvData/记录-2024-04-25 023955 239.csv")

## 遍历指定目录下的所有TDMS文件，并将其转换为CSV文件，保存到指定目录下
function convertTDMS2CSV(tdms_dir::String, csv_dir::String, isDebug::Bool=false)
    tdms_files = filter(x -> endswith(x, ".tdms"), readdir(tdms_dir))
    for tdms_file in tdms_files
        if isDebug
            println("Converting ", tdms_file, " to CSV file.")
        end
        converter = TDMSdataConverter(joinpath(tdms_dir, tdms_file), isDebug)
        toCSV(converter, joinpath(csv_dir, splitext(tdms_file)[1] * ".csv"))
    end
end

## 运行convertTDMS2CSV函数
# 确保有足够的参数传入
if length(ARGS) < 2
    println("Usage: julia TDMSdataConverter.jl <tdmsData path> <csvData path>")
    exit(1)
end

tdmsData_path = ARGS[1]
csvData_path = ARGS[2]

convertTDMS2CSV(tdmsData_path, csvData_path)

## 退出Julia
exit()
