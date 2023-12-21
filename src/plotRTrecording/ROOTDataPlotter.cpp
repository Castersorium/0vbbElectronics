#include <iostream> // C++ header

// ROOT header

#include "ROOTDataPlotter.hpp" // my header

//#define DEBUGGING
#define DEBUGGINGVERBOSE

namespace TTREEIO
{

ROOTDataPlotter::ROOTDataPlotter( const std::string & filePath )
    : filePath( filePath ), canvas( std::make_unique<TCanvas>( "RTcanvas", "R-T Plot Canvas", 800, 600 ) ),
    multiGraph( std::make_unique<TMultiGraph>() )
{
    // 打开ROOT文件
    file = std::make_unique<TFile>( filePath.c_str(), "READ" );
    if ( !file || file->IsZombie() )
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }
    // 获取TTree
    tree.reset( dynamic_cast<TTree *>( file->Get( "MeasurementData" ) ) );
    generalInfoTree.reset( dynamic_cast<TTree *>( file->Get( "GeneralInfo" ) ) ); // 获取GeneralInfo树
    if ( !tree || !generalInfoTree )
    {
        std::cerr << "Error getting TTrees from file: " << filePath << std::endl;
        return;
    }

    canvas->SetGrid();
}

void ROOTDataPlotter::plotData()
{
    // 绘制每个通道的图形
    createGraph( "ResistanceCh7" );
    createGraph( "ResistanceCh8" );
    createGraph( "ResistanceCh9" );
    createGraph( "ResistanceCh10" );
    createGraph( "ResistanceCh11" );
    createGraph( "ResistanceCh12" );
}

void ROOTDataPlotter::createGraph( const char * branchName )
{
    // 读取分支数据
    double temperature = 0.0, temperatureErr = 0.0, resistance = 0.0;
    tree->SetBranchAddress( "Temperature", &temperature );
    tree->SetBranchAddress( "TemperatureErr", &temperatureErr );
    tree->SetBranchAddress( branchName, &resistance );

    // 创建TGraphErrors，不指定点数
    auto graph = std::make_unique<TGraphErrors>();
    int validPointIndex = 0;
    for ( int i = 0; i < tree->GetEntries(); ++i )
    {
        tree->GetEntry( i );

#ifdef DEBUGGINGVERBOSE
        std::cout << "GetEntry #" << i << "\t (" << temperature << ", " << resistance << ") from branch \"" << branchName << "\"." << std::endl;
#endif // DEBUGGINGVERBOSE

        // 检查数据点是否包含NaN值
        if ( std::isnan( temperature ) || std::isnan( temperatureErr ) || std::isnan( resistance ) )
        {
            std::clog << "Skipping NaN value at entry " << i << " in branch \"" << branchName << "\"..." << std::endl;
            continue; // 跳过包含NaN的数据点
        }
        graph->SetPoint( validPointIndex, temperature, resistance );

#ifdef DEBUGGINGVERBOSE
        std::cout << "Set #" << validPointIndex << "\tPoint (" << temperature << ", " << resistance << ") to graph of \"" << branchName << "\"." << std::endl;
#endif // DEBUGGINGVERBOSE

        graph->SetPointError( validPointIndex, temperatureErr, 0.01 * resistance ); // 假设电阻误差为1%
        validPointIndex++;
    }

    // 添加图例条目
    std::string detectorComment = getDetectorComment( branchName ); // 获取探测器备注

#ifdef DEBUGGING
    std::cout << "For branch \"" << branchName << "\", got Detector Comment: " << detectorComment << std::endl;
#endif // DEBUGGING

    // 将图例条目添加到向量中
    detectorComments.push_back( detectorComment );

    // 将图形添加到向量中
    graphs.push_back( std::move( graph ) );
}

void ROOTDataPlotter::plotAllGraphs( const std::string & filename )
{
    canvas->cd();

    // 创建一个图例
    auto legend = std::make_unique<TLegend>( 0.5, 0.7, 0.9, 0.9 ); // 调整图例位置和大小

    // 定义一组颜色和标记样式用于不同的图形
    std::vector<int> colors = { kRed, kBlue, kGreen, kBlack, kMagenta, kCyan, kYellow, kOrange };
    std::vector<int> markerStyles = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 }; // 预定义的标记样式

    // 将所有图形添加到TMultiGraph中，并设置颜色、标记样式和图例
    for ( size_t i = 0; i < graphs.size(); ++i )
    {
        graphs[i]->SetLineColor( colors[i % colors.size()] ); // 设置颜色
        graphs[i]->SetMarkerColor( colors[i % colors.size()] ); // 设置标记颜色
        graphs[i]->SetMarkerStyle( markerStyles[i % markerStyles.size()] ); // 设置标记样式
        multiGraph->Add( graphs[i].get(), "AP" );

        // 添加图例条目
        legend->AddEntry( graphs[i].get(), ( detectorComments[i] ).c_str(), "lep" );
    }

    // 绘制所有图形
    multiGraph->Draw( "AP" );

    // 设置坐标轴名称和标题
    multiGraph->SetTitle( "Measurement Data;MXC Temperature [K];Resistance [#Omega]" ); // 格式: "标题;X轴名称;Y轴名称"

    // 绘制图例
    legend->Draw();

    // 更新canvas内容
    canvas->Update();

    // 初始化outputFile并保存图形到ROOT文件
    outputFile = std::make_unique<TFile>( filename.c_str(), "RECREATE" );
    canvas->Write();

    graphs.clear(); // 清空向量并销毁所有图形

    //outputFile->Close();
}

std::string ROOTDataPlotter::getDetectorComment( const std::string & channelName )
{
    // 将channelName转换为ChannelName分支中的格式
    std::string channelNameInBranch = "CH" + channelName.substr( channelName.find( "Ch" ) + 2 );

    // 在GeneralInfo树中查找对应的探测器备注
    std::string * channelNameInTree_str_ptr = new std::string( "" );
    std::string * detectorComment_str_ptr = new std::string( "" );
    generalInfoTree->SetBranchAddress( "ChannelName", &channelNameInTree_str_ptr );
    generalInfoTree->SetBranchAddress( "DetectorComment", &detectorComment_str_ptr );
    for ( int i = 0; i < generalInfoTree->GetEntries(); ++i )
    {
        generalInfoTree->GetEntry( i );
        if ( channelNameInBranch == *channelNameInTree_str_ptr )
        {
            std::string detectorComment = *detectorComment_str_ptr;
            delete channelNameInTree_str_ptr;
            delete detectorComment_str_ptr;
            return detectorComment;
        }
    }

    delete channelNameInTree_str_ptr;
    delete detectorComment_str_ptr;
    return "Undefined detector"; // 如果没有找到对应的备注，返回未定义探测器
}

} // namespace TTREEIO

