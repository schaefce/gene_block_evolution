//
//  difflib.cpp
//  GeneBlock
//
//  Created by Schaeffer, Charlotte Elizabeth Ms. on 6/19/15.
//  Copyright (c) 2015 Charlotte Schaeffer. All rights reserved.
//
#include "difflib.h"
#include <limits>
#include <algorithm>
#include <map>

static float DUP_PENALTY, DEL_DUP_PENALTY, SPLIT_PENALTY, DEL_SPLIT_PENALTY, DEL_PENALTY, INS_PENALTY, MISMATCH_PENALTY, MATCH_PENALTY;
static std::string SPLIT;
static bool allowInsertions;

LabelMatcher::LabelMatcher(){
  DEL_DUP_PENALTY = DUP_PENALTY = 5;
  SPLIT_PENALTY = DEL_SPLIT_PENALTY = 7;
  INS_PENALTY = DEL_PENALTY = 3;
  MISMATCH_PENALTY = 100000;
  MATCH_PENALTY = -1;
  SPLIT = "*";
}

float LabelMatcher::getDeletionPenalty(stringVector A, stringVector B, int j, int i){
  /**
    * Determine cost of deleting Bi.
    */
  if (i > 0 && B[i] == B[i-1]){
    return DEL_DUP_PENALTY;
  }
  else if (B[i] == SPLIT){
    return DEL_SPLIT_PENALTY;
  }
  else{
    return DEL_PENALTY;
  }
}


float LabelMatcher::getInsertionPenalty(stringVector A, stringVector B, int j, int i){
  /**
   * Determine cost of inserting Aj after Bi.
   */
  if (j > 0 && (A[j] == A[j-1] && A[j-1] == B[i])){
    return DUP_PENALTY;
  }
  else if (A[j] == SPLIT){
    return SPLIT_PENALTY;
  }
  else {
    return INS_PENALTY;
  }
}

float LabelMatcher::getMatchPenalty(stringVector A, stringVector B, int j, int i){
  /**
   * Determine cost of doing nothing
   */
  if (A[j] == B[i]){
    return MATCH_PENALTY;
  }
  else {
    return MISMATCH_PENALTY;
  }
}

float LabelMatcher::getSpecialSplitPenalty(stringVector A, stringVector B, int j, int i){
  /**
   * Determine
   */
  if (j > 1 && (B[i] == A[j] && A[j]== A[j-2]) && A[j-1] == SPLIT){
    return DUP_PENALTY + SPLIT_PENALTY;
  }
  else {
    return MISMATCH_PENALTY;
  }
}

float LabelMatcher::getEditDistance(stringVector A, stringVector B, intermediatesVector &intermediates, bool backtrace){
  if (!intermediates.empty()){
    intermediates.clear();
  }
  long n = A.size();
  long m = B.size();
  //std::vector<std::vector<int>> subproblems;
  int **subproblems = new int*[m+1];
  for (int i = 0; i < m+1; i++){
    subproblems[i] = new int[n+1];
  }
  
  for (int j = 0; j < n+1; j++){
    subproblems[0][j] = j * INS_PENALTY;
  }
  for (int i = 0; i < n+1; i++){
    subproblems[i][0] = i * DEL_PENALTY;
  }
  
  int aj, bi;
  float case1, case2, case3, case4;
  for (int j = 1; j < n+1; j++){
    aj = j - 1;
    for (int i = 1; i < m+1; i++){
      bi = i - 1;
      // case 1 : Bi and Aj match
      case1 = subproblems[i-1][j-1] + getMatchPenalty(A, B, aj, bi);
      // case 2 : Delete Bi
      case2 = subproblems[i-1][j] + getDeletionPenalty(A, B, aj, bi);
      // case 3 : Insert Aj at Bi
      case3 = subproblems[i][j-1] + getInsertionPenalty(A, B, aj, bi);
      if (j > 1){
        // case 4: Insert a Aj-1 and Aj (Aj matches Aj-2 (a dup) and Aj-1 is a split)
        case4 = subproblems[i][j-2] + getSpecialSplitPenalty(A, B, aj, bi);
      }
      else{
        case4 = std::numeric_limits<float>::max();
      }
      subproblems[i][j] = std::min(std::min(std::min(case1,case2),case3),case4);
    }
  }
  
  float penalty = subproblems[m][n];
  //intermediatesVector intermediates;
  if(backtrace)
    intermediates = performBacktrace(A, B, subproblems, intermediates);
  
  return penalty; //TODO: return penalty, intermediates;
}

