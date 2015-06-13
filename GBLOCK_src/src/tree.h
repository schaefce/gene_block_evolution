// Tree.h: Simple (binary) tree class
// Defined: A treee is a node with zero or more trees as children.
#include <string>
#include <iostream>
#include <cctype>
#include <cassert>
#include <fstream>
#include <sstream>
#include <stack>

#ifndef KARRO_TREE_H
#define KARRO_TREE_H

template <class N>
class Tree {
 public:
  Tree();
  Tree()
  Tree(std::string name);

  Tree(std::pair<Tree*<N>,double> left, std::pair<Tree*<N>,double> right, std::string name);

  ~Tree();

  bool hasChild(bool left) {
    return left ? this->left.first : this->right.first;
  }

  Tree*<N> getChild(bool left) {
    return left ? this->left.first : this->right.first;
  }

  double getWeight(bool left) {
    return left ? this->left.second : this->right.second;
  }

  void addChild(Tree*<N> t, double weight, bool left);

  std::string getName() {return name;}

  bool isLeaf() {return !left.first && !right.first;}

  std::string newick();  // Return Newick string

  int num_leaves();

 protected:
  std::pair<Tree*<N>,double> left;
  std::pair<Tree*<N>,double> right;
  N* nodeData;
  std::string newick_helper();
};


Tree*<N> parse_tree(std::string s);
Tree*<N> read_newick_file(std::string file);

#endif



template<class N>
Tree<N>::Tree() {
  left = std::make_pair((Tree*)NULL,0);
  right = std::make_pair((Tree*)NULL,0);
  name = "";
}

template<class N>
Tree<N>::Tree(std::string name) {
  this->left = std::make_pair((Tree*<N>)NULL,0);
  this->right = std::make_pair((Tree*<N>)NULL,0);
  this->nodeData->name = name;
}

template<class N>
Tree::Tree(std::pair<Tree*<N>,double> left, std::pair<Tree*<N>,double> right, std::string name) {
  this->left = left;
  this->right = right;
  this->nodeData->name = name;
}

template<class N>
Tree<N>::~Tree() {
  if (hasChild(true))
    delete getChild(true);
  if (hasChild(false))
    delete getChild(false);
}

template<class N>
void Tree<N>::addChild(Tree*<N> t, double weight, bool left) {
  if (hasChild(left))
    delete getChild(left);

  if (left)
    this->left = std::make_pair(t,weight);
  else
    this->right = std::make_pair(t,weight);
}

template<class N>
std::string Tree<N>::newick_helper() {
  if (isLeaf())
      return name;

  std::string s = "(";
  if (hasChild(true))
    s += getChild(true)->newick_helper() + ":" + std::to_string(getWeight(true));
  if (hasChild(true) && hasChild(false))
    s += ",";
  if (hasChild(false))
    s += getChild(false)->newick_helper() + ":" + std::to_string(getWeight(false));
  return s + ")" + name;
}

template <class N>
std::string Tree<N>::newick() {
  return this->newick_helper() + ";";
}

template <class N>
int Tree<N>::num_leaves() {
  std::stack<Tree*> S;
  S.push(this);
  int count = 0;
  while (!S.empty()) {
    Tree* s = S.top();
    S.pop();
    if (s->isLeaf())
      count++;
    else {
      if (hasChild(true))
	S.push(s->getChild(true));
      if (hasChild(false))
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
std::string markers = ":,();";
inline bool is_marker(char c) {

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


template <class N>
Tree*<N> parse_subtree(const std::string& s, int& p) {
  std::string t = next_token(s,p);


  if (t == "(") {
    Tree*<N> left = parse_subtree(s, p);

    t = next_token(s,p);
    double left_weight;
    if (t == ":") {
      left_weight = std::stoi(next_token(s,p));
      t = next_token(s,p);
    }
    else         // No edge weight provided
      left_weight = 0;

    assert(t == ",");
    Tree*<N> right = parse_subtree(s,p);

    t = next_token(s,p);
    double right_weight;
    if (t == ":") {
      right_weight =  std::stoi(next_token(s,p));
      t = next_token(s,p);
    }
    else
      right_weight = 0;

    assert(t == ")");

    std::string name = next_token(s,p);
    if (is_marker(name)) { // Hack for the case where there is no name.
      p--;
      name = "";
    }
    return new Tree<N>(std::make_pair(left, left_weight), std::make_pair(right, right_weight), name);
  }
  return new Tree<N>(t); // Leaf: token was the leaf labe
}

template <class N>
Tree*<N> parse_tree(std::string s) {
  int p = 0;
  Tree*<N> t = parse_subtree(s, p);
  std::string token = next_token(s,p);
  if (token == ":") { // String of format "(...)?:5" -- create a new root with a single (left) child).
    double weight = stoi(next_token(s,p));
    t = new Tree<N>(std::make_pair(t,weight), std::make_pair((Tree*)NULL,0), std::string(""));
    token = next_token(s,p);
  }

  assert(token == ";" && next_token(s,p) == "");
  return t;
}

template <class N>
Tree*<N> read_newick_file(std::string file) {
  std::ifstream fin(file);
  std::stringstream strstream;
  strstream << fin.rdbuf();

  return parse_tree(strstream.str());
}
