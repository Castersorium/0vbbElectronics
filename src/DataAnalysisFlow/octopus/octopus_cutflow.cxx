#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "TFile.h"
#include "TTree.h"

void analyzeCumulativeCuts(const char *rootfile, const std::vector<std::string> &cuts) {

	// 打开 ROOT 文件
	TFile *file = new TFile(rootfile, "READ");
	// 获取各个树
	TTree *tree1 = (TTree*)file->Get("baseline");
	TTree *tree2 = (TTree*)file->Get("baselineslope");
	TTree *tree3 = (TTree*)file->Get("maxminusbaseline");
	TTree *tree4 = (TTree*)file->Get("risetime");
	TTree *tree5 = (TTree*)file->Get("decaytime");

	// 定义变量，用于存储树中的数据
	double RMS, baseline;
	double offset, slope;
	double amplitude, maxtime;
	double risetime, decaytime;
	double starttime,stoptime;
	double starttime1,starttime2;
	double stoptime1,stoptime2;

	// 设置树的分支地址
	tree1->SetBranchAddress("RMS", &RMS);
	tree1->SetBranchAddress("baseline", &baseline);
	tree2->SetBranchAddress("offset", &offset);
	tree2->SetBranchAddress("slope", &slope);
	tree3->SetBranchAddress("amplitude", &amplitude);
	tree3->SetBranchAddress("maxtime", &maxtime);
	tree4->SetBranchAddress("risetime", &risetime);
	tree4->SetBranchAddress("starttime", &starttime1);
	tree4->SetBranchAddress("stoptime", &stoptime1);
	tree5->SetBranchAddress("decaytime", &decaytime);
	tree5->SetBranchAddress("starttime", &starttime2);
	tree5->SetBranchAddress("stoptime", &stoptime2);

	// 将所有树作为 tree1 的朋友树
	tree1->AddFriend(tree2);
	tree1->AddFriend(tree3);
	tree1->AddFriend(tree4,"friend4");
	tree1->AddFriend(tree5,"friend5");

	tree1->SetAlias("starttime1","friend4.starttime");
	tree1->SetAlias("stoptime1","friend4.stoptime");
	tree1->SetAlias("starttime2","friend5.starttime");
	tree1->SetAlias("stoptime2","friend5.stoptime");

    // 用于存储每个累积 cut 对应的事件数
    std::vector<Long64_t> counts;
    // 累积条件初始化为空
    std::string cumulativeCut = "";

    // 遍历每个 cut
    for (const auto &cut : cuts) {
        // 如果不是第一个条件，用 "&&" 连接
        if (!cumulativeCut.empty()) {
            cumulativeCut += " && ";
        }
        cumulativeCut += cut; // 累积条件叠加

        // 获取满足累积条件的事件数
        Long64_t count = tree1->GetEntries(cumulativeCut.c_str());
        counts.push_back(count);
    }

    // 输出 cutflow 表
    std::cout << rootfile <<endl;
    std::cout << std::setw(40) << "Cumulative Cut" 
            << std::setw(20) << "Events"
            <<std::setw(20) << "Eff Loss(%)" << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(40) << cuts[0] << std::setw(20) << counts[0] << std::setw(20) << 1.*100 << std::endl;
    for (size_t i = 1; i < cuts.size(); ++i) {
        double eff=(1.-1.*counts[i]/counts[i-1])*100;
        std::cout << std::setw(40) << cuts[i] 
                << std::setw(20) << counts[i] 
                << std::setw(20) << std::fixed<< std::setprecision(2)<<eff
                << std::endl;
    }
    std::cout << "---------------------------------------------------------------------------------" << std::endl;

    // 关闭文件
    file->Close();
}

void octopus_cutflow(){

    const char *file1="../rootfile/Processed_20240824T130500_000013_1.root";
	//const char *file2="../rootfile/Processed_20241207T161500_000014_1.root";
    const char *file2="../rootfile/Processed_20241208T213600_000001_1.root";


    // 定义切割条件
    std::vector<std::string> cuts1 = {
        "1",
        "decaytime > 0.004 && decaytime < 0.06",
        "risetime > 0.002 && risetime < 0.04",
        "starttime1 > 0.03 && starttime1 < 0.1",
        "stoptime1 > 0.03 && stoptime1 < 0.1",
        "starttime2 > 0.03 && starttime2 < 0.1",
        "stoptime2 > 0.03 && stoptime2 < 0.1",
        //"starttime1 < stoptime1 && starttime2 < stoptime2"
    };

    std::vector<std::string> cuts2 = {
        "1",
        "decaytime > 0.005 && decaytime < 0.06",
        "risetime > 0.002 && risetime < 0.03",
        "starttime1 > 0.03 && starttime1 < 0.1",
        "stoptime1 > 0.03 && stoptime1 < 0.1",
        "starttime2 > 0.03 && starttime2 < 0.1",
        "stoptime2 > 0.03 && stoptime2 < 0.1",
        //"starttime1 < stoptime1 && starttime2 < stoptime2"
    };

    analyzeCumulativeCuts(file1,cuts1);
    analyzeCumulativeCuts(file2,cuts1);    
    //analyzeCumulativeCuts(file2,cuts2);

}
