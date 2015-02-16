

def convert_to_phyloxml(tree):
    tree_pxml = tree.as_phyloxml()

# generate mapping of all nodes to their parents
def all_parents(tree):
    parents = {}
    for clade in tree.find_clades(order = 'level'):
        for child in clade:
            parents[child] = clade
    return parents


def all_parents(tree):
    return all_parents_recursive(tree, {})

def all_parents_recursive(clade, pars):
    if clade.is_terminal():
        return clade
    else:
        for child in clade.find_clades(order = 'level'):
            pars[child] = all_parents_recursive(child,pars)
        return pars


import make_event_distance_matrix

def get_all_labelings(infolder, filter):
    s = return_gene_block_organism_data(return_file_list(infolder, filter))
    d = {}
    for gene_block in sorted(s.keys()):
        d[gene_block] = {}
        for organism in s[gene_block].keys():
            h_list = s[gene_block][organism]
            groups = homolog_list_grouping_function(h_list, 500)
            d[gene_block][organism] = []
            for neighborhood in groups:
                d[gene_block][organism].append([i.blast_annatation() for i in neighborhood])
    return d

def get_labelings(gene_block, infolder, filter):
    s = return_gene_block_organism_data(return_file_list(infolder, filter))[gene_block]
    d = {}
    for organism in s.keys():
        h_list = s[organism]
        groups = homolog_list_grouping_function(h_list, 500)
        d[organism] = []
        for neighborhood in groups:
            d[organism].append([i.blast_annatation() for i in neighborhood])
    return d


# get parent of clad
def get_parent(tree, child_clade):
    node_path = tree.get_path(child_clade)
    return node_path[-2]
