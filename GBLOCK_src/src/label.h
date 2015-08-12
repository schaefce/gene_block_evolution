// label.h is part of GENEBLOCKER.
//
// GENEBLOCKER is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GENEBLOCKER is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GENEBLOCKER.  If not, see <http://www.gnu.org/licenses/>.

// Created by Carly Schaeffer and John Karro

#include <cfloat>
#include "choicegroup.h"

#ifndef LABEL_H
#define LABEL_H

class LabeledTree;

class Label{
public:
  Label();
  
  Label(std::vector<ChoiceGroup*> choiceGroups, float score=MAX_SCORE){
    setChoiceGroups(choiceGroups);
    finalScore = score;
    isFinal = false;
  }
  
  Label(std::vector<std::string> splitGroups, float score=MAX_SCORE, bool isLeaf=false){
    Choice* c = new Choice(splitGroups);
    if(isLeaf){
      c->setScore(0);
    }
    ChoiceGroup* cg = new ChoiceGroup(c);
    if(isLeaf){
      cg->setScore(0);
    }
    setChoiceGroups(cg);
    if(isLeaf){
      setFinalChoice(c);
    }
  }
  
  Label(const Label &other){
    setChoiceGroups(other.copyChoiceGroups());
    if (other.isFinal){
      setFinalChoice(other.copyFinalChoice());
    }
  }
  
  Label& operator= (const Label &other){
    setChoiceGroups(other.copyChoiceGroups());
    if (other.isFinal){
      setFinalChoice(other.copyFinalChoice());
    }
    return *this;
  }
  
  ~Label(){
    for (ChoiceGroup* cg : getChoiceGroups()){
      delete cg;
    }
    if (hasFinal()){
      delete finalChoice;
    }
  }

  static Label* createLeafLabel(std::vector<std::string> splitGroups){
    
    Choice* c = new Choice(splitGroups);
    ChoiceGroup* cg = new ChoiceGroup(std::vector<Choice*> {c});
    Label* l = new Label();
    l->addChoiceGroups(cg);
    return l;
  }
  
  void setChoiceGroups(std::vector<ChoiceGroup*> grps){
    resetChoiceGroups();
    addChoiceGroups(grps);
  }
  
  void setChoiceGroups(ChoiceGroup* grp){
    resetChoiceGroups();
    addChoiceGroups(grp);
  }

  void addChoiceGroups(std::vector<ChoiceGroup*> choicegroups){
    for (ChoiceGroup* cg : choicegroups){
      addChoiceGroups(cg);
    }
  }

  void addChoiceGroups(ChoiceGroup* choicegroup){
    choiceGroups.push_back(choicegroup);
    choicegroup->setLabel(this);
  }

  std::vector<ChoiceGroup*> getChoiceGroups(){
    return this->choiceGroups;
  }
  
  std::vector<ChoiceGroup*> copyChoiceGroups() const{
    std::vector<ChoiceGroup*> copy;
    for(ChoiceGroup* cg : choiceGroups){
      copy.push_back(new ChoiceGroup(*cg));
    }
    return copy;
  }
  
  Choice* getBestChoice(){
    float min = MAX_SCORE;
    Choice* bestChoice = (Choice*)NULL;
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
  
  bool hasFinal() const { return isFinal; }
  float getFinalScore() const{ return finalScore; }
  
  void setFinalChoice(Choice* c){
    if (c) {
      isFinal = true;
      finalChoice = c;
      finalScore = c->getScore();
      setFinalLabel();
    }
  }
  
  Choice* copyFinalChoice() const{ return new Choice(*finalChoice); }
  
  void setFinalLabel(){
    if(finalChoice){
      std::stringstream ss;
      ss << *(finalChoice);
      finalLabel = ss.str();
    }
  }
  
  std::string getFinalLabel() const{
    if (isFinal)
      return finalLabel;
    else
      return "";
  }
  
  std::string formatted() const{
    if (isFinal){
      return finalChoice->formatted();
    }
    return "";
  }
  
  std::vector<std::string> getStringChoiceGroups() const{
    std::vector<std::string> stringGroups;
    for(ChoiceGroup* cg : choiceGroups){
      std::stringstream ss;
      ss << *cg;
      stringGroups.push_back(ss.str());
    }
    return stringGroups;
  }
  
  friend std::ostream& operator<<(std::ostream &strm, const Label &l){
    if (l.hasFinal())
      return strm << "Final Label: " << l.getFinalLabel() << "," << l.getFinalScore();
    else
      return strm << "Label:\n" << join<std::string>(l.getStringChoiceGroups(), "\n");
  }

private:
  void resetChoiceGroups(){
    if (!choiceGroups.empty()){
      choiceGroups.clear();
    }
  }
  
  std::vector<ChoiceGroup*> choiceGroups;
  bool isFinal;
  Choice* finalChoice;
  float finalScore;
  std::string finalLabel;

};



#endif
