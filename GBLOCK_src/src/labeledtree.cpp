#include "labeledtree.h"



void LabeledTree::addIdsAndLabels(map <string, string> idMap, map <string, vector<string>> labelMap, bool prune=false) {
  if(idMap){
    addLeafIds(idMap);
    if(labelMap){
      addLeafLabels(labelMap);
    }
  }
}

void LabeledTree::addLeafIds(map <string, string> idMap, bool prune=false){
  vector<LabeledNode*> toPrune;
  if (idMap) {
    for (LabeledNode* leaf : collectLeaves()){
      if(idMap.count(leaf->getName())){
        leaf->setID(idMap[leaf->getName()]);
      }
      else if(prune){
        toPrune.push_back(leaf);
      }
    }
    for (LabeledNode* leaf : toPrune){
      prune(leaf);
    }
  }
}

void LabeledTree::addLeafLabels(map <string, vector<string>> labelMap, bool prune=false){
  vector<LabeledNode*> toPrune;
  if (labelMap) {
    for (LabeledNode* leaf : collectLeaves()){
      if(labelMap.count(leaf->getID())){
        leaf->setLabel(Label.createLeafLabel(labelMap[leaf->getID()]));
        //leaf->setID(idMap[leaf->getName()]);
      }
      else if(prune){
        toPrune.push_back(leaf);
      }
    }
    for (LabeledNode* leaf : toPrune){
      prune(leaf);
    }
  }
}
