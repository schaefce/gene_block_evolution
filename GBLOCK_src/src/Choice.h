
#include <limits>
using namespace std;

const float MAX_SCORE = std::numeric_limits<float>::max();

class ChoiceGroup;

class Choice {
public:

    Choice(ChoiceGroup* cg, vector<string> groups, float score=MAX_SCORE){
        splitGroups = groups;
        choiceGroup = cg;
        choiceScore = score;
    }


    void setGroups(vector<ChoiceGroup*> grps){
        splitGroups = grps;
    }




    vector<ChoiceGroup*> splitGroups;
    float choiceScore;
    ChoiceGroup* choiceGroup;

};



    def set_groups(self, grps):
        self.groups = grps

    def get_children(self):
        if self.choice_group:
            return self.choice_group.children
        else:
            return None

    def get_label(self):
        #print(self.choice_group)
        return self.choice_group.label

    def with_str_groups(self):
        return [','.join(group) if type(group) is list else group for group in self.groups]

    def group_list_string(self):
        return '[{}]'.format(','.join('({})'.format(','.join(c for c in grp)) for grp in self.groups))

    def __str__(self):
        return 'Choice (score= {}): {}'.format(self.score, self.group_list_string())
        #'[' + ','.join('(' + ','.join(c for c in grp) + ')' for grp in self.groups) + ']'

    def __repr__(self):
        return self.__str__()

class Choice {
public:
    Choice(ChoiceGroup* cg, vector<string> groups, float score){
        splitGroups = groups;   //self.groups = groups
        cScore = score;         //self.score = score
        cGroup = cg;            //self.choice_group = choice_group
    }


    Choice(ChoiceGroup* cg, vector<string> groups, float score){
        splitGroups = groups;   //self.groups = groups
        cScore = score;         //self.score = score
        cGroup = cg;            //self.choice_group = choice_group
    }

    void setGroups(){   //def set_groups(self, grps):
        //self.groups = grps
    }

    children getChildren(){ //get_children(self):
        if (cGroup) //self.choice_group:
            return cGroup->getChildren();//self.choice_group.children
        else
            return 0;
    }

    string getLabel(){//def get_label(self):
        //#print(self.choice_group)
        return cGroup->getLabel();
    }

    vector<string> getStringGroups(){//def with_str_groups(self):
        vector<string> stringGroups;
        stringGroups.resize(splitGroups.size()*2);
        return transform(stringGroups.begin(), stringGroups.end(), stringGroups.begin(), joinByComma);
        //return transform(splitGroups.begin(), splitGroups.end(), joinByComma);
        //return [','.join(group) if type(group) is list else group for group in self.groups]
    }

    string groupListString(){//def group_list_string(self):
        vector<string> stringGroups = getStringGroups();
        stringstream s;
        copy(

        return sprintf("[$s]", transform(split ))
        return '[{}]'.format(','.join('({})'.format(','.join(c for c in grp)) for grp in self.groups))
    }

    //def __str__(self):
        //return 'Choice (score= {}): {}'.format(self.score, self.group_list_string())
        //#'[' + ','.join('(' + ','.join(c for c in grp) + ')' for grp in self.groups) + ']'

    //def __repr__(self):
        //return self.__str__()

    cgList* splitGroups;
    float cScore;
    ChoiceGroup* cGroup;

};
