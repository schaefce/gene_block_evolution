// Node.h: Simple (binary) tree class
// Defined: A node with zero or more nodes as children.
#include <string>
#include <iostream>

#ifndef NODE_H
#define NODE_H

class Node {
 public:
  Node();
  Node(std::string name);
  Node(std::pair<Node*,double> left, std::pair<Node*,double> right, std::string name);

  ~Node();

  bool hasChild(bool left) {
    return left ? this->left.first : this->right.first;
  }

  Node* getChild(bool left) {
    return left ? this->left.first : this->right.first;
  }

  double getWeight(bool left) {
    return left ? this->left.second : this->right.second;
  }

  void addChild(Node* t, double weight, bool left);

  std::string getName() {return name;}

  bool isLeaf() {return !left.first && !right.first;}
  
  int num_leaves();

 protected:
  std::pair<Node*,double> left;
  std::pair<Node*,double> right;
  std::string name;

  std::string newick_helper();
};

#endif
