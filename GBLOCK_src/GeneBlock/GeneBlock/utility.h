//
//  utility.h
//  GeneBlock
//
//  Created by Schaeffer, Charlotte Elizabeth Ms. on 6/19/15.
//  Copyright (c) 2015 Charlotte Schaeffer. All rights reserved.
//

#ifndef GeneBlock_utility_h
#define GeneBlock_utility_h





template <typename T>
std::string join(std::vector<T> groups, char C){
  std::ostringstream ss;
  std::copy(groups.begin(), groups.end() - 1, std::ostream_iterator<T>(ss, C));
  ss << groups.back();
  return ss.str();
}



std::string formatEnds(string s, char left, char right){
  std::ostringstream ss;
  ss << left;
  ss << s;
  ss << right;
  return ss.str();
}




#endif
