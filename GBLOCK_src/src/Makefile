CC = g++ -std=c++11
CPPINCL+=-I../../tree_code/tree.h
CXXFLAGS+=-Wall -W -O3 -DNDEBUG

###########################################################
# GBLOCK
###########################################################
all: main

main: LabeledTree.h DiffLib.h LabelAncestors.h main.cpp
    @echo compiling GBLOCK;
        $(CC) $(CPPINCL) $(CXXFLAGS) $^ -o ../../reconstruct_gblock
