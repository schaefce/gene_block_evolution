//
//  utility.h
//  GeneBlock
//
//  Created by Schaeffer, Charlotte Elizabeth Ms. on 6/19/15.
//  Copyright (c) 2015 Charlotte Schaeffer. All rights reserved.
//

#ifndef GeneBlock_utility_h
#define GeneBlock_utility_h

#include <regex>
#include <string>
#include <vector>
#include <sstream>



template <typename T>
std::string join(std::vector<T> groups, std::string C){
  std::ostringstream ss;
  std::copy(groups.begin(), groups.end() - 1, std::ostream_iterator<T>(ss, C.c_str()));
  ss << groups.back();
  return ss.str();
}

template <typename T>
std::vector<std::string> joinNested(std::vector<std::vector<T>> groups, std::string C){
  std::vector<std::string> stringGroups;
  stringGroups.resize(groups.size()*2);
  std::transform(groups.begin(), groups.end(), stringGroups.begin(), [&C](std::vector<T> v){ return join<T>(v,C); });
  return stringGroups;
}


inline std::vector<std::string> reSplit(const std::string &s, std::string reString= "\\s+"){
  std::vector<std::string> elements;
  std::regex re(reString);
  std::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
  std::sregex_token_iterator end;
  while(iter != end){
    elements.push_back(*iter);
    ++iter;
  }
  return elements;
}

inline std::string formatEnds(std::string s, char left, char right){
  std::ostringstream ss;
  ss << left;
  ss << s;
  ss << right;
  return ss.str();
}

template <typename T>
std::vector<std::vector<T>> groupBy(std::vector<T> input, T item){
  std::vector<std::vector<T>> result;
  std::vector<T> addition;
  for(T currItem : input){
    if (currItem == item){
      result.push_back(addition);
      addition.clear();
    }
    else{
      addition.push_back(currItem);
    }
  }
  if(!addition.empty()){
    result.push_back(addition);
  }
  return result;
}




#endif
