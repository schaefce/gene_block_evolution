
#include <limits>
#include <algorithm>
#include <string>
#include <vector>
#include "utility.h"

const float MAX_SCORE = std::numeric_limits<float>::max();

class Label;

class ChoiceGroup;

class Choice {
public:
  Choice(std::vector<std::vector<string>> groups, float score=MAX_SCORE){
    setGroups(groups);
    this->choiceScore = score;
  }
  
  Choice(std::vector<string> groups, float score=MAX_SCORE){
    setGroups(groups);
    this->choiceScore = score;
  }
  
  void setChoiceGroup(ChoiceGroup* cg){
    choiceGroup = cg;
  }
  
  ChoiceGroup* getChoiceGroup(){
    return choiceGroup;
  }

  void setGroups(std::vector<std::vector<string>> grps){
    this->splitGroups = grps;
  }

  void setGroups(std::vector<string> grps){
    for(std::string s : grps){
      this->splitGroups.push_back({s});
    }
  }
  
  std::vector<vector<string>> getGroups(){
    return this->splitGroups;
  }
  
  void setScore(float score){
    this->choiceScore = score;
  }
  
  float getScore() const{
    return this->choiceScore;
  }
  
  //std::pair<Choice*, Choice*> getChildren(){
  //  if(this->choiceGroup){ return this->choiceGroup->getChildren(); }
  //  else{ return std::pair<Choice*, Choice*>((Choice*)NULL, (Choice*)NULL); }}
  

  std::vector<std::string> getStringGroups() const{
    
    vector<string> stringGroups;
    stringGroups.resize(splitGroups.size()*2);
    std::transform(splitGroups.begin(), splitGroups.end(), stringGroups.begin(), [](vector<string> v){ return join<string>(v,','); });
    return stringGroups;
    
    //std::vector<std::string> stringGroupV;
    
    //for (std::vector<std::vector<std::string>>::iterator it = splitGroups.begin(); it != splitGroups.end(); ++it){
    //  stringGroupV.push_back(join<string>((*it),','));
    //}
    //return stringGroupV;
  }
  
  std::string groupListString() const {
    std::vector<std::string> stringGroups = getStringGroups();
    stringGroups.resize(stringGroups.size()*3);
    std::transform(stringGroups.begin(), stringGroups.end(), stringGroups.begin(), [](string s){ return formatEnds(s, '(', ')'); }); //'(' + s + ')'; });
    return formatEnds(join<string>(stringGroups, ','), '[', ']');//'[' + join<string>(stringGroups,',') + ']';
  }
  
  friend std::ostream& operator<<(std::ostream &strm, const Choice &c){
    return strm << "Choice (score " << c.getScore() << "): " << c.groupListString();
  }

private:
  std::vector<std::vector<string>> splitGroups;
  //vector<string> splitGroups;
  float choiceScore;
  ChoiceGroup* choiceGroup;
};

