#ifndef GBLOCK_DIFFLIB
#define GBLOCK_DIFFLIB

/**
 * Module difflib_geneblock -- helpers for computing deltas between objects.
 * Class LabelMatcher:
 * 		A flexible class for comparing pairs of labels.
 */

//import itertools
//import re
//import logging
//from Labeled_Tree import Label, Choice, Choice_Group
#include <string>



class LabelMatcher { //:
    /**
     * Matches two choices as best as possible, where a choice is a list of
     * 	lists, the outer list's order doesn't matter but the inner list's order matters
     */

public:
  //LabelMatcher(Label* one, Label* two);
  LabelMatcher();
  
  static float getEditDistance(std::string A, std::string B, bool allowInsertions=false, bool backtrace=false);
  
  
  
private:
  static float DUP_PENALTY, DEL_DUP_PENALTY, SPLIT_PENALTY, DEL_SPLIT_PENALTY, DEL_PENALTY, INS_PENALTY, MISMATCH_PENALTY, MATCH_PENALTY;
  static char SPLIT;
  
  static float getDeletionPenalty(std::string A, std::string B, int j, int i);
  static float getInsertionPenalty(std::string A, std::string B, int j, int i);
  static float getMatchPenalty(std::string A, std::string B, int j, int i);
  static float getSpecialSplitPenalty(std::string A, std::string B, int j, int i);



};


#endif