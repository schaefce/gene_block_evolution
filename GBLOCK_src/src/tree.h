// Tree.h: Simple (binary) tree class
// Defined: A treee is a node with zero or more trees as children.
#include <string>
#include <iostream>

#ifndef KARRO_TREE_H
#define KARRO_TREE_H

class Tree {
 public:
  Tree();
  Tree(std::string name);
  Tree(std::pair<Tree*,double> left, std::pair<Tree*,double> right, std::string name);

  ~Tree();

  bool hasChild(bool left) {
    return left ? this->left.first : this->right.first;
  }

  Tree* getChild(bool left) {
    return left ? this->left.first : this->right.first;
  }

  double getWeight(bool left) {
    return left ? this->left.second : this->right.second;
  }

  void addChild(Tree* t, double weight, bool left);

  std::string getName() {return name;}

  bool isLeaf() {return !left.first && !right.first;}

  std::string newick();  // Return Newick string

  int num_leaves();

 protected:
  std::pair<Tree*,double> left;
  std::pair<Tree*,double> right;
  std::string name;

  std::string newick_helper();
};


Tree* parse_tree(std::string s);
Tree* read_newick_file(std::string file);

#endif
