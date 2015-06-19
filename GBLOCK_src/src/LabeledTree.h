// Tree.h: Simple (binary) tree class
#include <string>
#include <iostream>
#include <map>
#include "tree.h"
#include "labelednode.h"

#ifndef LABELED_TREE_H
#define LABELED_TREE_H


class LabeledTree: public Tree<LabeledNode> {
 public:
  LabeledTree() : Tree(){ }
  
  LabeledTree(LabeledNode* root) : Tree(root){ }
  
  void addIdsAndLabels(std::map <string, string> idMap, std::map <string, vector<string>> labelMap, bool prune=false);
  
  void setLabelsFromRoot();

  ~LabeledTree();

private:
  LabeledNode* root;
  void addLeafIds(std::map <string, string> idMap, bool prune=false);
  void addLeafLabels(std::map <string, vector<string>> labelMap, bool prune=false);
  void setLabelsFromChoice(Choice* c);
};

#endif








