#pragma once

#ifndef ROOTDATAPLOTTER_HPP
#define ROOTDATAPLOTTER_HPP

#include <memory> // C++ header
#include <string>
#include <vector>

#include <TFile.h> // ROOT header
#include <TTree.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TLegend.h>

namespace TTREEIO
{

class ROOTDataPlotter
{
public:
    ROOTDataPlotter( const std::string & filePath );
    void plotData();
    void plotAllGraphs( const std::string & filename );

private:
    std::string filePath;
    std::unique_ptr<TFile> file;
    std::unique_ptr<TTree> tree;
    std::vector< std::unique_ptr<TGraphErrors> > graphs; // 存储所有图形
    std::unique_ptr<TFile> outputFile; // 用于保存输出文件
    std::unique_ptr<TCanvas> canvas; // 用于绘制图形
    std::unique_ptr<TMultiGraph> multiGraph; // 用于组合多个图形
    void createGraph( const char * branchName );
};

} // namespace TTREEIO

#endif // ROOTDATAPLOTTER_HPP

