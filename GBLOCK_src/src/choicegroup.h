#include "choice.h"
#include <map>

#ifndef CHOICE_GROUP_H
#define CHOICE_GROUP_H

class Label;

class ChoiceGroup {

public:
  ChoiceGroup(std::vector<Choice*> choices, std::pair<Choice*, Choice*> parentChoices, float score=MAX_SCORE){
    setChoices(choices);
    setParents(parentChoices);
    score = score;
  }

  ChoiceGroup(std::vector<Choice*> choices, float score=MAX_SCORE){
    setChoices(choices);
  }
  
  ChoiceGroup(Choice* choice){
    setChoices((std::vector<Choice*>){choice});
  }
  
  ChoiceGroup(const ChoiceGroup &other){
    setChoices(other.copyChoices());
    setScore(other.getScore());
    setParents(other.copyParents());
  }
  
  ChoiceGroup& operator= (const ChoiceGroup &other){
    setChoices(other.copyChoices());
    setScore(other.getScore());
    setParents(other.copyParents());
    return *this;
  }
  
  ~ChoiceGroup(){
    for (Choice* c : getChoices()){
      delete c;
    }
    if (hasParents()){
      delete parents.first;
      delete parents.second;
    }
  }

  void setParents(std::pair<Choice*, Choice*> parentChoices){
    parents = parentChoices;
  }
  
  std::pair<Choice*,Choice*> getParents() {
    return parents;
  }
  
  std::pair<Choice*,Choice*> copyParents() const{
    if (hasParents()){
      return (std::pair<Choice*,Choice*>) {new Choice(*parents.first), new Choice(*parents.second)};
    }
    else{
      return (std::pair<Choice*,Choice*>) {(Choice*)NULL, (Choice*)NULL};
    }
  }
  
  bool hasParents() const {
    return parents.first && parents.second;
  }

  void setLabel(Label* l){
    groupLabel = l;
  }

  Label* getLabel(){
    return groupLabel;
  }

  float getScore() const {
    return score;
  }

  void setScore(float score){
    this->score = score;
  }
  

  void setChoices(std::vector<Choice*> choiceV){
    if(!choiceMap.empty()){
      choiceMap.clear();
    }
    for (Choice* c : choiceV){
      addChoice(c);
    }
  }

  void setChoices(std::map<std::vector<std::string>,float> choiceDict){
    if(!choiceMap.empty()){
      choiceMap.clear();
    }
    for (std::map<std::vector<std::string>,float>::iterator it = choiceDict.begin(); it != choiceDict.end(); ++it){
      addChoice(new Choice(it->first, it->second));
    }
  }

  void addChoice(Choice* choice){
    std::string cString = choice->groupListString();
    if(choiceMap.count(cString)){
      Choice* curr = choiceMap[cString];
      if(choice->getScore() < curr->getScore()){
        choiceMap[cString] = choice;
      }
    }
    else{
      choiceMap[cString] = choice;
    }
    choiceMap[cString]->setChoiceGroup(this);
  }

  std::vector<Choice*> getChoices(){
    std::vector<Choice*> choiceV;
    choiceV.reserve(choiceMap.size());
    std::for_each(choiceMap.begin(),choiceMap.end(),
                  [&choiceV](const std::map<std::string,Choice*>::value_type& p)
                  { choiceV.push_back(p.second); });
    return choiceV;
  }
  
  std::vector<Choice*> copyChoices() const{
    std::vector<Choice*> choiceV;
    choiceV.reserve(choiceMap.size());
    std::for_each(choiceMap.begin(),choiceMap.end(),
                  [&choiceV](const std::map<std::string,Choice*>::value_type& p)
                  { choiceV.push_back(new Choice(*p.second)); });
    return choiceV;
  }

  bool removeChoiceByString(std::string cString){
    if(choiceMap.count(cString)){
      choiceMap.erase(cString);
      return true;
    }
    else{
      return false;
    }
  }

  std::string choicesString() const{
    std::stringstream ss;
    for(std::map<std::string,Choice*>::const_iterator it = choiceMap.begin(); it != choiceMap.end(); ++it){
      long i = std::distance(choiceMap.begin(), it);
      ss << '\t';
      ss << i;
      ss << ". ";
      ss << *(it->second);
      if (std::distance(it, choiceMap.end()) != 1){
        ss << '\n';
      }
    }
    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream &strm, const ChoiceGroup &cg){
    return strm << "Choice Group: (overall score " << cg.getScore() << "): " << cg.choicesString();
  }

private:
  Label* groupLabel;
  std::map<std::string,Choice*> choiceMap;
  std::pair<Choice*, Choice*> parents;
  float score;

};

#endif
