#!/usr/bin/python
#from os.path import basename
import os
import logging
import argparse
import csv
import math
#from make_event_distance_matrix import homolog_list_grouping_function
import sys
from homolog4 import *
from Bio import Phylo
from Bio.Phylo import *
from difflib_geneblock2 import LabelMatcher
from Labeled_Tree import Label, Choice, Labeled_Tree, Labeled_Clade


def parse_params(args):
    """Parse command line arguments using the argparse library"""
    parser = argparse.ArgumentParser(description = "Evaluate RAIDER against RepeatScout")
    parser.add_argument('gene_block', help='Path to file with data about geneblock')
    parser.add_argument('tree', help='Path to file with tree data')
    parser.add_argument('id_map', help='Path to file mapping IDs to labels')
    parser.add_argument('--no_prune', dest='prune', action='store_false', help='Do not prune unlabeled leaves', default=True)
    parser.add_argument('--max_gap', dest='max_gap', type=int, help='Max gap', default=500)
    parser.add_argument('-f','--fname', dest='ofile', help='File to output tree to')#, default="labeled_tree.nwk")
    return parser.parse_args(args)

def get_label_map(gene_block_fname, max_gap=500):
    """ Use gene_block result from file to determine the homologs for each organism
    and group them according to homolog_list_grouping_function. Returns a mapping
    from organism's accession ID to list of homolog lists.
    """

    # In case ever want to store gene_block name
    # gene_block = os.path.basename(fname).split('.')[0]
    d = {}
    org_dict = {}
    for line in [i.strip() for i in open(gene_block_fname).readlines() if i.strip()]:
    # catch some errors if they occur and report a useful message as a result.
        try:
            hlog = Homolog.from_blast(line)
        except:
            print("ERROR " +  line)
        try:
            accession = hlog.accession()
        except:
            print(line)
        predicted_gene = hlog.blast_annotation()

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
            d[accession].append([i.blast_annotation() for i in neighborhood])
    return d

def homolog_list_grouping_function(list_homologs, max_gap):
    """ Stole this out of make_event_distance_matrix """
    result = []
    neighborhood = [list_homologs[0]]
    
    for i in list_homologs[1:]:
        #look at current
        start = neighborhood[-1].start() #start = list_homologs[i].start()
        stop = neighborhood[-1].stop() #stop = list_homologs[i].stop()
        # look at next
        start_n = i.start() #start_n = list_homologs[i+1].start()
        stop_n = i.stop() #stop_n = list_homologs[i+1].stop()
        
        # We have found neighboring genes, as defined by max_gap
        if math.fabs(start - stop_n) < max_gap or math.fabs(stop - start_n) < max_gap:
            neighborhood.append(i)
        # These genes do not neighbor eachother
        else: 
            result.append(neighborhood)
            neighborhood = [i]
    result.append(neighborhood)
    #print list_homologs[0].organism(), "result", result, "neighborhood_found ", neighborhood_found  
    return result

def get_identifiers(fname):
    #idmap = {}
    #for v, k in csv.reader(open(fname, 'r')):
    #    idmap[k] = v
    #return idmap
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
                leaf.ident = ident
                #only_choice = Choice(label_map[ident])
                leaf.label = Label.for_leaf(label_map[ident])#(choices=[only_choice])
                #leaf.label.set_final_choice(choice=only_choice)
                #if leaf.label.is_final:
                #    leaf.comment = leaf.label.final_label
            else:
                prune_list.append(curr_name)
        else:
            prune_list.append(curr_name)
    if prune_unlabeled:
        for node in prune_list:
            tree.prune(node)
    tree.ladderize(reverse=True)


def read_tree(fname, treeformat='newick'):
    tree = Phylo.read(fname, treeformat)
    return tree

def write_tree(tree, fname, treeformat='newick'):
    logger.info("Writing phylogenetic tree to file " + fname)
    Phylo.write(tree, fname, treeformat)


