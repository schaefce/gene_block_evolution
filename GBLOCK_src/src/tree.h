// tree.h is part of GENEBLOCKER.
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
#include <cctype>
#include <cassert>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <streambuf>

#ifndef KARRO_TREE_H
#define KARRO_TREE_H

template <typename Node>
class Tree {
 public:
  Tree();
  Tree(Node* root);
  ~Tree();
  
  Node* getRoot(){ return root; }
  std::string getName() {return this->root->name;}

  int num_leaves();
  bool prune(Node* target);
  std::vector<Node*> collectLeaves();

  std::string newick();  // Return Newick string
  static Tree<Node>* read_newick_file(std::string file);
  static Tree<Node>* parse_tree(std::string s);

 protected:
  Node* root;
};

template<typename Node>
Tree<Node>::Tree() {
  this->root = (Node*)NULL;
}

template<typename Node>
Tree<Node>::Tree(Node* root) {
  this->root = root;
}

template<typename Node>
Tree<Node>::~Tree() {
  if (this->root)
    delete this->root;
}

template <typename Node>
std::string Tree<Node>::newick() {
  std::stringstream ss;
  this->root->newick_helper(ss);
  return ss.str() + ";";
}


template <typename Node>
bool Tree<Node>::prune(Node* target){
  if (!target){
    return false;
  }
  else if (target == this->root){
    this->root = (Node*)NULL;
    return true;
  }
  else {
    //root->prune(target);
    assert(target->hasParent());
    Node* parent = target->getParent();
    if (parent->hasChild(true) && target == parent->getChild(true)){
      parent->removeChild(true);
    }
    else if (parent->hasChild(false) && target == parent->getChild(false)){
      parent->removeChild(false);
    }
    
    return true;
  }
}

template <typename Node>
std::vector<Node*> Tree<Node>::collectLeaves(){
  std::stack<Node*> S;
  std::vector<Node*> leafList;
  S.push(this->root);
  while (!S.empty()) {
    Node* s = S.top();
    S.pop();
    if (s->isLeaf())
      leafList.push_back(s);
    else {
      if (s->hasChild(true))
        S.push(s->getChild(true));
      if (s->hasChild(false))
        S.push(s->getChild(false));
    }
  }
  return leafList;
}

template <typename Node>
int Tree<Node>::num_leaves() {
  std::stack<Node*> S;
  S.push(this->root);
  int count = 0;
  while (!S.empty()) {
    Node* s = S.top();
    S.pop();
    if (s->isLeaf())
      count++;
    else {
      if (s->hasChild(true))
	      S.push(s->getChild(true));
      if (s->hasChild(false))
	      S.push(s->getChild(false));
    }
  }
  return count;
}


// Grammer:
//   TREE     --> SUBTREE;
//   SUBTREE  --> (SUBTREE:WEIGHT,SUBTREE:WEIGHT)NAME | NAME
//   NAME    --> <string of 1 or more chaacters>
//   WEIGHT   --> <int> | <float>

// Determine of the string represents one of the grammer markers

inline bool is_marker(char c) {
  std::string markers = ":,();";
  for (std::string::iterator i = markers.begin(); i != markers.end(); i++)
    if (*i == c)
      return true;

  return false;
}

inline bool is_marker(std::string s) {
  if (s.size() > 1)
    return false;
  return is_marker(s[0]);
}

inline std::string next_token(const std::string& s, int& p) {
  while (p < s.size() && std::isspace(s[p])) p++;
  if (p >= s.size())
    return "";

  int p2 = p;
  if (is_marker(s[p]))
    p++;

  else {
    while (p < s.size()) {
      if (std::isspace(s[p]) || is_marker(s[p]))
	break;
      p++;
    }
  }
  return s.substr(p2, p-p2);
}


template <typename Node>
Node* parse_node(const std::string& s, int& p) {
  std::string t = next_token(s,p);
  if (t == "(") {
    Node* left = parse_node<Node>(s, p);

    t = next_token(s,p);
    //double left_weight;
    if (t == ":") {
      left->setWeight(std::stoi(next_token(s,p)));
      t = next_token(s,p);
    }

    assert(t == ",");
    Node* right = parse_node<Node>(s,p);

    t = next_token(s,p);
    //double right_weight;
    if (t == ":") {
      right->setWeight(std::stoi(next_token(s,p)));
      t = next_token(s,p);
    }

    assert(t == ")");

    std::string name = next_token(s,p);
    if (is_marker(name)) { // Hack for the case where there is no name.
      p--;
      name = "";
    }
    return new Node(left, right, name);
  }
  return new Node(t); // Leaf: token was the leaf labe
}

template <typename Node>
Tree<Node>* Tree<Node>::parse_tree(std::string s) {
  int p = 0;
  Node* root = parse_node<Node>(s, p);
  Tree* t = new Tree<Node>(root);
  std::string token = next_token(s,p);
  if (token == ":") { // String of format "(...)?:5" -- create a new root with a single (left) child).
    double weight = stoi(next_token(s,p));
    Node* root2 = new Node();
    root->setWeight(weight);
    root2->addChild(root, true);
    t = new Tree<Node>(root2);
    token = next_token(s,p);
  }

  assert(token == ";" && next_token(s,p) == "");
  return t;
}

template <typename Node>
Tree<Node>* Tree<Node>::read_newick_file(std::string file) {
  std::ifstream fin(file);
  std::string s((std::istreambuf_iterator<char>(fin)),
                  std::istreambuf_iterator<char>());
  return parse_tree(s);
}

#endif
