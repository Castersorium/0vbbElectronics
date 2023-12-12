#include <iostream> // C++ header

// ROOT header

#include "ROOTDataPlotter.hpp" // my header

//#define DEBUGGING
#define DEBUGGINGVERBOSE

namespace TTREEIO
{

ROOTDataPlotter::ROOTDataPlotter( const std::string & filePath )
    : filePath( filePath ), canvas( std::make_unique<TCanvas>( "canvas", "Plot Canvas", 800, 600 ) ),
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

ROOTDataPlotter::~ROOTDataPlotter()
{
    // 不需要手动释放资源，智能指针会自动管理
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
    for ( int i = 0; i < tree->GetEntries(); ++i )
    {
        tree->GetEntry( i );
        graph->SetPoint( i, temperature, resistance );
#ifdef DEBUGGINGVERBOSE
        std::cout << "Set #" << i << "\tPoint (" << temperature << ", " << resistance << ") to graph of \"" << branchName << "\"." << std::endl;
#endif // DEBUGGINGVERBOSE

        graph->SetPointError( i, temperatureErr, 0.01 * resistance ); // 假设电阻误差为1%
    }

    graph->SetMarkerColor( 4 );
    graph->SetMarkerStyle( 21 );

    // 将图形添加到向量中
    graphs.push_back( std::move( graph ) );
}

void ROOTDataPlotter::plotAllGraphs( const std::string & filename )
{
    // 将所有图形添加到TMultiGraph中
    for ( auto & graph : graphs )
    {
        multiGraph->Add( graph.get(), "AP" );
    }

    // 绘制所有图形
    canvas->cd();
    multiGraph->Draw( "AP" );
    canvas->Update(); // 更新canvas内容

    // 保存图形到ROOT文件
    auto outputFile = std::make_unique<TFile>( filename.c_str(), "RECREATE" );
    canvas->Write();

    //outputFile->Close();
}

} // namespace TTREEIO
