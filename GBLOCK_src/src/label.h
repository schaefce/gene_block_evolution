// Label.h: class for allowing labeled homologs in phylogenetic tree

#include "choicegroup.h"

#ifndef LABEL_H
#define LABEL_H

class LabeledTree;

class Label{
public:
  Label();
  //Label(LabeledTree* t);
  void setChoiceGroups()


private:
  //LabeledTree* clade;
  vector<ChoiceGroup*> choiceGroups;
  bool isFinal;
  Choice* finalChoice;
  float finalScore;

}

#endif


    def __init__(self, clade=None, choice_groups=None, final_choice=None, final_score=MAX_SCORE):
        self.clade = clade
        self.set_choice_groups(choice_groups)
        self.is_final = False
        self.set_final_choice(final_choice)
        self.final_score = final_score

    @classmethod
    def for_leaf(cls, clade, group_list=None):
        c = Choice(groups=group_list, score=0)
        g = Choice_Group(choices=c, overall_score=0)
        #c.choice_group = g
        l = Label(clade=clade, choice_groups=g, final_choice=c)
        #g.label = l
        return l

    def set_choice_groups(self, choice_groups):
        if choice_groups:
            self.choice_groups = []
            if type(choice_groups) is list:
                for cg in choice_groups:
                    self.choice_groups.append(cg)
                    cg.label = self
            else:
                self.choice_groups.append(choice_groups)
                choice_groups.label = self

    def add_choice_group(self, choice_group):
        if type(choice_group) is list:
            for cg in choice_groups:
                self.choice_groups.append(cg)
                cg.label = self
        else:
            self.choice_groups.append(choice_group)
            choice_groups.label = self


    def get_best_choice(self):
        min_score = float("inf")
        best_choice=None
        for cg in self.choice_groups:
            for c in cg.choices:
                if c.score < min_score:
                    min_score = c.score
                    best_choice = c
        return best_choice

    #def set_best_choice_as_final(self):
    #    best = self.get_best_choice()


    def set_final_choice(self, choice):
        if choice:
            self.final_choice = choice
            self.is_final = True
            self.set_final_label()
            self.final_score = self.final_choice.score
        else:
            self.is_final = False
            self.final_label = None
            self.final_score = MAX_SCORE

    def set_final_label(self):
        self.final_label = str(self.final_choice)
        self.clade.comment = self.final_label

    def get_final_label(self):
        if self.is_final:
            return self.final_label

    def __str__(self):
        if self.is_final:
            return 'Final Label:{lab},{score}'.format(lab=self.get_final_label(), score=self.score)
        else:
            return 'Label:\n' + '\t\n'.join('{}. {}'.format(i, str(k)) for i,k in enumerate(self.choice_groups))
