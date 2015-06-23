CC = g++ -std=c++11
CXXFLAGS+=-Wall -W -O3 -DNDEBUG
CFLAGS = -Wall -c -DNDEBUG
LFLAGS = -Wall -DNDEBUG
CPPINCLUDE = -I/usr/local/Cellar/boost/

###########################################################
# GBLOCK
###########################################################
all: main

main: main.o labeledtree.o difflib.o
  @echo compiling GBLOCK;
      $(CC) $(CXXFLAGS) $^ -o ../../reconstruct_gblock

main.o: main.cpp labeledtree.h difflib.h
  $(CC) $(CFLAGS) main.cpp

labeledtree.o: labeledtree.cpp labeledtree.h tree.h basenode.h labelednode.h label.h choicegroup.h choice.h utility.h
  $(CC) $(CFLAGS) labeledtree.cpp

difflib.o: difflib.cpp difflib.h label.h choicegroup.h choice.h utility.h
  $(CC) $(CFLAGS) difflib.cpp
