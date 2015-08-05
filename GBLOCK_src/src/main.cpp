// main.cpp is part of GENEBLOCKER.
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

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include "difflib.h"
#include "labeledtree.h"


using namespace boost;
namespace po = boost::program_options;

// HOMOLOG struct to easily contain the info needed for clustering
struct HOMOLOG {
  std::string accession;
  int start;
  int stop;
  std::string annotation;
  
  bool operator < (const HOMOLOG& h2) const {
    return (start < h2.start);
  }
};

// Groups all of the homologs. Stolen from make_event_distance_matrix
std::vector<std::vector<HOMOLOG>> homologListGroupingFunction(std::vector<HOMOLOG> &homologs, int maxGap){
  std::vector<std::vector<HOMOLOG>> result;
  
  std::vector<HOMOLOG> neighborhood;
  neighborhood.push_back(homologs[0]);
  
  if(homologs.size() > 1){
    for (int i = 1; i < homologs.size(); i++){
      int start = neighborhood.back().start;
      int stop = neighborhood.back().stop;
      
      int start_n = homologs[i].start;
      int stop_n = homologs[i].stop;
      
      if(abs(start - stop_n) < maxGap || abs(stop - start_n) < maxGap){
        neighborhood.push_back(homologs[i]);
      }
      else{
        result.push_back(neighborhood);
        neighborhood.clear();
        neighborhood.push_back(homologs[i]);
      }
    }
  }
  result.push_back(neighborhood);
  return result;
}



// Uses gene block result from file to determine the homologs for each organism
// and groups them according to homolog_list_grouping_function. Returns a mapping
// from organism's accession ID to list of homolog lists.
std::map<std::string, std::vector<std::string>> getLabelMap(std::string geneBlock, int maxGap){
  std::map<std::string, std::vector<std::string>> labelMap;
  std::map<std::string, std::vector<HOMOLOG>> orgMap;
  
  std::ifstream infile(geneBlock);
  std::string line;
  
  while(getline(infile,line)){
    boost::trim(line); // line represents a homolog object- parse to get attributes
    std::vector<std::string> homologV;
    boost::split(homologV, line, ::isspace);
    
    std::vector<std::string> subjLine;
    boost::split(subjLine, homologV[1], is_from_range('|','|'));
    std::string accession = subjLine[0];
    
    std::vector<std::string> queryLine;
    boost::split(queryLine, homologV[0], is_from_range('|','|'));
    std::string annotation = subjLine[3];
    
    HOMOLOG h ;
    h.accession = accession;
    h.start = stoi(subjLine[4]);
    h.stop = stoi(subjLine[5]);
    h.annotation = annotation;
    
    if (orgMap.count(accession)) {
      orgMap[accession].push_back(h);
    }
    else {
      std::vector<HOMOLOG> hlogs;
      hlogs.push_back(h);
      orgMap[accession] = hlogs;
    }
    
  }
  
  for (std::map<std::string,std::vector<HOMOLOG>>::iterator it = orgMap.begin(); it != orgMap.end(); it++){
    std::vector<HOMOLOG> homologs = it->second;
    std::sort(homologs.begin(), homologs.end());
    std::vector<std::vector<HOMOLOG>> groups = homologListGroupingFunction(homologs, maxGap);
    std::vector<std::string> neighbors;
    for (std::vector<HOMOLOG> neighbor : groups){
      for (HOMOLOG h : neighbor){
        neighbors.push_back(h.annotation);
      }
    }
    labelMap[it->first] = neighbors;
  }
  
  return labelMap;
}


// Creates a mapping of each organism's common name to its corresponding ID
std::map<std::string,std::string> getIdentifiers(std::string fname){
  std::map<std::string, std::string> idMap;
  std::ifstream infile(fname);
  std::string line;
  while(getline(infile,line)){
    boost::trim(line);
    std::vector<std::string> idV; //idV consists of 2 parts -- name and id
    boost::split(idV, line, is_from_range(',',','));
    idMap[idV[1]] = idV[0];
  }
  return idMap;
}

// Uses gene block (1) geneBlock organism data, (2) a mapping of common names to
// IDs, and (3) an existing tree file to create a tree labeled with list of homologs.
LabeledTree* formatTree(std::string geneBlock, std::string idFile , std::string treeFile, int maxGap, bool noPrune, int verbosity){
  std::map<std::string, std::vector<std::string>> labelMap = getLabelMap(geneBlock, maxGap);
  std::map<std::string, std::string> idMap = getIdentifiers(idFile);
  LabeledTree* tree = (LabeledTree*)LabeledTree::read_newick_file(treeFile);
  if (verbosity > 0){
    std::cout << "--- ORIGINAL TREE ---" << std::endl;
    std::cout << tree->newick() << std::endl << std::endl;
  }
  tree->addIdsAndLabels(idMap, labelMap, !noPrune);
  return tree;
}


