from Bio import Phylo
from Bio.Phylo import Newick


class Label():
    def __init__(self, groups=[], finalLabel=None, confidence=None):
        self.groups = groups
        self.final = finalLabel
        self.confidence = confidence

class Tree(Label, Newick.Tree):
    def __init__(self, root=None, rooted=False, id=None, name=None, weight=1.0):
        Newick.Tree.__init__(self, root=root or Clade(),
                               rooted=rooted, id=id, name=name, weight)

class Clade(Label, Newick.Clade):
    """Newick Clade (sub-tree) object."""

    def __init__(self, branch_length=None, name=None, clades=None,
                 confidence=None, comment=None):
        Newick.Clade.__init__(self, branch_length=branch_length,
                                name=name, clades=clades, confidence=confidence, comment)
        self.label =
