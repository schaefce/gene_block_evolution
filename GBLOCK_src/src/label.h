// Label.h: class for allowing labeled homologs in phylogenetic tree

#include <cfloat>
#include "choicegroup.h"

#ifndef LABEL_H
#define LABEL_H

class LabeledTree;

class Label{
public:
  Label();
  
  Label(vector<ChoiceGroup*> choiceGroups, float score=MAX_SCORE){
    setChoiceGroups(choiceGroups);
    finalScore = score;
    //this->isFinal = false;
  }
  
  

  void setChoiceGroups(vector<ChoiceGroup*> grps){
    resetChoiceGroups();
    addChoiceGroups(grps);
  }
  
  void setChoiceGroups(ChoiceGroup* grp){
    resetChoiceGroups();
    addChoiceGroups(grp);
  }

  void addChoiceGroups(vector<ChoiceGroup*> choicegroups){
    for (ChoiceGroup* cg : choicegroups){
      addChoiceGroups(cg);
    }
  }

  void addChoiceGroups(ChoiceGroup* choicegroup){
    choiceGroups.push_back(choicegroup);
    choicegroup->setLabel(this);
  }

  vector<ChoiceGroup*> getChoiceGroups(){
    return this->choiceGroups;
  }
  
  Choice* getBestChoice(){
    float min = MAX_SCORE;
    Choice* bestChoice = nullptr;
    for (ChoiceGroup* cg : this->choiceGroups){
      for (Choice* c : cg->getChoices()){
        if (c->getScore() < min){
          min = c->getScore();
          bestChoice = c;
        }
      }
    }
    return bestChoice;
  }
  
  bool hasFinal() const{
    return isFinal;
  }
  
  float getFinalScore() const{
    return finalScore;
  }
  
  void setFinalChoice(Choice* c){
    if (c) {
      isFinal = true;
      finalChoice = c;
      finalScore = c->getScore();
      setFinalLabel();
    }
  }
  
  void setFinalLabel(){
    if(finalChoice){
      std::stringstream ss;
      ss << finalChoice;
      finalLabel = ss.str();
    }
  }
  
  std::string getFinalLabel() const{
    if (isFinal)
      return finalLabel;
    else
      return "";
  }
  
  
  std::vector<std::string> getStringChoiceGroups() const{
    std::vector<std::string> stringGroups;
    std::transform(choiceGroups.begin(), choiceGroups.end(), stringGroups.begin(), [](ChoiceGroup &cg){ std::stringstream ss; ss << cg; return ss.str(); });
    return stringGroups;
  }

  static Label* createLeafLabel(std::vector<string> splitGroups){
    Choice* c = new Choice(splitGroups);
    ChoiceGroup* cg = new ChoiceGroup(vector<Choice*> {c});
    Label* l = new Label();
    l->addChoiceGroups(cg);
    return l;
  }
  
  friend std::ostream& operator<<(std::ostream &strm, const Label &l){
    if (l.hasFinal())
      return strm << "Final Label: " << l.getFinalLabel() << "," << l.getFinalScore();
    else
      return strm << "Label:\n" << join(l.getStringChoiceGroups(), '\n');
  }


private:
  void resetChoiceGroups(){
    if (!choiceGroups.empty()){
      choiceGroups.clear();
    }
  }
  
  //LabeledTree* clade;
  vector<ChoiceGroup*> choiceGroups;
  bool isFinal;
  Choice* finalChoice;
  float finalScore;
  std::string finalLabel;

};

#endif