// Helps set all possible labels for the tree using preorder traversal
void setPossibleLabelsHelper(LabeledNode* node, int verbosity) {
  if(node && node->getChild(true) && node->getChild(false) ){
    setPossibleLabelsHelper(node->getChild(true), verbosity);
    setPossibleLabelsHelper(node->getChild(false), verbosity);
    node->setLabel(LabelMatcher::getAncestorLabel(node->getChild(true)->getLabel(), node->getChild(false)->getLabel(), verbosity));
    if (verbosity > 1){
      if (node->getLabel()){
        std::cout << "Parent assigned label: " << *node->getLabel() << std::endl;
      }
      else{
        std::cout << "Parent assigned label: NULL" << std::endl;
      }
    }
  }
}


// Sets all possible labels for the tree using preorder traversal
void setPossibleLabels(LabeledTree* tree, int verbosity) {
  LabelMatcher::initialize();
  if(tree && tree->getRoot()){
    setPossibleLabelsHelper(tree->getRoot(), verbosity);
  }
}

void writeTree(LabeledTree* tree, std::string ofile, int verbosity){
  if (verbosity > 0){
    std::cout << "--- LABELED TREE ---" << std::endl;
    std::cout << tree->newick() << std::endl << std::endl;
  }
  std::ofstream fstream;
  fstream.open(ofile);
  fstream << tree->newick();
  fstream.close();
}


int main(int argc, char **argv){
  
  std::string geneblock;
  std::string tree;
  std::string ids;
  
  // command line options
  po::options_description desc("Geneblocker Usage Options");
  desc.add_options()
  ("help,h", "Display this help message")
  ("noPrune,n", po::value<bool>()->default_value(false), "Do not prune unlabeled leaves")
  ("maxGap,g", po::value<int>()->default_value(5), "Maximum gap size")
  ("fname,f", po::value<std::string>(), "File to output tree to.")
  ("verbosity,v", po::value<int>()->default_value(1), "Verbosity level.")
  ("geneBlock", po::value<std::string>(&geneblock)->required(), "Path to file with data about geneblock")
  ("treeFile", po::value<std::string>(&tree)->required(), "Path to file with tree data")
  ("idMap", po::value<std::string>(&ids)->required(), "Path to file mapping IDs to labels");
  po::positional_options_description p;
  p.add("geneBlock", 1).add("treeFile", 1).add("idMap", 1);
  
  // parse command line arguments
  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, desc),vm);
    
    if(vm.count("help")){
      std::cout << desc << std::endl;
      return 0;
    }
    
    po::notify(vm);

    
  }
  catch(std::exception& e){
    std::cerr << "Error parsing options: " << e.what() << std::endl;
    std::cerr << desc << std::endl;
    return 1;
  }
  
  boost::filesystem::path filePath(tree);
  std::string baseTreeFile = filePath.stem().string();
  std::string ofile = vm.count("fname") ? vm["fname"].as<std::string>() : baseTreeFile + ".labeled.nwk";
  
  if(vm["verbosity"].as<int>() > 1){
    std::cout << "GBLOCK OPTIONS:" << std::endl << "GBLOCK FILE: " << geneblock << std::endl
    << "TREE FILE: " << tree << std::endl << "IDS FILE: " << ids << std::endl
    << "OUTPUT FILE: " << ofile << std::endl << "MAXGAP: " << vm["maxGap"].as<int>() << std::endl
    << "PRUNING? " << !vm["noPrune"].as<bool>() << std::endl << std::endl;
  }
  // Read in all information given as input, and set leaf labels
  LabeledTree* lTree = formatTree(geneblock, ids, tree, vm["maxGap"].as<int>(),
                                  vm["noPrune"].as<bool>(),vm["verbosity"].as<int>());
  // Preorder traversal, setting each parent with all possible labels via children
  setPossibleLabels(lTree, vm["verbosity"].as<int>());
  // Postorder traversal. Choose best label for root and pass decision down tree
  lTree->setLabelsFromRoot();
  
  writeTree(lTree, ofile, vm["verbosity"].as<int>());
}
