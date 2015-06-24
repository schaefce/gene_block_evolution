// BaseNode.h: Simple (binary) tree class
// Defined: A BaseNode with zero or more BaseNodes as children.
#include <string>
#include <iostream>
#include "basenode.h"
#include "label.h"

#ifndef LABELED_NODE_H
#define LABELED_NODE_H


class LabeledNode : public BaseNode<LabeledNode> {
 public:

  LabeledNode() : BaseNode<LabeledNode>(){
    this->label = (Label*)NULL;
  };

  LabeledNode(std::string name, double weight=0) : BaseNode<LabeledNode>(name, weight){
    this->label = (Label*)NULL;
  };

  LabeledNode(LabeledNode* left, LabeledNode* right, std::string name, double weight=0) :
    BaseNode<LabeledNode>(left, right, name, weight){
    this->label = (Label*)NULL;
  };
  
  void setWeight(double weight){
    this->weight = weight;
  }

  void setID(std::string id){
    this->identity = id;
  }
  
  std::string getID(){
    return this->identity;
  }
  
  void setName(std::string name){
    BaseNode<LabeledNode>::setName(name);
  }
  
  std::string getName(){
    return BaseNode<LabeledNode>::getName();
  }
  
  void setLabel(Label* label){
    this->label = label;
  }
  
  Label* getLabel(){
    return label;
  }

  ~LabeledNode(){
    delete this->label;
  };
  
  LabeledNode* recursivePrune(LabeledNode* target){
    if (this == target){
      return (LabeledNode*)NULL;
    }
    else{
      this->left = left->recursivePrune(target);
      this->right = right->recursivePrune(target);
      return this;
    }
  }
  


private:
  Label* label;
  std::string identity;
};

#endif
