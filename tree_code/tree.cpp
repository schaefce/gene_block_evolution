#include "tree.h"
#include <cctype>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

Tree::Tree() {
  left = std::make_pair((Tree*)NULL,0);
  right = std::make_pair((Tree*)NULL,0);
  label = "";
}

Tree::Tree(std::string label) {
  left = std::make_pair((Tree*)NULL,0);
  right = std::make_pair((Tree*)NULL,0);
  this->label = label;
}

Tree::Tree(std::pair<Tree*,double> left, std::pair<Tree*,double> right, std::string label) {
  this->left = left;
  this->right = right;
  this->label = label;
}

Tree::~Tree() {
  if (hasChild(true))
    delete getChild(true);
  if (hasChild(false))
    delete getChild(false);
}

void Tree::addChild(Tree* t, double weight, bool left) {
  if (hasChild(left))
    delete getChild(left);

  if (left)
    this->left = std::make_pair(t,weight);
  else 
    this->right = std::make_pair(t,weight);
}

std::string Tree::newick_helper() {
  if (isLeaf())
      return label;

  std::string s = "(";
  if (hasChild(true))
    s += getChild(true)->newick_helper() + ":" + std::to_string(getWeight(true));
  if (hasChild(true) && hasChild(false))
    s += ",";
  if (hasChild(false))
    s += getChild(false)->newick_helper() + ":" + std::to_string(getWeight(false));
  return s + ")" + label;
}


std::string Tree::newick() {
  return this->newick_helper() + ";";
}

int Tree::num_leaves() {
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
//   SUBTREE  --> (SUBTREE:WEIGHT,SUBTREE:WEIGHT)LABEL | LABEL
//   LABEL    --> <string of 1 or more chaacters>
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
 
 

Tree* parse_subtree(const std::string& s, int& p) {
  std::string t = next_token(s,p);


  if (t == "(") {
    Tree* left = parse_subtree(s, p);

    t = next_token(s,p);
    double left_weight;
    if (t == ":") {
      left_weight = std::stoi(next_token(s,p));
      t = next_token(s,p);
    }
    else         // No edge weight provided
      left_weight = 0;

    assert(t == ",");
    Tree* right = parse_subtree(s,p);
  
    t = next_token(s,p);
    double right_weight;
    if (t == ":") {
      right_weight =  std::stoi(next_token(s,p));
      t = next_token(s,p);
    }
    else
      right_weight = 0;

    assert(t == ")");

    std::string label = next_token(s,p);
    if (is_marker(label)) { // Hack for the case where there is no label.
      p--;
      label = "";
    }
    return new Tree(std::make_pair(left, left_weight), std::make_pair(right, right_weight), label);
  }
  return new Tree(t); // Leaf: token was the leaf labe
}

Tree* parse_tree(std::string s) {
  int p = 0;
  Tree* t = parse_subtree(s, p);
  std::string token = next_token(s,p);
  if (token == ":") { // String of format "(...)?:5" -- create a new root with a single (left) child).
    double weight = stoi(next_token(s,p));
    t = new Tree(std::make_pair(t,weight), std::make_pair((Tree*)NULL,0), std::string(""));
    token = next_token(s,p);
  }

  assert(token == ";" && next_token(s,p) == "");
  return t;
}
 
Tree* read_newick_file(std::string file) {
  std::ifstream fin(file);
  std::stringstream strstream;
  strstream << fin.rdbuf();

  return parse_tree(strstream.str());
}
