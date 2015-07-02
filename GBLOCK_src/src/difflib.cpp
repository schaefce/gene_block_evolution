//
//  difflib.cpp
//  GeneBlock
//
//  Created by Schaeffer, Charlotte Elizabeth Ms. on 6/19/15.
//  Copyright (c) 2015 Charlotte Schaeffer. All rights reserved.
//


#include "difflib.h"
#include <iostream>


static float DUP_PENALTY, DEL_DUP_PENALTY, SPLIT_PENALTY, DEL_SPLIT_PENALTY, DEL_PENALTY, INS_PENALTY, MISMATCH_PENALTY, MATCH_PENALTY;
static std::string SPLIT;
static bool allowInsertions;


void LabelMatcher::initialize(){
  DEL_DUP_PENALTY = DUP_PENALTY = 5;
  SPLIT_PENALTY = DEL_SPLIT_PENALTY = 7;
  INS_PENALTY = DEL_PENALTY = 3;
  MISMATCH_PENALTY = 100000;
  MATCH_PENALTY = -1;
  SPLIT = "*";
}

template <typename T>
void printV(std::vector<T> toPrint, std::string name=""){
  if (!name.empty()){
    std::cout << name << ": ";
  }
  for(int i = 0; i < toPrint.size(); i++){
    std::cout << toPrint[i] << " ";
  }
  std::cout << std::endl;
}

void printScores(std::vector<std::pair<float,float>> toPrint, std::string name=""){
  if (!name.empty()){
    std::cout << name << ": ";
  }
  for(int i = 0; i < toPrint.size(); i++){
    std::cout << "(" << toPrint[i].first << ", " << toPrint[i].second << ") ";
  }
  std::cout << std::endl;
}

