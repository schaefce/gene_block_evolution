//
//  difflib.cpp
//  GeneBlock
//
//  Created by Schaeffer, Charlotte Elizabeth Ms. on 6/19/15.
//  Copyright (c) 2015 Charlotte Schaeffer. All rights reserved.
//
#include "difflib.h"




LabelMatcher::LabelMatcher(){
  DEL_DUP_PENALTY = DUP_PENALTY = 5;
  SPLIT_PENALTY = DEL_SPLIT_PENALTY = 7;
  INS_PENALTY = DEL_PENALTY = 3;
  MISMATCH_PENALTY = 100000;
  MATCH_PENALTY = -1;
  SPLIT = '*';
}

float LabelMatcher::getDeletionPenalty(std::string A, std::string B, int j, int i){
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


float LabelMatcher::getInsertionPenalty(std::string A, std::string B, int j, int i){
  /**
   * Determine cost of inserting Aj after Bi.
   */
  if (j > 0 && A[j] == A[j-1] == B[i]){
    return DUP_PENALTY;
  }
  else if (A[j] == SPLIT){
    return SPLIT_PENALTY;
  }
  else {
    return INS_PENALTY;
  }
}

float LabelMatcher::getMatchPenalty(std::string A, std::string B, int j, int i){
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

float LabelMatcher::getSpecialSplitPenalty(std::string A, std::string B, int j, int i){
  /**
   * Determine
   */
  if (j > 1 && B[i] == A[j] == A[j-2] && A[j-1] == SPLIT){
    return DUP_PENALTY + SPLIT_PENALTY;
  }
  else {
    return MISMATCH_PENALTY;
  }
}

float LabelMatcher::getEditDistance(std::string A, std::string B){
  int n = A.length();
  int m = B.length();

  vector<vector<int>> subproblems;
  
  
  int **subproblems = new int*[m+1];
  for (int i = 0; i < m+1; i++){
    subproblems[i] = new int[n+1];
  }
  
  fill(subproblems,)
  
//# subproblems[i][j] will represent the edit distance from B[...i] to A[...j]
subproblems = [[0 for x in range(n+1)] for x in range(m+1)]

for j in range(n+1):
subproblems[0][j] = j * self.INS_PENALTY
for i in range(m+1):
subproblems[i][0] = i * self.DEL_PENALTY
for j in range(1, n+1):
aj = j - 1
for i in range(1, m+1):
bi = i - 1
# Bi and Aj match
case1 = subproblems[i-1][j-1] + self.match_penalty(A, B, aj, bi)
# Delete Bi
case2 = subproblems[i-1][j] + self.deletion_penalty(A, B, aj, bi)
# Insert Aj at Bi
case3 = subproblems[i][j-1] + self.insertion_penalty(A, B, aj, bi)
if j > 1:
# Insert a Aj-1 and Aj (Aj matches Aj-2 (a dup) and Aj-1 is a split)
case4 = subproblems[i][j-2] + self.special_split_penalty(A, B, aj, bi)
else:
case4 = float("inf")
subproblems[i][j] = min([case1, case2, case3, case4])
self.logger.debug("\t\t\t\t\tHere is the matrix:")
for i in range(m+1):
self.logger.debug("\t\t\t\t\t\t" + str(subproblems[i]))

#for i in range(m):
#    for j in range(n):
#        print '{:4}'.format(subproblems[i][j]),
#    print

penalty = subproblems[m][n]

self.logger.debug("\t\t\t\t\tEdit distance between {c1} and {c2}:\t{p}".format(c1=str(''.join(A)),c2=str(''.join(B)),p=str(penalty)))
intermediates= self.perform_backtrace(A, B, subproblems, allow_insertions) if backtrace else None
return penalty, intermediates


def perform_backtrace(self, A, B, subproblems, allow_insertions=False):
self.logger.debug("\t\t\t\t\tPerforming backtrace for {c1} and {c2}".format(c1=str(''.join(A)),c2=str(''.join(B))))
self.logger.debug("A: {Alist}\nB: {Blist}".format(Alist=str(A), Blist=str(B)))
answer = []
intermediates = []
scores = []

j = len(A) #j = n
aj = j - 1
i = len(B) #i = m
bi = i - 1

while i > 0 or j > 0:
self.logger.debug('\t\t\t\t\t\ti: '+str(i)+',j: '+ str(j))
curr_intermediate = []
curr_score = []
pos = subproblems[i][j]
case1_match = subproblems[i-1][j-1] + self.MATCH_PENALTY
case1_mismatch = subproblems[i-1][j-1] + self.MISMATCH_PENALTY
case2_del_dup = subproblems[i-1][j] + self.DEL_DUP_PENALTY
case2_del_split = subproblems[i-1][j] + self.DEL_SPLIT_PENALTY
case2_del_gen = subproblems[i-1][j] + self.DEL_PENALTY
case3_ins_dup = subproblems[i][j-1] + self.DUP_PENALTY
case3_ins_split = subproblems[i][j-1] + self.SPLIT_PENALTY
case3_ins_gen = subproblems[i][j-1] + self.INS_PENALTY

self.logger.debug('\t\t\t\t\t\tpos:{p} match:{c11} mmatch:{c12} ddup:{c21} dspl:{c22} del:{c23} idup:{c31} ispl:{c32} ins:{c33}'.format(p=pos,c11=case1_match,c12=case1_mismatch,c21=case2_del_dup,c22=case2_del_split,c23=case2_del_gen,c31=case3_ins_dup,c32=case3_ins_split,c33=case3_ins_gen))
if i > 0 and j > 0 and pos == case1_match:
curr_intermediate.append(B[bi])
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="MATCHES", bpos=bi))#+str(i)+',j: '+ str(j))
curr_score.append(self.MATCH_PENALTY)
bi -= 1
aj -= 1
i -= 1
j -= 1
elif j > 1 and pos == subproblems[i][j-2] + self.DUP_PENALTY + self.SPLIT_PENALTY:
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="NEEDS A SPLIT+DUP", bpos=bi))#+str(i)+',j: '+ str(j))
curr_intermediate.append([A[aj],A[aj-1]])
curr_score.append((self.DEL_DUP_PENALTY + self.DEL_SPLIT_PENALTY))
curr_intermediate.append(A[aj])
curr_score.append((self.DEL_DUP_PENALTY + self.SPLIT_PENALTY))
curr_intermediate.append(A[aj-1])
curr_score.append((self.DUP_PENALTY + self.DEL_SPLIT_PENALTY))
curr_intermediate.append('')
curr_score.append(self.DUP_PENALTY + self.SPLIT_PENALTY)
#answer.append( ('add_split', aj, j, i, i) )
#answer.append( ('add_dup', aj-1, aj, i, i) )
j -= 2
aj -= 2
elif i > 0 and pos == case2_del_dup:
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="HAS TO DELETE DUP FROM", bpos=bi))#+str(i)+',j: '+ str(j))
curr_intermediate.append(B[bi])
curr_score.append(self.DEL_DUP_PENALTY)
curr_intermediate.append('')
curr_score.append(self.DUP_PENALTY)
#answer.append( ('delete_dup', j, j, bi, i) )
i -= 1
bi -= 1
elif i > 0 and pos == case2_del_split:
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="HAS TO DELETE SPLIT FROM", bpos=bi))#+str(i)+',j: '+ str(j))
curr_intermediate.append(B[bi])
curr_score.append(self.DEL_SPLIT_PENALTY)
curr_intermediate.append('')
curr_score.append(self.SPLIT_PENALTY)
#answer.append( ('remove_split', j, j, bi, i) )
i -= 1
bi -= 1
elif i > 0 and pos == case2_del_gen:
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="HAS TO DELETE FROM", bpos=bi))#+str(i)+',j: '+ str(j))
curr_intermediate.append(B[bi])
curr_score.append(self.DEL_PENALTY)
if allow_insertions:
curr_intermediate.append('') #IF ALLOWED TO HAVE SUDDEN INSERTS
curr_score.append(self.INS_PENALTY)
#answer.append( ('delete', j, j, bi, i) )
i -= 1
bi -= 1
elif j > 0 and pos == case3_ins_dup:
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="HAS TO ADD DUP FROM", bpos=bi))#+str(i)+',j: '+ str(j))
curr_intermediate.append(A[aj])
curr_score.append(self.DEL_DUP_PENALTY)
curr_intermediate.append('')
curr_score.append(self.DUP_PENALTY)
#answer.append( ('insert_dup', aj, j, i, i) )
j -= 1
aj -= 1
elif j > 0 and pos == case3_ins_split:
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="HAS TO ADD SPLIT FROM", bpos=bi))#+str(i)+',j: '+ str(j))
curr_intermediate.append(A[aj])
curr_score.append(self.DEL_SPLIT_PENALTY)
curr_intermediate.append('')
curr_score.append(self.SPLIT_PENALTY)
#answer.append( ('add_split', aj, j, i, i) )
j -= 1
aj -= 1
elif j > 0 and pos == case3_ins_gen:
self.logger.debug('\t\t\t\t\t\tA at {apos} {case} B at {bpos}'.format(apos=aj, case="HAS TO ADD FROM", bpos=bi))
curr_intermediate.append(A[aj])
curr_score.append(self.DEL_PENALTY)
if allow_insertions:
curr_intermediate.append('') #IF ALLOWED TO HAVE SUDDEN INSERTS
curr_score.append(self.INS_PENALTY)
#answer.append( ('insert', aj, j, i, i) )
j -= 1
aj -= 1
intermediates.append(curr_intermediate)
scores.append(curr_score)
#print(intermediates)
#print(intermediates)
intermediates=intermediates[::-1]
scores=scores[::-1]#reversed(intermediates)
return intermediates, scores

