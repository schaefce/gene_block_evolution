// labelednode.h is part of GENEBLOCKER.
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

#include "basenode.h"
#include "label.h"

#ifndef LABELED_NODE_H
#define LABELED_NODE_H


// BaseNode + labels
class LabeledNode : public BaseNode<LabeledNode> {
public:
  
  LabeledNode() : BaseNode<LabeledNode>(){ this->label = (Label*)NULL; };
  
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
  
  ~LabeledNode(){
    if(this->label){
      delete this->label;
    }
  };
  
  void setWeight(double weight){ this->weight = weight; }
  
  std::string getID() const { return this->identity; }
  void setID(std::string id){ this->identity = id;   }
  bool hasID() const { return !this->identity.empty(); }
  
  std::string getName()  { return BaseNode<LabeledNode>::getName(); }
  void setName(std::string name){ BaseNode<LabeledNode>::setName(name); }
  
  Label* getLabel() { return label; }
  void setLabel(Label* label){ this->label = label; }
  
  LabeledNode* getParent() { return this->parent; }
  void setParent(LabeledNode* node){ this->parent = node; }
  bool hasParent() const   { return this->parent; }
  
  void setChild(LabeledNode* node, bool left){
    BaseNode<LabeledNode>::setChild(node, left);
    node->setParent(this);
  }
  
  std::string get_info_string() const{ return ":" + std::to_string(weight); }
  
  std::string newick_helper(bool withLabels=true) {
    if (!withLabels){
      return BaseNode::newick_helper();
    }
    else{
      std::string joiner = (!name.empty() and label)? "|" : "";
      std::string fLabel = label ? label->formatted() : "";
      if (isLeaf()){
        return name + joiner + fLabel;
      }
      std::string s = "(";
      if (hasChild(true))
        s += getChild(true)->newick_helper() + getChild(true)->get_info_string();
      if (hasChild(true) && hasChild(false))
        s += ",";
      if (hasChild(false))
        s += getChild(false)->newick_helper() + getChild(false)->get_info_string();
      return s + ")" + name + joiner + fLabel;
    }
  };
  
  void newick_helper(std::stringstream &ss, bool withLabels=true){
    if (!withLabels){
      BaseNode::newick_helper(ss);
    }
    else{
      std::string joiner = (!name.empty() and label)? "|" : "";
      std::string fLabel = label ? label->formatted() : "";
      if (isLeaf()){
        ss << name << joiner << fLabel;
        return;
      }
      ss << "(";
      if (hasChild(true))
        ss << getChild(true)->newick_helper() << getChild(true)->get_info_string();
      if (hasChild(true) && hasChild(false))
        ss << ",";
      if (hasChild(false))
        ss << getChild(false)->newick_helper() << getChild(true)->get_info_string();
      ss << ")" << name << joiner << fLabel;
    }
  };
  
private:
  Label* label;
  LabeledNode* parent;
  std::string identity;
};

#endif
