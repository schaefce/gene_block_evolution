// Tree.h: Simple (binary) tree class
#include "tree.h"
#include "labelednode.h"
#include "label.h"

#ifndef LABELED_TREE_H
#define LABELED_TREE_H


// Tree with labeled nodes
class LabeledTree: public Tree<LabeledNode> {
 public:
  LabeledTree() : Tree(){ }
  LabeledTree(LabeledNode* root) : Tree(root){ }
  ~LabeledTree();
  
  void addIdsAndLabels(std::map <std::string, std::string> idMap, std::map <std::string, std::vector<std::string>> labelMap, bool prune=true);
  
  void setLabelsFromRoot();

  std::string newick();

  
private:
  void addLeafIds(std::map <std::string, std::string> idMap, bool prune=true);
  void addLeafLabels(std::map <std::string, std::vector<std::string>> labelMap, bool prune=true);
  void setLabelsFromChoice(Choice* c);
};

#endif








