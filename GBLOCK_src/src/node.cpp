#include "Node.h"
#include <cctype>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

Node::Node() {
  left = std::make_pair((Node*)NULL,0);
  right = std::make_pair((Node*)NULL,0);
  name = "";
}

Node::Node(std::string name) {
  left = std::make_pair((Node*)NULL,0);
  right = std::make_pair((Node*)NULL,0);
  this->name = name;
}

Node::Node(std::pair<Node*,double> left, std::pair<Node*,double> right, std::string name) {
  this->left = left;
  this->right = right;
  this->name = name;
}

Node::~Node() {
  if (hasChild(true))
    delete getChild(true);
  if (hasChild(false))
    delete getChild(false);
}

void Node::addChild(Node* t, double weight, bool left) {
  if (hasChild(left))
    delete getChild(left);

  if (left)
    this->left = std::make_pair(t,weight);
  else
    this->right = std::make_pair(t,weight);
}

std::string Node::newick_helper() {
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


// Grammer:
//   Node     --> SUBNode;
//   SUBNode  --> (SUBNode:WEIGHT,SUBNode:WEIGHT)NAME | NAME
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



Node* parse_subNode(const std::string& s, int& p) {
  std::string t = next_token(s,p);


  if (t == "(") {
    Node* left = parse_subNode(s, p);

    t = next_token(s,p);
    double left_weight;
    if (t == ":") {
      left_weight = std::stoi(next_token(s,p));
      t = next_token(s,p);
    }
    else         // No edge weight provided
      left_weight = 0;

    assert(t == ",");
    Node* right = parse_subNode(s,p);

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
    return new Node(std::make_pair(left, left_weight), std::make_pair(right, right_weight), name);
  }
  return new Node(t); // Leaf: token was the leaf labe
}

Node* parse_Node(std::string s) {
  int p = 0;
  Node* t = parse_subNode(s, p);
  std::string token = next_token(s,p);
  if (token == ":") { // String of format "(...)?:5" -- create a new root with a single (left) child).
    double weight = stoi(next_token(s,p));
    t = new Node(std::make_pair(t,weight), std::make_pair((Node*)NULL,0), std::string(""));
    token = next_token(s,p);
  }

  assert(token == ";" && next_token(s,p) == "");
  return t;
}

Node* read_newick_file(std::string file) {
  std::ifstream fin(file);
  std::stringstream strstream;
  strstream << fin.rdbuf();

  return parse_Node(strstream.str());
}
