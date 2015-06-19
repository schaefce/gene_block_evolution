#include "labeledtree.h"



void LabeledTree::addIdsAndLabels(map <string, string> idMap, map <string, vector<string>> labelMap, bool prune) {
  if(!idMap.empty()){
    addLeafIds(idMap, prune);
    if(!labelMap.empty()){
      addLeafLabels(labelMap, prune);
    }
  }
}

void LabeledTree::addLeafIds(map <string, string> idMap, bool prune){
  vector<LabeledNode*> toPrune;
  if (!idMap.empty()) {
    for (LabeledNode* leaf : collectLeaves()){
      if(idMap.count(leaf->getName())){
        leaf->setID(idMap[leaf->getName()]);
      }
      else if(prune){
        toPrune.push_back(leaf);
      }
    }
    for (LabeledNode* leaf : toPrune){
      this->prune(leaf);
    }
  }
}

void LabeledTree::addLeafLabels(map <string, vector<string>> labelMap, bool prune){
  vector<LabeledNode*> toPrune;
  if (!labelMap.empty()) {
    for (LabeledNode* leaf : collectLeaves()){
      if(labelMap.count(leaf->getID())){
        leaf->setLabel(Label::createLeafLabel(labelMap[leaf->getID()]));
        //leaf->setID(idMap[leaf->getName()]);
      }
      else if(prune){
        toPrune.push_back(leaf);
      }
    }
    for (LabeledNode* leaf : toPrune){
      this->prune(leaf);
    }
  }
}

void LabeledTree::setLabelsFromChoice(Choice* c){
  if (c){
    Label *l = c->getChoiceGroup()->getLabel();
    if(l){
      l->setFinalChoice(c);
      std::pair<Choice*, Choice*> children = c->getChoiceGroup()->getChildren();
      setLabelsFromChoice(children.first);
      setLabelsFromChoice(children.second);
    }
  }
  
}

void LabeledTree::setLabelsFromRoot(){
  Choice* rootChoice = root->getLabel()->getBestChoice();
  setLabelsFromChoice(rootChoice);
}