def get_min_edit_distance(self, choice1, choice2, allow_insertions=False, backtrace=True):
#print(choice1)
#print(choice2)
self.logger.debug("\t\t\tGetting min edit distance between {c1} and {c2}".format(c1=str(''.join(choice1)),c2=str(''.join(choice2))))
curr_min = float("inf")
best_combo = None
#best_ops = None
best_intermediates = [] #None
best_scores = []
backwards = False
for A in itertools.permutations(choice1):
A = [i for i in re.split('(\W+)', SPLIT.join(A)) if i != ',']
#A = re.split('(\*)', SPLIT.join(A))
for B in itertools.permutations(choice2):
B = [i for i in re.split('(\W+)', SPLIT.join(B)) if i != ',']
#self.logger.debug("\t\t\t\t\tA:" + str(A) + "\tB:" + str(B))
#B = re.split('(\*)', SPLIT.join(B))
score, intermediates = self.get_edit_distance(A, B, allow_insertions, backtrace=False)
#self.logger.debug('\t\t\t\tScore={s}'.format(s=score))
#self.logger.debug(type(intermediates))
#if intermediates:
#    self.logger.debug('\t\t\t\tIntermediates:' + ',\t'.join(map(str, intermediates)))#[str(''.join(inter)) for inter in intermediates]))
if score < curr_min:
curr_min = score
best_combo = (A, B)
#best_ops = codes
#if intermediates:
#    best_intermediates = list(intermediates[0])
#    best_scores = list(intermediates[1])
#backwards = False
self.logger.debug("\t\t\t\tUpdated curr min:\t{m}".format(m=str(curr_min)))
#self.logger.debug("\t\t\t\tUpdated best intermediates:"+ ',\t'.join(map(str, best_intermediates)))
#self.logger.debug("\t\t\t\tUpdated best scores:"+ ',\t'.join(map(str, best_scores)))
score, intermediates = self.get_edit_distance(B, A, allow_insertions, backtrace=False)
#self.logger.debug('\t\t\t\tScore={s}'.format(s=score))
#if intermediates:
#    self.logger.debug('\t\t\t\tIntermediates:' + ',\t'.join(map(str, intermediates)))#[str(''.join(inter)) for inter in intermediates]))
if score < curr_min:
curr_min = score
best_combo = (B, A)
#best_ops = codes
#if intermediates:
#    best_intermediates = list(intermediates[0])
#    best_scores = list(intermediates[1])
#    #best_intermediates = list(intermediates)
#if scores:
#    best_scores = list(scores)
#backwards = True
self.logger.debug("\t\t\t\tUpdated curr min:\t{m}".format(m=str(curr_min)))
#self.logger.debug("\t\t\t\tUpdated best intermediates:"+ ',\t'.join(map(str, best_intermediates)))
#self.logger.debug("\t\t\t\tUpdated best scores:"+ ',\t'.join(map(str, best_scores)))
#best_intermediates = [(''.join(x)).split(SPLIT) for x in best_intermediates]
#best_intermediates = [filter(None, x) for x in best_intermediates]
if backtrace:
score, intermediates = self.get_edit_distance(best_combo[0], best_combo[1], allow_insertions)
#best_intermediates = list(intermediates[0])
#best_intermediates = list(itertools.product(*best_intermediates))
#best_scores = list(itertools.product(*best_scores))
#print(best_intermediates)
best_intermediates = list(itertools.product(*(list(intermediates[0]))))
best_scores = list(itertools.product(*(list(intermediates[1]))))
#print(best_intermediates)

