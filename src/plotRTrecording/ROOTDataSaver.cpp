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
    // 使用std::unique_ptr创建ROOT文件
    auto file = std::make_unique<TFile>( filename.c_str(), "RECREATE" );

    // 使用std::unique_ptr创建第一个TTree
    auto tree1 = std::make_unique<TTree>( "GeneralInfo", "General Information" );
    // 创建分支
    tree1->Branch( "ChannelName", &channelNames_string_vec );
    tree1->Branch( "PinNumber", &pinNumbers_string_vec );
    tree1->Branch( "DetectorName", &detectorNames_string_vec );
    tree1->Branch( "DetectorComment", &detectorComments_string_vec );
    tree1->Fill(); // 填充TTree

    // 使用std::unique_ptr创建第二个TTree
    auto tree2 = std::make_unique<TTree>( "MeasurementData", "Measurement Data" );
    // 创建分支
    tree2->Branch( "Temperature", &temperature_d_vec );
    tree2->Branch( "TemperatureErr", &temperature_err_d_vec );
    tree2->Branch( "ResistanceCh7", &resistance_ch7_d_vec );
    tree2->Branch( "ResistanceCh8", &resistance_ch8_d_vec );
    tree2->Branch( "ResistanceCh9", &resistance_ch9_d_vec );
    tree2->Branch( "ResistanceCh10", &resistance_ch10_d_vec );
    tree2->Branch( "ResistanceCh11", &resistance_ch11_d_vec );
    tree2->Branch( "ResistanceCh12", &resistance_ch12_d_vec );
    tree2->Fill(); // 填充TTree

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
