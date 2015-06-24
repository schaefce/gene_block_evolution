CC = g++ -std=c++11
CXXFLAGS+=-Wall -W -O3 -DNDEBUG
CFLAGS = -Wall -c -DNDEBUG
LFLAGS = -Wall -L/usr/local/Cellar/boost/1.56.0/lib/ -lboost_filesystem -lboost_program_options -lboost_system -g
CPPINCLUDE = -I/usr/local/Cellar/boost/1.56.0/include/

###########################################################
# GBLOCK
###########################################################
all: main

main: main.o labeledtree.o difflib.o
	@echo compiling GBLOCK;
		$(CC) $(LFLAGS) $^ -o ../../reconstruct_gblock

main.o: main.cpp labeledtree.h difflib.h
	$(CC) $(CPPINCLUDE) $(CFLAGS) main.cpp

labeledtree.o: labeledtree.cpp labeledtree.h tree.h basenode.h labelednode.h label.h choicegroup.h choice.h utility.h
	$(CC) $(CFLAGS) labeledtree.cpp

difflib.o: difflib.cpp difflib.h label.h choicegroup.h choice.h utility.h
	$(CC) $(CFLAGS) difflib.cpp
