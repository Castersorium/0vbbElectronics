#include <iostream> // C++ header

// ROOT header

#include "ROOTDataPlotter.hpp" // my header

//#define DEBUGGING
//#define DEBUGGINGVERBOSE

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
    if ( !tree )
    {
        std::cerr << "Error getting TTree from file: " << filePath << std::endl;
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

    // 创建TGraphErrors
    auto graph = std::make_unique<TGraphErrors>( tree->GetEntries() );
    int validPointIndex = 0;
    for ( int i = 0; i < tree->GetEntries(); ++i )
    {
        tree->GetEntry( i );
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

    graph->SetMarkerColor( 4 );
    graph->SetMarkerStyle( 21 );

    // 将图形添加到向量中
    graphs.push_back( std::move( graph ) );
}

void ROOTDataPlotter::plotAllGraphs( const std::string & filename )
{
    canvas->cd();

    // 创建一个图例
    auto legend = std::make_unique<TLegend>( 0.7, 0.7, 0.9, 0.9 ); // 调整图例位置和大小

    // 定义一组颜色用于不同的图形
    std::vector<int> colors = { kRed, kBlue, kGreen, kBlack, kMagenta, kCyan, kYellow, kOrange };

    // 将所有图形添加到TMultiGraph中，并设置颜色和图例
    for ( size_t i = 0; i < graphs.size(); ++i )
    {
        graphs[i]->SetLineColor( colors[i % colors.size()] ); // 设置颜色
        graphs[i]->SetMarkerColor( colors[i % colors.size()] ); // 设置标记颜色
        multiGraph->Add( graphs[i].get(), "AP" );

        // 添加图例条目
        legend->AddEntry( graphs[i].get(), ( "Graph " + std::to_string( i + 1 ) ).c_str(), "lep" );
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

} // namespace TTREEIO

