#ifndef TREELOADER_H
#define TREELOADER_H

#include <TFile.h>
#include <TTree.h>

//void LoadTrees(const char* rootfile);
void LoadTrees(const char* rootfile, TTree*& tree1, TTree*& tree2, TTree*& tree3, TTree*& tree4, TTree*& tree5, TTree*& tree6);

#endif
