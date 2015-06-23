#include "choice.h"

class Label;

class ChoiceGroup {

public:
  ChoiceGroup(vector<Choice*> choices, pair<Choice*, Choice*> childChoices, float score=MAX_SCORE){
    //groupLabel = l;
    setChoices(choices);
    children = childChoices;
    score = score;
  }

  ChoiceGroup(vector<Choice*> choices){
    setChoices(choices);
    //this->children = new pair<(Choice*)NULL, (Choice*)NULL>;
  }


  void setChildren(std::pair<Choice*, Choice*> childChoices){
    children = childChoices;
  }

  std::pair<Choice*,Choice*> getChildren() {
    return children;
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

  void setChoices(vector<Choice*> choiceV){
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
    string cString = choice->groupListString();
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
    transform(choiceMap.begin(), choiceMap.end(), choiceV.begin(), [](std::pair<std::string, Choice*> &p){ return p.second; });
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
    for(std::map<string,Choice*>::const_iterator it = choiceMap.begin(); it != choiceMap.end(); ++it){
      long i = std::distance(choiceMap.begin(), it);
      ss << '\t';
      ss << i;
      ss << ". ";
      ss << it->second;
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
  std::pair<Choice*, Choice*> children;
  float score;

};
