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
      std::cout << idMap.count(leaf->getName()) << std::endl;
      if(idMap.find(leaf->getName()) != idMap.end()){
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

void LabeledTree::addLeafLabels(std::map <std::string, std::vector<std::string>> labelMap, bool prune){
  std::vector<LabeledNode*> toPrune;
  if (!labelMap.empty()) {
    for (LabeledNode* leaf : collectLeaves()){
      if(labelMap.count(leaf->getID())){
        leaf->setLabel(new Label(labelMap[leaf->getID()],0,true));// Label::createLeafLabel((std::vector<std::string>){labelMap[leaf->getID()]}));
        //leaf->setID(idMap[leaf->getName()]);
      }
      else if(prune){
        toPrune.push_back(leaf);
      }
    }
    for (LabeledNode* leaf : toPrune){
      this->prune(leaf);
    }
    for (LabeledNode* leaf : collectLeaves()){
      assert(leaf->getLabel());
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
