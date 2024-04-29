# TDMSdataConverter.jl

# ���ڵ�һ��ʹ��Julia���û�����Ҫ��װPyCallģ�飬��װ�������£�
# 1. ����Pkg.add("PyCall")����װPyCallģ��
# 2. ����ENV["PYTHON"] = "/usr/bin/python3.10"������Python�İ汾��·��
# 3. ����Pkg.build("PyCall") �����°�װPyCallģ��
# 4. ��������Julia������using PyCall������PyCallģ��

## ����PyCallģ��
using PyCall
np = pyimport("numpy")
npTDMS = pyimport("nptdms")

## ����Datesģ��
using Dates

## ������ڱ���ʱ��ı���ʱ���ת������
function fromTimestamp(timestamp::Float64)
    LocalTimeShift = Dates.Hour(8)
    return (Dates.unix2datetime(timestamp) + LocalTimeShift)
end

## ����TDMSdataConverter��
struct TDMSdataConverter
    group_names::Vector{String}
    channel_names::Vector{String}
    channel_start_times::Vector{Float64}
    time_track_channel::Dict{String,Vector{Float64}}
    data_channel::Dict{String,Any}
end

## ����TDMSdataConverter��Ĺ��캯��
function TDMSdataConverter(filename::String, isDebug::Bool=false)
    tdms_file = npTDMS.TdmsFile.read(filename)
    group_names = String[]
    channel_names = String[]
    channel_start_times = Float64[]
    time_track_channel = Dict{String,Vector{Float64}}()
    data_channel = Dict{String,Any}()
    for group in tdms_file.groups()
        group_name = group.name
        ## ���group_name������channels�Ƿ�Ϊ�գ����Ϊ��������
        if isempty(group.channels())
            continue
        end
        ## ��group_names�����group_name
        push!(group_names, group_name)
        for channel in group.channels()
            channel_name = channel.name
            ## ��channel_names�����channel_name
            push!(channel_names, channel_name)
            ## ��channel_start_times�����channel.properties["wf_start_time"]
            push!(channel_start_times, convert(Float64, (channel.properties["wf_start_time"] - np.datetime64("1970-01-01T00:00:00Z")) / np.timedelta64(1, "s")))
            time_track = channel.time_track()
            time_track_channel[channel_name] = time_track
            ## ��data_channel�����channel_name��channel.data
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

## ����һ���ܹ���TDMSdataConverter��ת��ΪCSV�ļ��ĺ���
function toCSV(converter::TDMSdataConverter, filename::String)
    # ���converter.channel_names�ĳ����Ƿ������
    if length(converter.channel_names) == 0
        error("No channels in the converter.")
    end

    csv_file = open(filename, "w")
    try
        # д��ʱ���ʶ��
        write(csv_file, join(["ʱ���ʶ," * string(fromTimestamp(converter.channel_start_times[i])) for i in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # д������
        intervals = [converter.time_track_channel[name][2] - converter.time_track_channel[name][1] for name in converter.channel_names]
        write(csv_file, join(["���," * string(intervals[i]) for i in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # д��ͨ��������
        write(csv_file, join(["ͨ������,\"" * converter.channel_names[i] * "\"" for i in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # д�뵥λ��
        write(csv_file, join(["��λ,\"V\"" for _ in 1:length(converter.channel_names)], ","))
        write(csv_file, "\n")

        # �������
        for i in 1:length(converter.time_track_channel[converter.channel_names[1]])
            write(csv_file, join([string(converter.time_track_channel[converter.channel_names[j]][i]) * "," * string(converter.data_channel[converter.channel_names[j]][i]) for j in 1:length(converter.channel_names)], ","))
            write(csv_file, "\n")
        end
    finally
        close(csv_file)
    end
end

## ����TDMSdataConverter��
converter = TDMSdataConverter("/home/shihongfu/temp/FDUdata/tdmsData/��¼ 1.tdms")

## ����toCSV����
toCSV(converter, "/home/shihongfu/temp/FDUdata/csvData/��¼ 1.csv")
