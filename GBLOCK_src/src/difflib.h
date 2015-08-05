#ifndef GBLOCK_DIFFLIB_H
#define GBLOCK_DIFFLIB_H

/**
 * Module difflib_geneblock -- helpers for computing deltas between objects.
 * Class LabelMatcher:
 * 		A flexible class for comparing pairs of labels.
 */

#include "label.h"

typedef std::vector<std::string> stringVector;
typedef std::vector<stringVector> ssVector;

struct splitGroupPiece{
  stringVector piece;
  float score;
  
  splitGroupPiece(std::string piece, float score){
    this->piece.push_back(piece);
    this->score = score;
  }
  splitGroupPiece(stringVector pieces, float score){
    piece = pieces;
    this->score = score;
  }
  splitGroupPiece(float score){
    this->score = score;
  }
};

typedef std::vector<splitGroupPiece*> splitGroup;
typedef std::vector<splitGroup> splitGroupVector;

struct Intermediate{
  ssVector splits;
  float score;
  Intermediate(ssVector splits, float score){
    this->splits = splits;
    this->score =score;
  }
};
typedef std::vector<Intermediate*> intermediatesVector;

struct Iters{
  splitGroup::const_iterator begin;
  splitGroup::const_iterator end;
  splitGroup::const_iterator me;
};

// Matches two choices as best as possible, where a choice is a list of
// lists, the outer list's order doesn't matter but the inner list's order matters
class LabelMatcher {
  
public:
  static void initialize();
  static float getMinEditDistance(ssVector choice1, ssVector choice2, intermediatesVector &intermediates, bool backtrace);
  static Label* getAncestorLabel(Label *L1, Label *L2, int verbosity);
  
private:
  static float getDeletionPenalty(stringVector A, stringVector B, int j, int i);
  static float getInsertionPenalty(stringVector A, stringVector B, int j, int i);
  static float getMatchPenalty(stringVector A, stringVector B, int j, int i);
  static float getSpecialSplitPenalty(stringVector A, stringVector B, int j, int i);
  static splitGroupVector crossProduct(splitGroupVector input);
  static float getEditDistance(stringVector A, stringVector B, intermediatesVector &intermediates, bool backtrace);
  static void performBacktrace(stringVector A, stringVector B, float **subproblems, intermediatesVector &intermediates);
  
};


#endif
