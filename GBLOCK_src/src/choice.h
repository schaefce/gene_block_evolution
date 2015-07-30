
#include <limits>
#include <algorithm>
#include <iostream>
#include "utility.h"

#ifndef CHOICE_H
#define CHOICE_H

const float MAX_SCORE = std::numeric_limits<float>::max();
using ssVector = std::vector<std::vector<std::string>>;

class Label;
class ChoiceGroup;

class Choice {
public:
  Choice(ssVector groups, float score=MAX_SCORE){
    setGroups(groups);
    setScore(score);
  }
  
  Choice(std::vector<std::string> groups, float score=MAX_SCORE){
    setGroups(groups);
    setScore(score);
  }
  
  Choice(const Choice &other){
    setGroups(other.getGroups());
    setScore(other.getScore());
  }
  
  ChoiceGroup* getChoiceGroup(){ return choiceGroup; }
  void setChoiceGroup(ChoiceGroup* cg){ choiceGroup = cg; }
  
  ssVector getGroups() const{ return this->splitGroups; }
  void setGroups(ssVector grps){ splitGroups = grps; }
  void setGroups(std::vector<std::string> grps){
    try{
      std::regex re("\\s+");
      for(std::string s : grps){
        if (!std::regex_match (s,re)){
          this->splitGroups.push_back({s});
        }
      }
    } catch (std::regex_error& e) {
      std::cerr << e.code();
    }
  }
  
  float getScore() const { return this->choiceScore; }
  void setScore(float score){ this->choiceScore = score; }
  
  
  std::vector<std::string> getStringGroups() const{
    std::vector<std::string> stringGroups;
    std::for_each(splitGroups.begin(),splitGroups.end(),
                  [&stringGroups](const ssVector::value_type& p)
                  { stringGroups.push_back(join<std::string>(p,",")); });
    return stringGroups;
  }
  
  std::string groupListString() const {
    std::vector<std::string> stringGroups = getStringGroups();
    std::transform(stringGroups.begin(), stringGroups.end(), stringGroups.begin(), [](std::string s){ return formatEnds(s, "(", ")"); });
    return formatEnds(join<std::string>(stringGroups, ","), "[", "]");
  }
  
  std::string formatted() const {
    std::vector<std::string> stringGroups;
    std::for_each(splitGroups.begin(),splitGroups.end(),
                  [&stringGroups](const ssVector::value_type& p)
                  { stringGroups.push_back(join<std::string>(p,".")); });
    
    std::stringstream ss;
    ss << join<std::string>(stringGroups, "*");
    ss << "|" << choiceScore;
    return ss.str();
  }
  
  friend std::ostream& operator<<(std::ostream &strm, const Choice &c){
    return strm << "Choice (score " << c.getScore() << "): " << c.groupListString();
  }
  
private:
  ssVector splitGroups;
  float choiceScore;
  ChoiceGroup* choiceGroup;
};

#endif
