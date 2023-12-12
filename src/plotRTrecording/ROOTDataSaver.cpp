#include "ROOTDataSaver.hpp"

namespace TTREEIO
{

RootDataSaver::RootDataSaver( const MDFIO::markdownTableDataExtractor & extractor )
    : m_extractor( extractor )
{
    channelNames_string_vec = extractor.getChannelNames();
    pinNumbers_string_vec = extractor.getPinNumbers();
    detectorNames_string_vec = extractor.getDetectorNames();
    detectorComments_string_vec = extractor.getDetectorComments();

    temperature_d_vec = extractor.getTemperature();
    temperature_err_d_vec = extractor.getTemperature_err();

    resistance_ch7_d_vec = extractor.getResistance_ch7();
    resistance_ch8_d_vec = extractor.getResistance_ch8();
    resistance_ch9_d_vec = extractor.getResistance_ch9();
    resistance_ch10_d_vec = extractor.getResistance_ch10();
    resistance_ch11_d_vec = extractor.getResistance_ch11();
    resistance_ch12_d_vec = extractor.getResistance_ch12();
}

void RootDataSaver::saveToRootFile( const std::string & filename )
{
    // 检查General Information向量的大小是否相同
    size_t generalInfo_vec_size = channelNames_string_vec.size();
    if ( pinNumbers_string_vec.size() != generalInfo_vec_size ||
         detectorNames_string_vec.size() != generalInfo_vec_size ||
         detectorComments_string_vec.size() != generalInfo_vec_size )
    {
        // 处理大小不一致的情况
        std::cerr << "Error: General Information Vectors have different sizes." << std::endl;
        return; // 或者其他错误处理
    }

    // 检查Measurement Data向量的大小是否相同
    size_t measurementData_vec_size = temperature_d_vec.size();
    if ( temperature_err_d_vec.size() != measurementData_vec_size ||
         resistance_ch7_d_vec.size() != measurementData_vec_size ||
         resistance_ch8_d_vec.size() != measurementData_vec_size ||
         resistance_ch9_d_vec.size() != measurementData_vec_size ||
         resistance_ch10_d_vec.size() != measurementData_vec_size ||
         resistance_ch11_d_vec.size() != measurementData_vec_size ||
         resistance_ch12_d_vec.size() != measurementData_vec_size )
    {
        // 处理大小不一致的情况
        std::cerr << "Error: Measurement Data Vectors have different sizes." << std::endl;
        return; // 或者其他错误处理
    }

    // 使用std::unique_ptr创建ROOT文件
    auto file = std::make_unique<TFile>( filename.c_str(), "RECREATE" );

    // 使用std::unique_ptr创建第一个TTree
    auto tree1 = std::make_unique<TTree>( "GeneralInfo", "General Information" );

    // 创建分支并单独填充每个元素
    std::string channelName = "", pinNumber = "", detectorName = "", detectorComment = "";

    tree1->Branch( "ChannelName", &channelName );
    tree1->Branch( "PinNumber", &pinNumber );
    tree1->Branch( "DetectorName", &detectorName );
    tree1->Branch( "DetectorComment", &detectorComment );

    for ( size_t i = 0; i < generalInfo_vec_size; ++i )
    {
        // 为每个元素赋值
        channelName = channelNames_string_vec[i];
        pinNumber = pinNumbers_string_vec[i];
        detectorName = detectorNames_string_vec[i];
        detectorComment = detectorComments_string_vec[i];

        // 填充TTree
        tree1->Fill();
    }

    // 使用std::unique_ptr创建第二个TTree
    auto tree2 = std::make_unique<TTree>( "MeasurementData", "Measurement Data" );

    // 创建分支并单独填充每个元素
    double temperature = 0.0, temperatureErr = 0.0;
    double resistanceCh7 = 0.0, resistanceCh8 = 0.0, resistanceCh9 = 0.0, resistanceCh10 = 0.0, resistanceCh11 = 0.0, resistanceCh12 = 0.0;

    tree2->Branch( "Temperature", &temperature );
    tree2->Branch( "TemperatureErr", &temperatureErr );
    tree2->Branch( "ResistanceCh7", &resistanceCh7 );
    tree2->Branch( "ResistanceCh8", &resistanceCh8 );
    tree2->Branch( "ResistanceCh9", &resistanceCh9 );
    tree2->Branch( "ResistanceCh10", &resistanceCh10 );
    tree2->Branch( "ResistanceCh11", &resistanceCh11 );
    tree2->Branch( "ResistanceCh12", &resistanceCh12 );

    for ( size_t i = 0; i < measurementData_vec_size; ++i )
    {
        // 为每个元素赋值
        temperature = temperature_d_vec[i];
        temperatureErr = temperature_err_d_vec[i];
        resistanceCh7 = resistance_ch7_d_vec[i];
        resistanceCh8 = resistance_ch8_d_vec[i];
        resistanceCh9 = resistance_ch9_d_vec[i];
        resistanceCh10 = resistance_ch10_d_vec[i];
        resistanceCh11 = resistance_ch11_d_vec[i];
        resistanceCh12 = resistance_ch12_d_vec[i];

        // 填充TTree
        tree2->Fill();
    }

    // 写入文件并关闭
    file->Write();
    //file->Close();
    /* The line file->Close();; invokes the destructor on the TTrees.
    * The destructor for a TFile first calls the Close() method.
    * The destructors of std::unique_ptr are called in the reverse
    *  order of the variable declarations.
    * The destructors of std::unique_ptr as the TFile actually deletes
    *  the resource meant to be managed by std::unique_ptr as TTree.
    * This results in a double free error, invoked by the destructor
    *  for the std::unique_ptr as some other object has ownership.
    */

    // 不需要手动删除智能指针，它们会在离开作用域时自动释放资源
}

} // namespace TTREEIO
