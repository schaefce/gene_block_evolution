// BaseNode.h: Simple (binary) tree class
// Defined: A BaseNode with zero or more BaseNodes as children.
#include <string>
#include <iostream>

#ifndef BASE_NODE_H
#define BASE_NODE_H


template <typename TargetNode>
class BaseNode {
 public:
  BaseNode(){
    this->left = (TargetNode*)NULL;
    this->right = (TargetNode*)NULL;
    setName("");
  };

  BaseNode(std::string name, double weight=0){
    this->left = (TargetNode*)NULL;
    this->right = (TargetNode*)NULL;
    setName(name);
    this->weight = weight;
  };

  BaseNode(TargetNode* left, TargetNode* right, std::string name, double weight=0){
    this->left = left;
    this->right = right;
    setName(name);
    this->weight = weight;
  };


  ~BaseNode(){
    if (hasChild(true))
      delete getChild(true);
    if (hasChild(false))
      delete getChild(false);
  };

  void setName(std::string name){
    name.erase(
            remove( name.begin(), name.end(), '\'' ),
            name.end()
            );
    this->name = name;
  }

  std::string getName() {return this->name;}

  void setWeight(double weight){ this->weight = weight;}

  double getWeight() { return this->weight;}

  bool hasChild(bool left) { return left ? this->left : this->right; }

  TargetNode* getChild(bool left) { return left ? this->left : this->right; }

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



  void prune(TargetNode* target){
    target->recursivePrune(target);
  }

  std::string newick_helper(){
    if (isLeaf())
        return name;
    std::string s = "(";
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
  std::string name;
  double weight;
};

#endif
