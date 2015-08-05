// basenode.h is part of GENEBLOCKER.
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

#include <string>
#include <iostream>
#include <algorithm>

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
    setChild(left, true); //this->left = left;
    setChild(right, false); //this->right = right;
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
  
  std::string getName() const {return this->name;}

  double getWeight() const { return this->weight;}
  void setWeight(double weight){ this->weight = weight;}

  TargetNode* getChild(bool left) { return left ? this->left : this->right; }
  bool hasChild(bool left) { return left ? this->left : this->right; }

  bool isLeaf() const { return !left && !right; }

  void setParent(TargetNode* node){
    this->parent = node;
  }

  void addChild(TargetNode* node, bool left){
    if (hasChild(left))
      delete getChild(left);
    setChild(node, left);
  };
  
  void setChild(TargetNode* node, bool left){
    if (left){
      this->left = node;
    }
    else{
      this->right = node;
    }
  }
  
  void removeChild(bool left){
    if(hasChild(left)){
      delete getChild(left);
    }
    setChild((TargetNode*)NULL, left);
  }
  
  void newick_helper(std::stringstream &ss){
    if (isLeaf()){
      ss << name;
      return;
    }
    ss << "(";
    if (hasChild(true))
      ss << getChild(true)->newick_helper() << ":" << std::to_string(getChild(true)->getWeight());
    if (hasChild(true) && hasChild(false))
      ss << ",";
    if (hasChild(false))
      ss << getChild(false)->newick_helper() + ":" + std::to_string(getChild(false)->getWeight());
    ss << ")" << name;
  };

  std::string newick_helper() {
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

 protected:
  TargetNode* left;
  TargetNode* right;
  std::string name;
  double weight;
};

#endif
