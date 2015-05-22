#include "tree.h"
#include <iostream>

int main(int argc, char** argv) {
  
  // Read in tree from a file (must contain exaclty 1 newick string)
  Tree* t = read_newick_file(argv[1]);

  // Print out newick string
  std::cout << t->newick() << std::endl;

  // Print out number of leaves
  std::cout << t->num_leaves() << std::endl;

  delete t;

  return 0;
}
