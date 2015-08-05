// labeledtree.h is part of GENEBLOCKER.
//
// GENEBLOCKER is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GENEBLOCKER is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GENEBLOCKER.  If not, see <http://www.gnu.org/licenses/>.

// Created by Carly Schaeffer and John Karro

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








