// BaseNode.h: Simple (binary) tree class
// Defined: A BaseNode with zero or more BaseNodes as children.
#include <string>
#include <iostream>
using namespace std;

#ifndef BASE_NODE_H
#define BASE_NODE_H

using namespace std;

template <typename TargetNode>
class BaseNode {
 public:
  BaseNode(){
    this->left = (TargetNode*)NULL;
    this->right = (TargetNode*)NULL;
    this->name = "";
  };

  BaseNode(string name, double weight=0){
    this->left = (TargetNode*)NULL;
    this->right = (TargetNode*)NULL;
    this->name = name;
    this->weight = weight;
  };

  BaseNode(TargetNode* left, TargetNode* right, string name, double weight=0){
    this->left = left;
    this->right = right;
    this->name = name;
    this->weight = weight;
  };


  ~BaseNode(){
    if (hasChild(true))
      delete getChild(true);
    if (hasChild(false))
      delete getChild(false);
  };

  //void setName(string name){ this->name = name};

  string getName() {return this->name;}

  void setWeight(double weight){ this->weight = weight;}

  double getWeight() { return this->weight;}

  bool hasChild(bool left) { return left ? this->left.first : this->right.first; }

  TargetNode* getChild(bool left) { return left ? this->left.first : this->right.first; }

  bool isLeaf() { return !left && !right; }

  void setParent(TargetNode* node){
    this->parent = node;
  }

  void addChild(TargetNode* node, bool left){
    if (hasChild(left))
      delete getChild(left);
    if (left)
      this->left = node;
    else
      this->right = node;
  };

  TargetNode* recursivePrune(TargetNode* target){
    if (this == target){
      return (TargetNode*)NULL;
    }
    else{
      this->left = left->recursivePrune(target);
      this->right = right->recursivePrune(target);
      return this;
    }
  }

  void prune(TargetNode* target){
    recursivePrune(target);
  }

  string newick_helper(){
    if (isLeaf())
        return name;
    string s = "(";
    if (hasChild(true))
      s += getChild(true)->newick_helper() + ":" + std::to_string(getChild(true)->getWeight());
    if (hasChild(true) && hasChild(false))
      s += ",";
    if (hasChild(false))
      s += getChild(false)->newick_helper() + ":" + std::to_string(getChild(false)->getWeight());
    return s + ")" + name;
  };

  //int num_leaves();

 protected:
  TargetNode* left;
  TargetNode* right;
  string name;
  double weight;
};

#endif
