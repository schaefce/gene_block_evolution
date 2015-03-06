from Bio import Phylo
from Bio.Phylo import Newick
from Bio.Phylo import BaseTree
from Bio.Phylo.BaseTree import TreeElement

MAX_SCORE = float("inf")

class Choice():
    def __init__(self, groups=None, score=MAX_SCORE):
        self.groups = groups
        self.score = score

    def set_groups(self, grps):
        self.groups = grps

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
    def __init__(self, choices=None, children=None,  overall_score=MAX_SCORE):
        self.set_choices(choices)
        self.children = children
        self.overall_score = overall_score

    def set_choices_from_dict(cls, choice_dict=None):
        self.choices = []
        for c in choice_dict:
            self.choices.append(Choice(groups=c, score=choice_dict[c]))

    def set_choices(self, choices):
        if choices:
            self.choices = []
            if type(choices) is list:
                for c in choices:
                    self.choices.append(c)
            else:
                self.choices.append(choices)

    def set_score(self, score):
        if score:
            self.overall_score = score

    def get_choices_string(self):
        return '\n'.join('\t{}. {}'.format(i, str(k)) for i,k in enumerate(self.choices))

    def __str__(self):
        #print('called str on group')
        return 'Choice Group: (overall score: {})\n{}'.format(self.overall_score, self.get_choices_string())

    #def __repr__(self):
    #    return self.__str__()


class Label():
    def __init__(self, choice_groups=None, final_choice=None, final_score=MAX_SCORE):
        self.set_choice_groups(choice_groups)
        self.is_final = False
        self.set_final_choice(final_choice)
        self.final_score = final_score

    @classmethod
    def for_leaf(cls, group_list=None):
        c = Choice(groups=group_list, score=0)
        g = Choice_Group(choices=c, overall_score=0)
        l = Label(choice_groups=g, final_choice=c)
        return l

    def set_choice_groups(self, choice_groups):
        if choice_groups:
            self.choice_groups = []
            if type(choice_groups) is list:
                self.choice_groups.extend(choice_groups)
            else:
                self.choice_groups.append(choice_groups)


    def add_choice_group(self, choice_group):
        if type(choice_group) is list:
            self.choice_groups.extend(choice_group)
        else:
            self.choice_groups.append(choice_group)

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

    def get_final_label(self):
        if self.is_final:
            return self.final_label

    def __str__(self):
        if self.is_final:
            return 'Final Label:{lab},{score}'.format(lab=self.get_final_label(), score=self.score)
        else:
            return 'Label:\n' + '\t\n'.join('{}. {}'.format(i, str(k)) for i,k in enumerate(self.choice_groups))


class Labeled_Tree(Newick.Tree):
    def __init__(self, root=None, rooted=False, id=None, name=None, weight=1.0, leaf_id_map=None, leaf_label_map=None):
        Newick.Tree.__init__(self, root=root or Labeled_Clade(),
                               rooted=rooted, id=id, name=name, weight=weight)
        if leaf_id_map:
            self.add_leaf_ids(leaf_id_map)
            if leaf_label_map:
                self.add_leaf_labels(leaf_label_map, True)

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
        if id_map:
            terminals = self.get_terminals()
            for leaf in terminals:
                if leaf.name in id_map.keys():
                    leaf.ident = id_map[leaf.name]
                elif prune_unidentified:
                    self.prune(leaf)


    def add_leaf_labels(self, label_map=None, prune_unlabeled=False):
        if label_map:
            terminals = self.get_terminals()
            for leaf in terminals:
                if leaf.ident in label_map.keys():
                    #only_choice = Choice(groups=label_map[leaf.ident], score=0)
                    leaf.label = Label.for_leaf(group_list=label_map[leaf.ident])
                    #leaf.label = Label(choices=only_choice)
                    #leaf.label.set_final_choice(choice=only_choice)
                elif prune_unlabeled:
                    self.prune(leaf)

class Labeled_Clade(Label, Newick.Clade):
    """Labeled Tree Clade (sub-tree) object."""

    def __init__(self, branch_length=None, name=None, clades=None,
                 confidence=None, comment=None, ident=None, choice_groups=None):
        Newick.Clade.__init__(self, branch_length=branch_length,
                                name=name, clades=clades, confidence=confidence, comment=comment)
        self.label = Label(choice_groups=choice_groups)
        self.ident = ident

    @classmethod
    def from_clade(cls, clade, **kwargs):
        """Create a new Labeled Tree Clade from a Newick or BaseTree Clade object.
        """
        #print(cls)
        #print(type(clade))
        new_clade = cls(branch_length=clade.branch_length,
                        name=clade.name)
        new_clade.clades = [cls.from_clade(c) for c in clade]
        new_clade.confidence = clade.confidence
        new_clade.comment = clade.comment
        #new_clade.label = Label(clade.label.choices) if clade.label else None
        new_clade.__dict__.update(kwargs)
        return new_clade

    def to_tree(self, **kwargs):
        """Create a new Labeled Tree containing just this clade."""
        ltree = Labeled_Tree(root=self)
        ltree.__dict__.update(kwargs)
        return ltree

    def add_label(self, label=None):
        self.label = label