intermediatesVector LabelMatcher::performBacktrace(stringVector A, stringVector B, int **subproblems, intermediatesVector &intermediates){
  /**
    * Perform backtrace to determine what alignment of A and B produced lowest penalty in subproblems
    */
  
  //intermediatesVector intermediates;
  
  splitGroupVector splits;
  long j = A.size(); //j = n
  long aj = j - 1;
  long i = B.size(); //i = m
  long bi = i - 1;
  while (i > 0 || j > 0){
    std::vector<splitGroupPiece> currIntermediate;
    float pos = subproblems[i][j];
    float case1_match = subproblems[i-1][j-1] + MATCH_PENALTY;
    float case2_del_dup = subproblems[i-1][j] + DEL_DUP_PENALTY;
    float case2_del_split = subproblems[i-1][j] + DEL_SPLIT_PENALTY;
    float case2_del_gen = subproblems[i-1][j] + DEL_PENALTY;
    float case3_ins_dup = subproblems[i][j-1] + DUP_PENALTY;
    float case3_ins_split = subproblems[i][j-1] + SPLIT_PENALTY;
    float case3_ins_gen = subproblems[i][j-1] + INS_PENALTY;
    if (i > 0 && j > 0 & pos == case1_match){
      currIntermediate.push_back(splitGroupPiece(B[bi], MATCH_PENALTY));
      bi -= 1;
      aj -= 1;
      i -= 1;
      j -= 1;
    }
    else if (j > 1 && pos == subproblems[i][j-2] + DUP_PENALTY + SPLIT_PENALTY) {
      currIntermediate.push_back(splitGroupPiece((std::vector<std::string>) {A[aj],A[aj-1]}, DEL_DUP_PENALTY + DEL_SPLIT_PENALTY));
      currIntermediate.push_back(splitGroupPiece(A[aj], DEL_DUP_PENALTY + SPLIT_PENALTY));
      currIntermediate.push_back(splitGroupPiece(A[aj-1], DUP_PENALTY + DEL_SPLIT_PENALTY));
      currIntermediate.push_back(splitGroupPiece(DUP_PENALTY + SPLIT_PENALTY));
      j -= 2;
      aj -= 2;
    }
    else if (i > 0 && pos == case2_del_dup){
      currIntermediate.push_back(splitGroupPiece(B[bi], DEL_DUP_PENALTY));
      currIntermediate.push_back(splitGroupPiece(DUP_PENALTY));
      i -= 1;
      bi -= 1;
    }
    else if (i > 0 && pos == case2_del_split){
      currIntermediate.push_back(splitGroupPiece(B[bi], DEL_SPLIT_PENALTY));
      currIntermediate.push_back(splitGroupPiece(SPLIT_PENALTY));
      i -= 1;
      bi -= 1;
    }
    else if (i > 0 && pos == case2_del_gen){
      currIntermediate.push_back(splitGroupPiece(B[bi],DEL_PENALTY));
      
      if (allowInsertions){
        currIntermediate.push_back(splitGroupPiece(INS_PENALTY)); //IF ALLOWED TO HAVE SUDDEN INSERTS
      }
      i -= 1;
      bi -= 1;
    }
    else if (j > 0 && pos == case3_ins_dup) {
      currIntermediate.push_back(splitGroupPiece(A[aj],DEL_DUP_PENALTY));
      currIntermediate.push_back(splitGroupPiece(DUP_PENALTY));
      j -= 1;
      aj -= 1;
    }
    
    else if (j > 0 && pos == case3_ins_split) {
      currIntermediate.push_back(splitGroupPiece(A[aj],DEL_SPLIT_PENALTY));
      currIntermediate.push_back(splitGroupPiece(SPLIT_PENALTY));
      j -= 1;
      aj -= 1;
    }
    else if (j > 0 && pos == case3_ins_gen) {
      currIntermediate.push_back(splitGroupPiece(A[aj], DEL_PENALTY));
      if (allowInsertions){
        currIntermediate.push_back(splitGroupPiece(INS_PENALTY)); //IF ALLOWED TO HAVE SUDDEN INSERTS
      }
      j -= 1;
      aj -= 1;
    }
    splits.push_back(currIntermediate);
  }
  std::reverse(intermediates.begin(), intermediates.end());
  
  splitGroupVector bestSplitGroups = crossProduct(splits);
  
  for (splitGroup sg : bestSplitGroups){
    stringVector parts;
    float score = 0;
    for (splitGroupPiece p : sg){
      for (std::string s : p.piece){
        if (s != "") parts.push_back(s);
      }
      score += p.score;
    }
    ssVector splits = groupBy(parts, SPLIT);
    intermediates.push_back(Intermediate(splits,score));
  }
  return intermediates;
}