for i in range(len(best_intermediates)):
new_inter = []
for l in best_intermediates[i]:
#print('l', l)
if type(l) is list:
l = [a for a in l if a is not '']
new_inter.extend(l)
elif l is not '':
new_inter.append(l)
#print(best_intermediates[i], new_inter)
best_intermediates[i] = new_inter

for i in range(len(best_scores)):
sum = 0
for l in best_scores[i]:
if type(l) is list:
sum += sum(l)# = [a for a in l if a is not '']
#new_score.extend(l)
else:
sum += l
best_scores[i] = sum
best_intermediates = [[list(group) for k, group in itertools.groupby(l, lambda x: x == "*") if not k] for l in best_intermediates]
best_intermediates = [filter(None, x) for x in best_intermediates]
self.logger.debug("\t\t\t\tBest intermediates:\t{b}".format(b=',\t'.join(map(str, best_intermediates))))

self.logger.debug("\t\t\t\tBest scores:\t{b}".format(b=',\t'.join(map(str, best_scores))))
#print()

#best_intermediates = [''.join(i) for i in best_intermediates]
#best_intermediates = []
return curr_min, best_combo, best_intermediates, best_scores


static Label* getAncestorLabel(Label* L1, Label* L2){  //def get_ancestor_label(self):
  cgList choiceGroups;    //choice_grps = []
  cMap choiceMap;        //choice_map = {}
  int i = 0;
  for (ChoiceGroup* grp1 : L1->getChoiceGroups()){        //for grp1 in self.L1.choice_groups:
    for (ChoiceGroup* grp2 : L2->getChoiceGroups()){    //for grp2 in self.L2.choice_groups:
      for (Choice* choice1 : grp1->getChoices()){     //for choice1 in grp1.choices:
        c1 = grp1->getChoice(choice1)->getStringGroups(); // c1 = grp1.choices[choice1].with_str_groups()#choice1.with_str_groups()
        for (Choice* choice2 : grp2->getChoices()){ //for choice2 in grp2.choices:
          c2 = grp2->getChoice(choice2)->getStringGroups(); //c2 = grp2.choices[choice2].with_str_groups()#choice2.with_str_groups()
          
          float prescore = grp1->choices[c1]->score + grp1->choices[c2]->score; //prescore = grp1.choices[choice1].score + grp2.choices[choice2].score
          
          
          
        }
        
        
      }
    }
  }
}








