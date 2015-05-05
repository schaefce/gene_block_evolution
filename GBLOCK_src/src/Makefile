CC = g++ -std=c++11

CXXFLAGS+=-Wall -W -O3 -DNDEBUG

###########################################################
# GBLOCK
###########################################################
all: main

main: LabeledTree.h DiffLib.h LabelAncestors.h main.cpp
    @echo compiling GBLOCK; 
        $(CC) $(CXXFLAGS) $^ -o ../../reconstruct_gblock

