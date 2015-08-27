import logging
from Bio import Phylo
from Bio.Phylo import Newick
from Bio.Phylo import BaseTree
from Bio.Phylo.BaseTree import TreeElement

MAX_SCORE = float("inf")


class Choice():
    def __init__(self, choice_group=None, groups=None, score=MAX_SCORE):
        self.groups = groups
        self.score = score
        self.choice_group = choice_group

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

class Choice_Group():
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

    #def __repr__(self):
    #    return self.__str__()


class Label():
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
            print("cg:", cg)
            for c in cg.choices:
                print("C:" , c, type(c))
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

class Final_Label(TreeElement):
    def __init__(self, choice):
        self.choice=choice

    @classmethod
    def from_label(cls, label):
        if label is None or not label.is_final:
            return None
        flabel = cls(choice=label.final_choice)
        return flabel

    def __str__(self):
        return str(self.choice)


class Labeled_Tree(Newick.Tree):
    def __init__(self, root=None, rooted=False, id=None, name=None, weight=1.0, leaf_id_map=None, leaf_label_map=None):
        Newick.Tree.__init__(self, root=root or Labeled_Clade(),
                               rooted=rooted, id=id, name=name, weight=weight)
        if leaf_id_map:
            self.add_leaf_ids(leaf_id_map)
            if leaf_label_map:
                self.add_leaf_labels(leaf_label_map, True)

        self.logger = logging.getLogger("geneblock.new.labeledtree")
        self.ladderize(reverse=True)


    @classmethod
    def from_tree(cls, tree, **kwargs):
        """Create a new Labeled Tree given a Tree (from Newick/Nexus or BaseTree).
        """
        ltree = cls(
            root=Labeled_Clade.from_clade(tree.root),
            rooted=tree.rooted,
            name=tree.name,
            id=(tree.id is not None) and Id(str(tree.id)) or None)
        ltree.__dict__.update(kwargs)
        return ltree

    @classmethod
    def from_tree_and_maps(cls, tree, leaf_id_map, leaf_label_map, **kwargs):
        """Create a new Labeled Tree given a Tree (from Newick/Nexus or BaseTree).
        """
        ltree = cls(
            root=Labeled_Clade.from_clade(tree.root),
            rooted=tree.rooted,
            name=tree.name,
            id=(tree.id is not None) and Id(str(tree.id)) or None,
            leaf_id_map=leaf_id_map,
            leaf_label_map=leaf_label_map)
        ltree.__dict__.update(kwargs)
        return ltree


    @classmethod
    def from_clade(cls, clade, **kwargs):
        """Create a new Labeled Tree given a Newick or BaseTree Clade object.
        """
        return Labeled_Clade.from_clade(clade).to_tree(**kwargs)

    def add_leaf_ids(self, id_map=None, prune_unidentified=False):
        prune_list = []
        if id_map:
            terminals = self.get_terminals()
            for leaf in terminals:
                if id_map.has_key(leaf.name):
                    leaf.ident = id_map[leaf.name]
                elif prune_unidentified:
                    prune_list.append(leaf)
        for leaf in prune_list:
            self.prune(leaf)


    def add_leaf_labels(self, label_map=None, prune_unlabeled=False):
        prune_list = []
        if label_map:
            terminals = self.get_terminals()
            for leaf in terminals:
                if leaf.ident in label_map.keys():
                    leaf.label = Label.for_leaf(clade=leaf, group_list=label_map[leaf.ident])
                elif prune_unlabeled:
                    prune_list.append(leaf)

        for leaf in prune_list:
            self.prune(leaf)

    def set_labels_from_root(self):
        self.logger.info("Setting labels from root")
        root_choice = self.root.label.get_best_choice()
        self.logger.debug("Set root choice as: " + str(root_choice))
        self.set_labels_from_choice(root_choice)

    def set_labels_from_choice(self, choice=None):
        if choice is None:
            return
        
        l = choice.get_label()
        self.logger.debug("Set clade with name " + str(l.clade.name) + " with choice " + str(choice))
        if l:
            l.set_final_choice(choice)
        children = choice.get_children()
        if children:
            for c in children:
                self.set_labels_from_choice(c)
    
    def __str__(self):
        return Newick.Tree.__str__(self) 
        

class Labeled_Clade(Label, Newick.Clade):
    """Labeled Tree Clade (sub-tree) object."""

    def __init__(self, branch_length=None, name=None, clades=None,
                 confidence=None, comment=None, ident=None, choice_groups=None):
        Newick.Clade.__init__(self, branch_length=branch_length,
                                name=name, clades=clades, confidence=confidence, comment=comment)
        self.label = Label(clade=self, choice_groups=choice_groups)
        #self.label.clade = self
        self.ident = ident

    @classmethod
    def from_clade(cls, clade, **kwargs):
        """Create a new Labeled Tree Clade from a Newick or BaseTree Clade object.
        """
        new_clade = cls(branch_length=clade.branch_length,
                        name=clade.name)
        new_clade.clades = [cls.from_clade(c) for c in clade]
        new_clade.confidence = clade.confidence
        new_clade.comment = clade.comment
        new_clade.__dict__.update(kwargs)
        return new_clade

    def to_tree(self, **kwargs):
        """Create a new Labeled Tree containing just this clade."""
        ltree = Labeled_Tree(root=self)
        ltree.__dict__.update(kwargs)
        return ltree

    def add_label(self, label=None):
        if label:
            self.label = label
            label.clade = self

    def __str__(self):
        return Newick.Clade.__str__(self)
