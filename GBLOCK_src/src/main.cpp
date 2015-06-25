#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

#include "labeledtree.h"
#include "difflib.h"

using namespace boost;
namespace po = boost::program_options;

struct HOMOLOG {
  /**
    * HOMOLOG struct to easily contain the info needed for clustering
    */
  std::string accession;
  int start;
  int stop;
  std::string annotation;

  bool operator < (const HOMOLOG& h2) const {
    return (start < h2.start);
  }
};

std::vector<std::vector<HOMOLOG>> homologListGroupingFunction(std::vector<HOMOLOG> &homologs, int maxGap){
  //""" Stole this out of make_event_distance_matrix """
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



std::map<std::string, std::vector<std::string>> getLabelMap(std::string geneBlock, int maxGap){
  /** Use gene_block result from file to determine the homologs for each organism
    * and group them according to homolog_list_grouping_function. Returns a mapping
    * from organism's accession ID to list of homolog lists.
    */

  std::map<std::string, std::vector<std::string>> labelMap;

  std::map<std::string, std::vector<HOMOLOG>> orgMap;

  std::ifstream infile(geneBlock);
  std::string line;

  while(getline(infile,line)){
    boost::trim(line);
    std::vector<std::string> homologV; //homologV will consist of 12 parts- we are interested in the 2nd and
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


std::map<std::string,std::string> getIdentifiers(std::string fname){
  /**
    * Given name of csv file of ids and common names, creates a mapping of each
    * common name to its respective id.
    */

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

// TODO: figure out how to handle this...
//def write_tree(tree, fname, treeformat='newick'):
    //logger.info("Writing phylogenetic tree to file " + fname)
    //Phylo.write(tree, fname, treeformat)


LabeledTree* formatTree(std::string geneBlock, std::string idFile , std::string treeFile, int maxGap, bool noPrune){
  /**
    * Use (1) geneBlock organism data, (2) a mapping of common names to
    * IDs, and (3) an existing tree file to create a tree labeled with list of homologs.
    *
    */
  std::map<std::string, std::vector<std::string>> labelMap = getLabelMap(geneBlock, maxGap);
  std::map<std::string, std::string> idMap = getIdentifiers(idFile);
  LabeledTree* tree = (LabeledTree*)LabeledTree::read_newick_file(treeFile);
  //TODO: figure out how to get this working
  tree->addIdsAndLabels(idMap, labelMap, !noPrune);
  return tree;
}


void setPossibleLabelsHelper(LabeledNode* node) {
  /**
   * Set all possible labels for the tree.
   */
  //tree.ladderize()
  if(node && node->getChild(true) && node->getChild(false) ){
    setPossibleLabelsHelper(node->getChild(true));
    setPossibleLabelsHelper(node->getChild(false));
    node->setLabel(LabelMatcher::getAncestorLabel(node->getChild(true)->getLabel(), node->getChild(false)->getLabel()));
  }
}



void setPossibleLabels(LabeledTree* tree) {
  /**
    * Set all possible labels for the tree.
    */
  //tree.ladderize()
  LabelMatcher::initialize();
  if(tree && tree->getRoot()){
    setPossibleLabelsHelper(tree->getRoot());
  }
}

void writeTree(LabeledTree* tree, std::string ofile){
  //TODO
}


int main(int argc, char **argv){

  std::string geneblock;
  std::string tree;
  std::string ids;

  po::options_description desc("Geneblocker Usage Options");

  desc.add_options()
    ("help,h", "Display this help message")
    ("noPrune,n", po::value<bool>()->default_value(false), "Do not prune unlabeled leaves")
    ("maxGap,g", po::value<int>()->default_value(5), "Maximum gap size")
    ("fname,f", po::value<std::string>(), "File to output tree to.")
    ("geneBlock", po::value<std::string>(&geneblock)->required(), "Path to file with data about geneblock")
    ("treeFile", po::value<std::string>(&tree)->required(), "Path to file with tree data")
    ("idMap", po::value<std::string>(&ids)->required(), "Path to file mapping IDs to labels");

  po::positional_options_description p;
  p.add("geneBlock", 1).add("treeFile", 1).add("idMap", 1);


  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, desc),vm);

    if(vm.count("help")){
      std::cout << desc << std::endl;
      return 0;
    }

    po::notify(vm);

  } catch(std::exception& e){
    std::cerr << "Error parsing options: " << e.what() << std::endl;
    std::cerr << desc << std::endl;
    return 1;
  }

  boost::filesystem::path filePath(tree);
  std::string baseTreeFile = filePath.stem().string();

  LabeledTree* lTree = formatTree(geneblock, ids, tree, vm["maxGap"].as<int>(), vm["noPrune"].as<bool>());
  setPossibleLabels(lTree); //,logname);

  lTree->setLabelsFromRoot();

  std::string ofile;

  if(vm.count("fname")){
    ofile = vm["fname"].as<std::string>();
  }
  else{
    ofile = baseTreeFile + "labeled.nwk";
  }

  writeTree(lTree, ofile);
}
