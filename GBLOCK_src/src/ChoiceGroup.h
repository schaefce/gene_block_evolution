#include "Choice.h"

class Label;

class ChoiceGroup {

public:

    ChoiceGroup(Label* l, vector<Choice*> choices, pair<Choice*, Choice*> childChoices, float score=MAX_SCORE){
        groupLabel = l;
        groupChoices = choices;
        children = childChoices;
        groupScore = score;
    }

    ChoiceGroup(Label* l, vector<Choice*> choices, pair<Choice*, Choice*> childChoices){
        groupLabel = l;
        groupChoices = choices;
        children = childChoices;
        groupScore = MAX_SCORE;
    }





    Label* groupLabel;
    vector<Choice*> groupChoices;
    pair<Choice*, Choice*> childChoices;
    float groupScore;


    def __init__(self, label=None, choices=None, children=None,  overall_score=MAX_SCORE):
        self.set_choices(choices)
        self.children = children
        self.overall_score = overall_score
        self.label = label

    def set_choices_from_dict(cls, choice_dict=None):
        self.choices = {}#[]
        for c in choice_dict:
            c2 = Choice(groups=c, choice_group=self, score=choice_dict[c])
            self.add_choice(c2)
            #self.choices.append(Choice(groups=c, choice_group=self, score=choice_dict[c]))

    def set_choices(self, choices):
        if choices:
            self.choices = {}#[]
            if type(choices) is list:
                for c in choices:
                    #self.choices.append(c)
                    self.add_choice(c)
                    c.choice_group = self
            else:
                #self.choices.append(choices)
                self.add_choice(choices)
                choices.choice_group = self

    def add_choice(self, choice):
        choice_str = choice.group_list_string()
        if choice_str in self.choices:
            curr = self.choices[choice_str]
            if choice.score < curr.score:
                self.choices[choice_str] = choice
        else:
            self.choices[choice_str] = choice


    def set_score(self, score):
        if score:
            self.overall_score = score

    def remove_by_str(self, choice_str):
        if choice_str in self.choices:
            del self.choices[choice_str]


    def get_choices_string(self):
        return '\n'.join('\t{}. {}'.format(i, str(k)) for i,k in enumerate(self.choices))

    def __str__(self):
        return 'Choice Group: (overall score: {})\n{}'.format(self.overall_score, self.get_choices_string())

};
