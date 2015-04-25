#!/usr/bin/python
"""
Module difflib_geneblock -- helpers for computing deltas between objects.
Class LabelMatcher:
    A flexible class for comparing pairs of labels.
"""
import itertools
import re
import logging
from Labeled_Tree import Label, Choice, Choice_Group


SPLIT = "*"

class LabelMatcher:
    """ Matches two choices as best as possible, where a choice is a list of
        lists, the outer list's order doesn't matter but the inner list's order matters."""

    def __init__(self, L1=None, L2=None, logname="geneblock.new.labeling"):
        self.L1 = self.L2 = None
        self.set_labels(L1, L2)
        self.DUP_PENALTY = 5.0
        self.DEL_DUP_PENALTY = self.DUP_PENALTY
        self.SPLIT_PENALTY = 7.0
        self.DEL_SPLIT_PENALTY = self.SPLIT_PENALTY
        self.DEL_PENALTY = 3.0
        self.INS_PENALTY = self.DEL_PENALTY
        self.MISMATCH_PENALTY = 100000#float("inf")
        self.MATCH_PENALTY = -1.0
        self.logger = logging.getLogger(logname)

    def set_labels(self, L1, L2):
        self.set_label1(L1)
        self.set_label2(L2)

    def set_label1(self, L1):
        if L1 is self.L1:
            return
        self.L1 = L1

    def set_label2(self, L2):
        if L2 is self.L2:
            return
        self.L2 = L2


    def deletion_penalty(self, A, B, j, i):
        """ Determine cost of deleting Bi """
        if i > 0 and B[i] == B[i-1]:
            return self.DEL_DUP_PENALTY
        elif B[i] == SPLIT:
            return self.DEL_SPLIT_PENALTY
        else:
            return self.DEL_PENALTY

    def insertion_penalty(self, A, B, j, i):
        """ Determine cost of inserting Aj after Bi """
        if j > 0 and A[j] == A[j-1] == B[i]:
            return self.DUP_PENALTY
        elif A[j] == SPLIT:
            return self.SPLIT_PENALTY
        else:
            return self.INS_PENALTY

    def match_penalty(self, A, B, j, i):
        if A[j] == B[i]:
            return self.MATCH_PENALTY
        else:
            return self.MISMATCH_PENALTY

    def special_split_penalty(self, A, B, j, i):
        if j > 1 and B[i] == A[j] == A[j-2] and A[j-1] == SPLIT:
            return self.DUP_PENALTY + self.SPLIT_PENALTY
        else:
            return self.MISMATCH_PENALTY


    def get_edit_distance(self, A, B, allow_insertions=False, backtrace=True):
        self.logger.debug("\t\t\t\t\tGetting edit distance between {c1} and {c2}".format(c1=str(''.join(A)),c2=str(''.join(B))))
        #print(A)
        #self.logger.debug("\t\t\t\t\tBacktrace:\t" + str(backtrace)) #Getting edit distance between {c1} and {c2}".format(c1=str(A),c2=str(B)))
        #print(B)
        n = len(A)
        m = len(B)

        # subproblems[i][j] will represent the edit distance from B[...i] to A[...j]
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
                    # Insert a split and Aj
                    case4 = subproblems[i][j-2] + self.special_split_penalty(A, B, aj, bi)
                else:
                    case4 = float("inf")
                subproblems[i][j] = min([case1, case2, case3, case4])
        #self.logger.debug("\t\t\t\t\tHere is the matrix:")
        #for i in range(m):
        #    self.logger.debug("\t\t\t\t\t\t" + str(subproblems[i]))

        #for i in range(m):
        #    for j in range(n):
        #        print '{:4}'.format(subproblems[i][j]),
        #    print

        penalty = subproblems[m][n]

        self.logger.debug("\t\t\t\t\tEdit distance between {c1} and {c2}:\t{p}".format(c1=str(''.join(A)),c2=str(''.join(B)),p=str(penalty)))
        intermediates = self.perform_backtrace(A, B, subproblems, allow_insertions) if backtrace else None
        return penalty, intermediates


    def perform_backtrace(self, A, B, subproblems, allow_insertions=False):
        self.logger.debug("\t\t\t\t\tPerforming backtrace for {c1} and {c2}".format(c1=str(''.join(A)),c2=str(''.join(B))))
        answer = []
        intermediates = []

        j = len(A) #j = n
        aj = j - 1
        i = len(B) #i = m
        bi = i - 1

        while i > 0 or j > 0:
            self.logger.debug('\t\t\t\t\t\ti: '+str(i)+',j: '+ str(j))
            curr_intermediate = []

            pos = subproblems[i][j]
            case1_match = subproblems[i-1][j-1] + self.MATCH_PENALTY
            case1_mismatch = subproblems[i-1][j-1] + self.MISMATCH_PENALTY
            case2_del_dup = subproblems[i-1][j] + self.DEL_DUP_PENALTY
            case2_del_split = subproblems[i-1][j] + self.DEL_SPLIT_PENALTY
            case2_del_gen = subproblems[i-1][j] + self.DEL_PENALTY
            case3_ins_dup = subproblems[i][j-1] + self.DUP_PENALTY
            case3_ins_split = subproblems[i][j-1] + self.SPLIT_PENALTY
            case3_ins_gen = subproblems[i][j-1] + self.INS_PENALTY

            if i > 0 and j > 0 and pos == case1_match:
                curr_intermediate.append(B[bi])
                bi -= 1
                aj -= 1
                i -= 1
                j -= 1
            elif j > 1 and pos == subproblems[i][j-2] + self.DUP_PENALTY + self.SPLIT_PENALTY:
                curr_intermediate.append([A[aj],A[aj-1]])
                curr_intermediate.append(A[aj])
                curr_intermediate.append(A[aj-1])
                curr_intermediate.append('')
                #answer.append( ('add_split', aj, j, i, i) )
                #answer.append( ('add_dup', aj-1, aj, i, i) )
                j -= 2
                aj -= 2
            elif i > 0 and pos == case2_del_dup:
                curr_intermediate.append(B[bi])
                curr_intermediate.append('')
                #answer.append( ('delete_dup', j, j, bi, i) )
                i -= 1
                bi -= 1
            elif i > 0 and pos == case2_del_split:
                curr_intermediate.append(B[bi])
                curr_intermediate.append('')
                #answer.append( ('remove_split', j, j, bi, i) )
                i -= 1
                bi -= 1
            elif i > 0 and pos == case2_del_gen:
                curr_intermediate.append(B[bi])
                if allow_insertions:
                    curr_intermediate.append('') #IF ALLOWED TO HAVE SUDDEN INSERTS
                #answer.append( ('delete', j, j, bi, i) )
                i -= 1
                bi -= 1
            elif j > 0 and pos == case3_ins_dup:
                curr_intermediate.append(A[aj])
                curr_intermediate.append('')
                #answer.append( ('insert_dup', aj, j, i, i) )
                j -= 1
                aj -= 1
            elif j > 0 and pos == case3_ins_split:
                curr_intermediate.append(A[aj])
                curr_intermediate.append('')
                #answer.append( ('add_split', aj, j, i, i) )
                j -= 1
                aj -= 1
            elif j > 0 and pos == case3_ins_gen:
                curr_intermediate.append(A[aj])
                if allow_insertions:
                    curr_intermediate.append('') #IF ALLOWED TO HAVE SUDDEN INSERTS
                #answer.append( ('insert', aj, j, i, i) )
                j -= 1
                aj -= 1
            intermediates.append(curr_intermediate)
            #print(intermediates)
        #print(intermediates)
        return intermediates[::-1]#reversed(intermediates)

    def get_min_edit_distance(self, choice1, choice2, allow_insertions=False, backtrace=True):
        #print(choice1)
        #print(choice2)
        self.logger.debug("\t\t\tGetting min edit distance between {c1} and {c2}".format(c1=str(''.join(choice1)),c2=str(''.join(choice2))))
        curr_min = float("inf")
        best_combo = None
        #best_ops = None
        best_intermediates = [] #None
        backwards = False
        for A in itertools.permutations(choice1):
            A = [i for i in re.split('(\W+)', SPLIT.join(A)) if i != ',']
            #A = re.split('(\*)', SPLIT.join(A))
            for B in itertools.permutations(choice2):
                B = [i for i in re.split('(\W+)', SPLIT.join(B)) if i != ',']
                #self.logger.debug("\t\t\t\t\tA:" + str(A) + "\tB:" + str(B))
                #B = re.split('(\*)', SPLIT.join(B))
                score, intermediates = self.get_edit_distance(A, B, allow_insertions, backtrace=backtrace)
                #self.logger.debug('\t\t\t\tScore={s}'.format(s=score))
                #self.logger.debug(type(intermediates))
                #if intermediates:
                #    self.logger.debug('\t\t\t\tIntermediates:' + ',\t'.join(map(str, intermediates)))#[str(''.join(inter)) for inter in intermediates]))
                if score < curr_min:
                    curr_min = score
                    best_combo = (A, B)
                    #best_ops = codes
                    if intermediates:
                        best_intermediates = list(intermediates)
                    backwards = False
                    self.logger.debug("\t\t\t\tUpdated curr min:\t{m}".format(m=str(curr_min)))
                    self.logger.debug("\t\t\t\tUpdated best intermediates:"+ ',\t'.join(map(str, best_intermediates)))
                score, intermediates = self.get_edit_distance(B, A, allow_insertions, backtrace=backtrace)
                #self.logger.debug('\t\t\t\tScore={s}'.format(s=score))
                #if intermediates:
                #    self.logger.debug('\t\t\t\tIntermediates:' + ',\t'.join(map(str, intermediates)))#[str(''.join(inter)) for inter in intermediates]))
                if score < curr_min:
                    curr_min = score
                    best_combo = (A, B)
                    #best_ops = codes
                    if intermediates:
                        best_intermediates = list(intermediates)
                    backwards = True
                    self.logger.debug("\t\t\t\tUpdated curr min:\t{m}".format(m=str(curr_min)))
                    self.logger.debug("\t\t\t\tUpdated best intermediates:"+ ',\t'.join(map(str, best_intermediates)))
        #best_intermediates = [(''.join(x)).split(SPLIT) for x in best_intermediates]
        #best_intermediates = [filter(None, x) for x in best_intermediates]
        if backtrace:
            best_intermediates = list(itertools.product(*best_intermediates))
            #print(best_intermediates)
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
            best_intermediates = [[list(group) for k, group in itertools.groupby(l, lambda x: x == "*") if not k] for l in best_intermediates]
            best_intermediates = [filter(None, x) for x in best_intermediates]
            self.logger.debug("\t\t\t\tBest intermediates:\t{b}".format(b=',\t'.join(map(str, best_intermediates))))
            
            #print()

        #best_intermediates = [''.join(i) for i in best_intermediates]
        #best_intermediates = []
        return curr_min, best_combo, best_intermediates



    def get_ancestor_label(self):
        #best_choices = None
        #curr_overall_min = float("inf")
        #best_choice_children = None
        #logger = logging.getLogger('gblock.labeling')
        choice_grps = []
        for grp1 in self.L1.choice_groups:
            for grp2 in self.L2.choice_groups:
                for choice1 in grp1.choices:
                    c1 = choice1.with_str_groups()
                    for choice2 in grp2.choices:
                        self.logger.debug("\tAligning choice {ch1} with choice {ch2}".format(ch1=str(choice1), ch2=str(choice2)))
                        prescore = choice1.score + choice2.score
                        #self.logger.debug("\t\tPrescore: " + str(prescore))
                        c2 = choice2.with_str_groups()
                        score, combo, intermediates = self.get_min_edit_distance(c1, c2)
                        self.logger.debug("\t\tProduced the following choices with overall score " + str(score))
                        curr_choices = [Choice(groups=inter) for inter in intermediates]
                        for c in curr_choices:
                            c3 = c.with_str_groups()
                            c.score = prescore + self.get_min_edit_distance(c3, c1, backtrace=False)[0] + self.get_min_edit_distance(c3, c2, backtrace=False)[0]
                            self.logger.debug("\t\t\t" + str(c))
                        choice_grps.append(Choice_Group(choices=curr_choices, children=[choice1, choice2], overall_score=score + prescore))
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
