# Makefile is part of GENEBLOCKER.
#
# GENEBLOCKER is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GENEBLOCKER is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GENEBLOCKER.  If not, see <http://www.gnu.org/licenses/>.

# Created by Carly Schaeffer and John Karro


UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
    BOOST_LIB = /usr/local/Cellar/boost/1.56.0/lib/
    BOOST_INC = /usr/local/Cellar/boost/1.56.0/include/
else
    BOOST_LIB = /software/boost/1_54_0/gnu-4.7.2/lib/
    BOOST_INC = /software/boost/1_54_0/gnu-4.7.2/include/
endif
CC = g++ -std=c++11
CXXFLAGS += -Wall -W -g -DNDEBUG
CFLAGS = -Wall -c -DNDEBUG
LFLAGS = -Wall -L$(BOOST_LIB) -lboost_filesystem -lboost_program_options -lboost_system -g
CPPINCLUDE = -I$(BOOST_INC)

###########################################################
# GBLOCK
###########################################################
all: main

main: labeledtree.o difflib.o main.o
	@echo compiling GBLOCK;
		$(CC) $(LFLAGS) $^ -o ../../reconstruct_gblock

main.o: main.cpp labeledtree.h difflib.h
	$(CC) $(CPPINCLUDE) $(CFLAGS) main.cpp

labeledtree.o: labeledtree.cpp labeledtree.h tree.h basenode.h labelednode.h label.h choicegroup.h choice.h utility.h
	$(CC) $(CFLAGS) labeledtree.cpp

difflib.o: difflib.cpp difflib.h label.h choicegroup.h choice.h utility.h
	$(CC) $(CFLAGS) difflib.cpp
