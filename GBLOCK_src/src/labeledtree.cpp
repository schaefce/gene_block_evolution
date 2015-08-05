// labeledtree.cpp is part of GENEBLOCKER.
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

#include "labeledtree.h"

void LabeledTree::addIdsAndLabels(std::map <std::string, std::string> idMap, std::map <std::string, std::vector<std::string>> labelMap, bool prune) {
  if(!idMap.empty()){
    addLeafIds(idMap, prune);
    if(!labelMap.empty()){
      addLeafLabels(labelMap, prune);
    }
  }
}

void LabeledTree::addLeafIds(std::map <std::string, std::string> idMap, bool prune){
  std::vector<LabeledNode*> toPrune;
  if (!idMap.empty()) {
    for (LabeledNode* leaf : collectLeaves()){
      if(idMap.find(leaf->getName()) != idMap.end()){
        leaf->setID(idMap[leaf->getName()]);
      }
      else if(prune){
        toPrune.push_back(leaf);
      }
    }
  }
}

void LabeledTree::addLeafLabels(std::map <std::string, std::vector<std::string>> labelMap, bool prune){
  std::vector<LabeledNode*> toPrune;
  if (!labelMap.empty()) {
    for (LabeledNode* leaf : collectLeaves()){
      if(labelMap.count(leaf->getID())){
        leaf->setLabel(new Label(labelMap[leaf->getID()],0,true));
      }
      else if(prune){
        toPrune.push_back(leaf);
      }
    }
  }
}

void LabeledTree::setLabelsFromChoice(Choice* c){
  if (c){
    Label *l = c->getChoiceGroup()->getLabel();
    if(l){
      l->setFinalChoice(c);
      std::pair<Choice*, Choice*> parents = c->getChoiceGroup()->getParents();
      setLabelsFromChoice(parents.first);
      setLabelsFromChoice(parents.second);
    }
  }
  
}

void LabeledTree::setLabelsFromRoot(){
  Choice* rootChoice = root->getLabel()->getBestChoice();
  setLabelsFromChoice(rootChoice);
}


std::string LabeledTree::newick() {
  std::stringstream ss;
  this->root->newick_helper(ss, true);
  return ss.str() + ";";
}


