// BaseNode.h: Simple (binary) tree class
// Defined: A BaseNode with zero or more BaseNodes as children.
#include <string>
#include <iostream>
#include "basenode.h"
#include "label.h"
using namespace std;

#ifndef LABELED_NODE_H
#define LABELED_NODE_H

using namespace std;

class LabeledNode : public BaseNode<LabeledNode> {
 public:

  LabeledNode() : BaseNode<LabeledNode>(){
    this->label = (Label*)NULL;
  };

  LabeledNode(string name, double weight=0) : BaseNode<LabeledNode>(name, weight){
    this->label = (Label*)NULL;
  };

  LabeledNode(LabeledNode* left, LabeledNode* right, string name, double weight=0) :
    BaseNode<LabeledNode>(left, right, string, weight){
    this->label = (Label*)NULL;
  };

  ~LabeledNode(){
    delete this->label;
  };

private:
  Label* label;
};

#endif