score, combo, intermediates, scores = self.get_min_edit_distance(c1, c2)
self.logger.debug("\t\tProduced the following choices with overall score " + str(score))
curr_choices = [Choice(groups=inter,score=(prescore+s)) for inter,s in zip(intermediates, scores)]
for c in curr_choices:
c3 = c.group_list_string()
if c3 in choice_map:
curr = choice_map[c3]
if curr[0] > c.score:
choice_grps[curr[1]].remove_by_str(c3)
choice_map[c3] = (c.score, i)
else:
choice_map[c3] = (c.score, i)
#self.logger.debug("\t\t\tPrevious score: " +  str(prescore + self.get_min_edit_distance(c3, c1, backtrace=False)[0] + self.get_min_edit_distance(c3, c2, backtrace=False)[0]))
self.logger.debug("\t\t\t" + str(c))
choice_grps.append(Choice_Group(choices=curr_choices, children=[choice1, choice2], overall_score=score + prescore))
i += 1
return Label(choice_groups = choice_grps)



def test():
c11 = Choice(groups=[['F'],['E'],['H','J']], score=10)
c12 = Choice(groups=[['F','E'],['E'],['H','J']], score=10)
g1 = Choice_Group(choices=[c11,c12])
c2 = Choice(groups=[['A','E'],['E'],['J','F']], score=0)
g2 = Choice_Group(choices=[c2])
l1 = Label(choice_groups = [g1])
l2 = Label(choice_groups = [g2])
lm = LabelMatcher(l1, l2)
al = lm.get_ancestor_label()
#print('made it here')
#print(al)

#endif //GBLOCK_DIFFLIB
