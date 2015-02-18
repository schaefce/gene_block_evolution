#!/usr/bin/python
import csv
from make_event_distance_matrix import *
from Bio import Phylo
from Bio.Phylo import *

class Label():
    def __init__(self, groups=None, final_groups=None):
        self.groups = groups or []
        self.final_groups = final_groups or []
        if final_groups:
            self.set_final()
        else:
            self.is_final = False
            self.final_label = None

    def add_groups(self, labels):
        for label in labels:
            grp = []
            for l in label:
                grp.append(l)
            self.groups.append(grp)

    def set_final_label(self, label):
        self.final_label = label
        self.is_final = True

    def set_final(self):
        self.set_final_label('(' + ','.join('(' + ','.join(l for l in label) + ')' for label in self.final_groups) + ')')




def get_label_map(gene_block_fname, max_gap=500):
    """ Use gene_block result from file to determine the homologs for each organism
    and group them according to homolog_list_grouping_function. Returns a mapping
    from organism's accession ID to list of homolog lists.
    """

    # In case ever want to store gene_block name
    # gene_block = os.path.basename(fname).split('.')[0]
    d = {}
    org_dict = {}
    for line in [i.strip() for i in open(gene_block_fname).readlines()]:
    # catch some errors if they occur and report a useful message as a result.
        try:
            hlog = Homolog.from_blast(line)
        except:
            print "ERROR", line
        try:
            accession = hlog.accession()
        except:
            print line
        predicted_gene = hlog.blast_annatation()

        # store the homolog where it belongs
        if accession in org_dict.keys():
            org_dict[accession].append(hlog)
        else:
            org_dict.update({accession:[hlog]})

    for accession in sorted(org_dict.keys()):
        h_list = org_dict.pop(accession)
        h_list.sort(key=lambda x: x.start())
        groups = homolog_list_grouping_function(h_list, max_gap)
        d[accession] = []
        for neighborhood in groups:
            d[accession].append([i.blast_annatation() for i in neighborhood])
    return d


def get_identifiers(fname):
    return {k:v for v,k in csv.reader(open(fname,'r'))}


def add_leaf_labels(tree, id_map, label_map, prune_unlabeled=True):
    terminals = tree.get_terminals()
    prune_list = []
    for i in range(len(terminals)):
        leaf = tree.get_terminals()[i]
        curr_name = terminals[i].name
        if curr_name in id_map.keys():
            ident = id_map[curr_name]
            if ident in label_map.keys():
                leaf.label = Label(groups=label_map[ident], final_groups=label_map[ident])
                if leaf.label.is_final:
                    leaf.comment = leaf.label.final_label

            else:
                prune_list.append(curr_name)
        else:
            prune_list.append(curr_name)
    if prune_unlabeled:
        for node in prune_list:
            tree.prune(node)


def read_tree(fname, treeformat='newick'):
    tree = Phylo.read(fname, treeformat)
    return tree

def format_tree(gene_block, map_fname, tree_fname, infolder=None, max_gap = 500, prune_unlabeled=True):
    """ Use gene_block_organism_data for this gene_block, mapping of common names
    IDs, and existing tree file to create tree labeled with list of homologs.

    :Parameters:
        gene_block : string

    """
    gblock_fname = gene_block if infolder is None else '{folder}/{block}.txt'.format(folder=infolder, block=gene_block)
    label_map = get_label_map(gblock_fname, max_gap)#get_labelings(gene_block, infolder, filter)
    id_map = get_identifiers(map_fname)
    tree = read_tree(tree_fname)
    add_leaf_labels(tree, id_map, label_map, prune_unlabeled)
    return tree


def set_possible_labels(tree):
    path = tree.find_elements(order='postorder')
