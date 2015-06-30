// BaseNode.h: Simple (binary) tree class
// Defined: A BaseNode with zero or more BaseNodes as children.
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
      if(left){
        left->setParent(this);
      }
      if(right){
        right->setParent(this);
      }

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
  
  bool hasID(){
    return !this->identity.empty();
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
    if(this->label){
      delete this->label;
    }
  };
  
  void setParent(LabeledNode* node){ this->parent = node; }
  
  LabeledNode* getParent() { return this->parent; }
  
  bool hasParent() { return this->parent; }
  
  
  void setChild(LabeledNode* node, bool left){
    BaseNode<LabeledNode>::setChild(node, left);
    node->setParent(this);
  }
  
  std::string get_info_string() const{
    return ":" + std::to_string(weight);
  }
  
  
  std::string newick_helper(bool withLabels=true) {
    if (!withLabels){
      return BaseNode::newick_helper();
    }
    else{
      std::string fLabel = label ? "|" + label->formatted() : "";
      if (isLeaf()){
        return name + fLabel;
      }
      std::string s = "(";
      if (hasChild(true))
        s += getChild(true)->newick_helper() + getChild(true)->get_info_string();//":" + std::to_string(getChild(true)->getWeight());
      if (hasChild(true) && hasChild(false))
        s += ",";
      if (hasChild(false))
        s += getChild(false)->newick_helper() + getChild(false)->get_info_string();//":" + std::to_string(getChild(false)->getWeight());
      return s + ")" + name + fLabel;
    }
  };
  

  
  void newick_helper(std::stringstream &ss, bool withLabels=true){
    if (!withLabels){
      BaseNode::newick_helper(ss);
    }
    else{
      std::string fLabel = label ? "|" + label->formatted() : "";
      if (isLeaf()){
        ss << name << fLabel;
        return;
      }
      ss << "(";
      if (hasChild(true))
        ss << getChild(true)->newick_helper() << getChild(true)->get_info_string();// ":" << std::to_string(getChild(true)->getWeight());
      if (hasChild(true) && hasChild(false))
        ss << ",";
      if (hasChild(false))
        ss << getChild(false)->newick_helper() << getChild(true)->get_info_string();//+ ":" + std::to_string(getChild(false)->getWeight());
      ss << ")" << name << fLabel;
    }
  };
  
  
  
  
  //LabeledNode* prune(LabeledNode* target){
  //  if (this->left){
  //    if(this->left== target){
  //      delete getChild(true);
  //      this->left = (LabeledNode*)NULL;
  //    }
  //    else{
  //      this->left = left->prune(target);
  //    }
  //  }
  //  if (this->right){
  //    if(this->right == target){
  //      delete getChild(false);
  //      this->right = (LabeledNode*)NULL;
  //    }
  //    else{
  //      this->right = right->prune(target);
  //    }
  //  }
  //  return this;
  //}
  


private:
  Label* label;
  LabeledNode* parent;
  std::string identity;
};

#endif
