// CmdLineParser.h is part of GENEBLOCKER.
//
// GENEBLOCKER is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GENEBLOCKER is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GENEBLOCKER.  If not, see <http://www.gnu.org/licenses/>.

// Created by Carly Schaeffer and John Karro

#include "CmdLineParser.h"
#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <iostream>

using namespace std;

CLParser parse_cmd_line(int argc, char** argv) {
  CLParser clp;

  int marker = 1;

  try {
    while (marker < argc && argv[marker][0] == '-') {
      if (strcmp(argv[marker], "-h")==0 || strcmp(argv[marker], "--help")==0) {
	cout << help_menu << endl;
	exit(0);
      }
      else if (strcmp(argv[marker], "-n")==0 || strcmp(argv[marker], "--noProve")==0)
	clp.noPrune = true;
      else if (strcmp(argv[marker], "-g")==0 || strcmp(argv[marker], "--maxGap")==0)
	clp.maxGap = atoi(argv[++marker]);
      else if (strcmp(argv[marker], "-f")==0 || strcmp(argv[marker], "--fname")==0)
	clp.fname = (string)argv[++marker];
      else if (strcmp(argv[marker], "-v")==0 || strcmp(argv[marker], "--verbosity")==0)
	clp.verbosity = atoi(argv[++marker]);
      else if (strcmp(argv[marker], "-p")==0 || strcmp(argv[marker], "--path")==0)
	clp.path = (string)argv[++marker];
      marker++;
    }
    assert(marker + 3 == argc);

    clp.geneBlock = (string)argv[marker++];
    clp.treeFile = (string)argv[marker++];
    clp.idMap = (string)argv[marker++];
  }
  catch (std::exception e) {
    cerr << "Bad Command line\n" << (string)help_menu << endl;
    exit(1);
  }

  return clp;
}
  
    