splitGroupVector LabelMatcher::crossProduct(splitGroupVector input){
  splitGroupVector out;
  std::vector<Iters> Vi;
  
  // Start all of the iterators at the beginning.
  for(splitGroupVector::const_iterator it = input.begin(); it != input.end();++it) {
    Iters i = {(*it).begin(), (*it).end(), (*it).begin()};
    Vi.push_back(i);
  }
  
  
  while(1) {
    
    // Construct your first product vector by pulling
    // out the element of each vector via the iterator.
    splitGroup result;
    for(std::vector<Iters>::const_iterator it = Vi.begin(); it != Vi.end(); it++) {
      result.push_back(*(it->me));
    }
    
    out.push_back(result);
    
    // Increment the rightmost one, and repeat.
    
    // When you reach the end, reset that one to the beginning and
    // increment the next-to-last one. You can get the "next-to-last"
    // iterator by pulling it out of the neighboring element in your
    // vector of iterators.
    for(std::vector<Iters>::iterator it = Vi.begin(); ; ) {
      // okay, I started at the left instead. sue me
      ++(it->me);
      if(it->me == it->end) {
        if(it+1 == Vi.end()) {
          // I'm the last digit, and I'm about to roll
          return out;
        } else {
          // cascade
          it->me = it->begin;
          ++it;
        }
      } else {
        // normal
        break;
      }
    }
  }
}



float LabelMatcher::getMinEditDistance(stringVector choice1, stringVector choice2, intermediatesVector &intermediates, bool backtrace){
  float currMin = std::numeric_limits<float>::max();
  std::pair<stringVector,stringVector> bestCombo;
    
  do {
    stringVector A = reSplit(join<std::string>(choice1, SPLIT), "\\W+");
    A.erase(std::remove_if(A.begin(), A.end(), [](std::string s){ return s == ","; }));
      
    do {
      stringVector B = reSplit(join<std::string>(choice2, SPLIT), "\\W+");
      B.erase(std::remove_if(B.begin(), B.end(), [](std::string s){ return s == ","; }));
      intermediatesVector currIntermediates;
      float score = getEditDistance(A, B, currIntermediates, false);
      if (score < currMin){
        currMin = score;
        bestCombo.first = A;
        bestCombo.second = B;
      }
      score = getEditDistance(B, A, currIntermediates, false);
      if (score < currMin){
        currMin = score;
        bestCombo.first = B;
        bestCombo.second = A;
      }
    } while(std::next_permutation(choice2.begin(),choice2.end()));
  } while(std::next_permutation(choice1.begin(),choice1.end()));

  if (backtrace){
    //intermediatesVector currIntermediates;
    getEditDistance(bestCombo.first, bestCombo.second, intermediates, true);
  }
  return currMin;
}


Label* LabelMatcher::getAncestorLabel(Label *L1, Label *L2){
  std::map<std::string, std::pair<float,int>> choiceMap;
  std::vector<ChoiceGroup*> choiceGroups;
  int i = 0;
  float prescore;
  for (ChoiceGroup* grp1 : L1->getChoiceGroups()){
    for (ChoiceGroup* grp2 : L2->getChoiceGroups()){
      for (Choice* choice1 : grp1->getChoices()){
        stringVector c1 = choice1->getStringGroups();
        for (Choice* choice2 : grp2->getChoices()){
          stringVector c2 = choice2->getStringGroups();
          prescore = choice1->getScore() + choice2->getScore();
          intermediatesVector intermediates;
          float score = getMinEditDistance(c1, c2, intermediates, true);
          std::vector<Choice*> currChoices;
          std::transform(intermediates.begin(), intermediates.end(), currChoices.begin(), [prescore](Intermediate i){
            return new Choice(i.splits, i.score + prescore);
          });
          for (Choice* c : currChoices){
            std::string choiceString = c->groupListString();
            if(!choiceMap.count(choiceString)){
              std::pair<float,int> curr = choiceMap[choiceString];
              if (curr.first > c->getScore()){
                choiceGroups[curr.second]->removeChoiceByString(choiceString);
                choiceMap[choiceString] = std::pair<float,int>(c->getScore(), i);
              }
            }
            else{
              choiceMap[choiceString] = std::pair<float, int>(c->getScore(),i);
            }
          }
          choiceGroups.push_back(new ChoiceGroup(currChoices, std::pair<Choice*,Choice*>(choice1,choice2), score+prescore));
          i++;
        }
      }
    }
  }
  return new Label(choiceGroups);
}