def format_tree(gene_block, map_fname, tree_fname, max_gap = 500, prune_unlabeled=True):
    """ Use gene_block_organism_data for this gene_block, mapping of common names
    IDs, and existing tree file to create tree labeled with list of homologs.

    :Parameters:
        gene_block : string

    """
    logger.info("Creating phylogenetic tree")
    #gblock_fname = gene_block if infolder is None else '{folder}/{block}.txt'.format(folder=infolder, block=gene_block)
    label_map = get_label_map(gene_block, max_gap)#get_labelings(gene_block, infolder, filter)
    id_map = get_identifiers(map_fname)
    tree = read_tree(tree_fname)
    #print(tree)
    logger.debug("Created phylogenetic tree:\n" + str(tree))
    #print(tree)
    tree = Labeled_Tree.from_tree_and_maps(tree, id_map, label_map)
    logger.debug("Created phylogenetic tree with labeled leaves:\n" + str(tree))
    return tree


def set_possible_labels(tree, logname):
    #tree.ladderize(reverse=True)
    logger.info("Setting all possible labels for tree")
    tree.ladderize()
    
    parents = {}
    for clade in tree.find_clades(order='level'):
        for child in clade:
            parents[child] = clade

    #print tree
    child_prs = get_child_pairs(tree)
    parent = None
    logger.debug("Child pairs:\t" + str(child_prs))
    while child_prs:
        pr = child_prs.pop()
        if len(pr) < 2:
            break
        A = pr[0]
        B = pr[1]
        #print(str(A), str(B))
        #assert(get_parent(tree, A) == get_parent(tree, B))
        #parent = get_parent(tree, A)
        assert(parents[A]==parents[B])#(tree, A) == get_parent(tree, B))
        parent = parents[A]#(tree, A)
        logger.info("Getting possible labels for parent of {c1} and {c2}".format(c1=str(A), c2=str(B)))
        lm = LabelMatcher(A.label, B.label, logname)
        ancestor_label = lm.get_ancestor_label()
        parent.add_label(ancestor_label)
        logger.info("Next parent received the labeling:\n" + str(ancestor_label))
    logger.debug("Done setting all possible labels for tree")



def get_parent(tree, child_clade):
    node_path = tree.get_path(child_clade)
    #print(str(node_path[-2]))
    
    return node_path[-2]


def get_child_pairs(tree):
    child_pairs = []

    def dfs(elem, get_children, stack):
        pair=get_children(elem)
        if pair:
            stack.append(pair)
            for v in pair:
                dfs(v,get_children,stack)
        return stack

    get_children = lambda elem: elem.clades
    #child_pairs.append(tree.root)
    return dfs(tree.root, get_children, child_pairs)


if __name__ == "__main__":
    args = parse_params(sys.argv[1:])
    tname = os.path.splitext(os.path.basename(args.tree))[0]# basename(args.tree)
    logname = 'geneblock.new.' + tname
    logger = logging.getLogger(logname)
    logger.setLevel(logging.DEBUG)
    fh = logging.FileHandler('gblock.log.debug.' + tname, mode='w')
    f = logging.Formatter('%(asctime)s: %(message)s')
    #f = logging.Formatter('%(asctime)s [%(levelname)s] %(name)s: %(message)s')
    fh.setLevel(logging.DEBUG)
    fh.setFormatter(f)
    logger.addHandler(fh)
    fih = logging.FileHandler('gblock.log.info' + tname, mode='w')
    #f = logging.Formatter('%(asctime)s [%(levelname)s] %(name)s: %(message)s')
    fih.setLevel(logging.INFO)
    fih.setFormatter(f)
    logger.addHandler(fih)
    tree = format_tree(args.gene_block,args.id_map, args.tree, args.max_gap, args.prune)
    #print(tree)
    set_possible_labels(tree, logname)
    logger.info("Created phylogenetic tree with labeled leaves:\n" + str(tree))
    tree.set_labels_from_root()
    logger.info("Final tree with labels and scores:\n" + str(tree))
    ofile = args.ofile if args.ofile else str(tname + '.labeled.nwk')
    write_tree(tree, ofile)
    #print(tree)

