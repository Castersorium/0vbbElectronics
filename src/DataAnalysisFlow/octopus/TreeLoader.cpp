#include "TreeLoader.h"

//void LoadTrees(const char* rootfile) {
void LoadTrees(const char* rootfile, TTree*& tree1, TTree*& tree2, TTree*& tree3, TTree*& tree4, TTree*& tree5, TTree*& tree6) {

    // 打开 ROOT 文件
    TFile *file = new TFile(rootfile, "READ");

    // 获取各个树
    tree1 = (TTree*)file->Get("baseline");
    tree2 = (TTree*)file->Get("baselineslope");
    tree3 = (TTree*)file->Get("maxminusbaseline");
    tree4 = (TTree*)file->Get("risetime");
    tree5 = (TTree*)file->Get("decaytime");
    tree6 = (TTree*)file->Get("timestamp");


    // 定义变量，用于存储树中的数据
    double RMS, baseline;
    double offset, slope;
    double amplitude, maxtime;
    double risetime, decaytime;
    double starttime, stoptime;
    double starttime1, starttime2;
    double stoptime1, stoptime2;
    double timestamp;

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
    tree6->SetBranchAddress("timestamp", &timestamp);

    // 将所有树作为 tree1 的朋友树
    tree1->AddFriend(tree2);
    tree1->AddFriend(tree3);
    tree1->AddFriend(tree4, "friend4");
    tree1->AddFriend(tree5, "friend5");
    tree1->AddFriend(tree6);

    // 设置别名
    tree1->SetAlias("starttime1", "friend4.starttime");
    tree1->SetAlias("stoptime1", "friend4.stoptime");
    tree1->SetAlias("starttime2", "friend5.starttime");
    tree1->SetAlias("stoptime2", "friend5.stoptime");
}