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
#include <string>

#ifndef CMDLINEPARSER_H
#define CMDLINEPARSER_H

class CLParser {
 public:
  bool noPrune;
  int maxGap;
  std::string fname;
  int verbosity;
  std::string path;
  std::string geneBlock;
  std::string treeFile;
  std::string idMap;

  CLParser() {
    noPrune = false;
    maxGap = 5;
    fname = "";
    verbosity = 1;
    geneBlock = "";
    treeFile = "";
    idMap = "";
    path = ".";
  }
};

CLParser parse_cmd_line(int argc, char** argv);

static const char* help_menu = "\
reconstruct_gblock [hngfv] geneBlock treeFile idMap:\n\
\t-h, --help:       this help menu.\n\
\t-n, --noProve:    turn pruning off.\n\
\t-g, --maxHap:     maximum gap size (default: 5).\n\
\t-o, --output:     output file (default: out.labeled.nwk).\n\
\t-v, --verbosity: verbosity level (default: 1).\n\
\t-p, --path:       file path.\n\
\tgeneBlock:        gene-block file.\n\
\ttreeFile:         tree file.\n\
\tidMap:            ID mappings.\n\
";

  


#endif
