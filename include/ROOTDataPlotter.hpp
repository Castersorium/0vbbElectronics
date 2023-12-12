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
    std::vector< std::unique_ptr<TGraphErrors> > graphs; // �洢����ͼ��
    std::unique_ptr<TFile> outputFile; // ���ڱ�������ļ�
    std::unique_ptr<TCanvas> canvas; // ���ڻ���ͼ��
    std::unique_ptr<TMultiGraph> multiGraph; // ������϶��ͼ��
    void createGraph( const char * branchName );
};

} // namespace TTREEIO

#endif // ROOTDATAPLOTTER_HPP