void printMatrix(float ** subproblems, long m, long n){
  std::cout << "\tSubproblem Matrix: ";
  for (int j = 0; j < n+1; j++){
    std::cout << "\t";
    for (int i = 0; i < m+1; i++){
      std::cout << subproblems[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

template <typename T>
void printVV(std::vector<std::vector<T>> toPrint){
  for(int i = 0; i < toPrint.size(); i++){
    printV<T>(toPrint[i]);
    //for(int j = 0; j < toPrint[i].size(); j++){
    //  std::cout << toPrint[i][j] << " ";
    //}
    //std::cout << std::endl;
  }
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
  float **subproblems = new float*[m+1];
  for (int i = 0; i < m+1; i++){
    subproblems[i] = new float[n+1];
  }
  
  for (int j = 0; j < n+1; j++){
    subproblems[0][j] = j * INS_PENALTY;
  }
  for (int i = 0; i < m+1; i++){
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
    performBacktrace(A, B, subproblems, intermediates);
  
  for(int i = 0; i < m+1; i++){
    delete [] subproblems[i];
  }
  delete [] subproblems;
  
  return penalty; //TODO: return penalty, intermediates;
}

std::vector<std::vector<std::string>> groupBy(std::vector<std::string> input, std::string item){
  std::vector<std::vector<std::string>> result;
  std::vector<std::string> addition;
  for(std::string currItem : input){
    if (currItem == item){
      result.push_back(addition);
      addition.clear();
    }
    else{
      addition.push_back(currItem);
    }
  }
  if(!addition.empty()){
    result.push_back(addition);
  }
  return result;
}

void LabelMatcher::performBacktrace(stringVector A, stringVector B, float **subproblems, intermediatesVector &intermediates){
  /**
    * Perform backtrace to determine what alignment of A and B produced lowest penalty in subproblems
    */

  splitGroupVector splits;
  long j = A.size(); //j = n
  long aj = j - 1;
  long i = B.size(); //i = m
  long bi = i - 1;
  while (i > 0 || j > 0){
    splitGroup currIntermediate;
    float pos = subproblems[i][j];
    
    if (i > 0 && j > 0 & pos == subproblems[i-1][j-1] + MATCH_PENALTY){ //case1_match){
      currIntermediate.push_back(new splitGroupPiece(B[bi], MATCH_PENALTY));
      bi -= 1;
      aj -= 1;
      i -= 1;
      j -= 1;
    }
    else if (j > 1 && pos == subproblems[i][j-2] + DUP_PENALTY + SPLIT_PENALTY) {
      currIntermediate.push_back(new splitGroupPiece((std::vector<std::string>) {A[aj],A[aj-1]}, DEL_DUP_PENALTY + DEL_SPLIT_PENALTY));
      currIntermediate.push_back(new splitGroupPiece(A[aj], DEL_DUP_PENALTY + SPLIT_PENALTY));
      currIntermediate.push_back(new splitGroupPiece(A[aj-1], DUP_PENALTY + DEL_SPLIT_PENALTY));
      currIntermediate.push_back(new splitGroupPiece(DUP_PENALTY + SPLIT_PENALTY));
      j -= 2;
      aj -= 2;
    }
    else if (i > 0 && pos == subproblems[i-1][j] + DEL_DUP_PENALTY){ //case2_del_dup){
      currIntermediate.push_back(new splitGroupPiece(B[bi], DEL_DUP_PENALTY));
      currIntermediate.push_back(new splitGroupPiece(DUP_PENALTY));
      i -= 1;
      bi -= 1;
    }
    else if (i > 0 && pos == subproblems[i-1][j] + DEL_SPLIT_PENALTY){ //case2_del_split){
      currIntermediate.push_back(new splitGroupPiece(B[bi], DEL_SPLIT_PENALTY));
      currIntermediate.push_back(new splitGroupPiece(SPLIT_PENALTY));
      i -= 1;
      bi -= 1;
    }
    else if (i > 0 && pos == subproblems[i-1][j] + DEL_PENALTY){ //case2_del_gen){
      currIntermediate.push_back(new splitGroupPiece(B[bi],DEL_PENALTY));
      
      if (allowInsertions){
        currIntermediate.push_back(new splitGroupPiece(INS_PENALTY)); //IF ALLOWED TO HAVE SUDDEN INSERTS
      }
      i -= 1;
      bi -= 1;
    }
    else if (j > 0 && pos == subproblems[i][j-1] + DUP_PENALTY){ //case3_ins_dup) {
      currIntermediate.push_back(new splitGroupPiece(A[aj],DEL_DUP_PENALTY));
      currIntermediate.push_back(new splitGroupPiece(DUP_PENALTY));
      j -= 1;
      aj -= 1;
    }
    
    else if (j > 0 && pos == subproblems[i][j-1] + SPLIT_PENALTY){ //case3_ins_split) {
      currIntermediate.push_back(new splitGroupPiece(A[aj],DEL_SPLIT_PENALTY));
      currIntermediate.push_back(new splitGroupPiece(SPLIT_PENALTY));
      j -= 1;
      aj -= 1;
    }
    else if (j > 0 && pos == subproblems[i][j-1] + INS_PENALTY){ //case3_ins_gen) {
      currIntermediate.push_back(new splitGroupPiece(A[aj], DEL_PENALTY));
      if (allowInsertions){
        currIntermediate.push_back(new splitGroupPiece(INS_PENALTY)); //IF ALLOWED TO HAVE SUDDEN INSERTS
      }
      j -= 1;
      aj -= 1;
    }
    splits.push_back(currIntermediate);
  }
  std::reverse(splits.begin(), splits.end());
  
  splitGroupVector bestSplitGroups = crossProduct(splits);
  
  for (splitGroup sg : bestSplitGroups){
    stringVector parts;
    float score = 0;
    for (splitGroupPiece* p: sg){
      for (std::string s : p->piece){
        if (s != "") parts.push_back(s);
      }
      score += p->score;
    }
    ssVector splitsV = groupBy(parts, SPLIT);
    Intermediate* inter = new Intermediate(splitsV,score);
    intermediates.push_back(inter);
  }
}



splitGroupVector LabelMatcher::crossProduct(splitGroupVector input){
  struct Iters {
    splitGroup::const_iterator begin;
    splitGroup::const_iterator end;
    splitGroup::const_iterator me;
  };
  
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

stringVector flattenAndAddDelim(ssVector target, std::string delim){
  stringVector ret;
  for(ssVector::iterator it = target.begin(); it != target.end(); ++it){
    for(std::string s : (*it)){
      ret.push_back(s);
    }
    ret.push_back(delim);
  }
  ret.pop_back();
  return ret;
}


float LabelMatcher::getMinEditDistance(ssVector choice1, ssVector choice2, intermediatesVector &intermediates, bool backtrace){
  float currMin = std::numeric_limits<float>::max();
  std::pair<stringVector,stringVector> bestCombo;
  std::string escapedSplit ="\\" + SPLIT;

  std::vector<std::pair<float,float>> scores;
  
  do {
    stringVector A = flattenAndAddDelim(choice1, SPLIT);//delimSplit(join<std::string>(choice1, SPLIT), escapedSplit, true); //"\\W+");
    A.erase(std::remove_if(A.begin(), A.end(), [](std::string s){ return s == "," || s == ""; }), A.end());
    do {
      std::pair<float,float> currScores;
      stringVector B = flattenAndAddDelim(choice2, SPLIT);//delimSplit(join<std::string>(choice2, SPLIT), escapedSplit, true); //"\\W+");
      B.erase(std::remove_if(B.begin(), B.end(), [](std::string s){ return s == "," || s == ""; }), B.end());
      intermediatesVector currIntermediates;
      float score = getEditDistance(A, B, currIntermediates, false);
      currScores.first = score;
      if (score < currMin){
        currMin = score;
        bestCombo.first = A;
        bestCombo.second = B;
      }
      score = getEditDistance(B, A, currIntermediates, false);
      currScores.second = score;
      if (score < currMin){
        currMin = score;
        bestCombo.first = B;
        bestCombo.second = A;
      }
      scores.push_back(currScores);
    } while(std::next_permutation(choice2.begin(),choice2.end()));
  } while(std::next_permutation(choice1.begin(),choice1.end()));

  
  if (backtrace){
    getEditDistance(bestCombo.first, bestCombo.second, intermediates, true);
  }
  return currMin;
}


Label* LabelMatcher::getAncestorLabel(Label *L1, Label *L2){
  std::map<std::string, std::pair<float,int>> choiceMap;
  std::vector<ChoiceGroup*> choiceGroups;
  int i = 0;
  float prescore;
  if(L1 && L2){
    for (ChoiceGroup* grp1 : L1->getChoiceGroups()){
      for (ChoiceGroup* grp2 : L2->getChoiceGroups()){
        for (Choice* choice1 : grp1->getChoices()){
          ssVector c1 = choice1->getGroups();
          //c1.erase(std::remove_if(c1.begin(), c1.end(), [](std::string s){ return s == ""; }), c1.end());
        
          for (Choice* choice2 : grp2->getChoices()){
            ssVector c2 = choice2->getGroups();
            //c2.erase(std::remove_if(c2.begin(), c2.end(), [](std::string s){ return s == ""; }), c2.end());
            prescore = choice1->getScore() + choice2->getScore();
            intermediatesVector intermediates;
            float score = getMinEditDistance(c1, c2, intermediates, true);
            std::vector<Choice*> currChoices;
            currChoices.reserve(intermediates.size());
            std::for_each(intermediates.begin(),intermediates.end(),
                        [&currChoices, prescore](const Intermediate *i)
                          { currChoices.push_back(new Choice((std::vector<std::vector<std::string>>)i->splits, i->score + prescore)); });
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
  else if(L1 || L2){
    return L1 ? new Label(L1->getChoiceGroups()) : new Label(L2->getChoiceGroups());
  }
  else{
    return (Label*)NULL;
  }
}
