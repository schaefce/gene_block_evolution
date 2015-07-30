//
//  utility.h
//  GeneBlock
//
//  Created by Schaeffer, Charlotte Elizabeth Ms. on 6/19/15.
//  Copyright (c) 2015 Charlotte Schaeffer. All rights reserved.
//

#pragma once
#ifndef GENEBLOCK_UTILITY_H
#define GENEBLOCK_UTILITY_H

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
  std::transform(groups.begin(), groups.end(), stringGroups.begin(), [&C](std::vector<T> v){ return join<T>(v,C); });
  return stringGroups;
}

inline std::string formatEnds(std::string s, std::string left, std::string right){
  std::ostringstream ss;
  ss << left;
  ss << s;
  ss << right;
  return ss.str();
}

inline std::vector<std::string> reSplit(const std::string &s, std::string reString= "\\s+"){
  std::vector<std::string> elements;
  std::string delimRe = "^" + reString;
  try {
    std::regex re(reString);
    
    std::sregex_token_iterator iter(s.begin(), s.end(), re);//, -1);
    std::sregex_token_iterator end;
    while(iter != end){
      elements.push_back(*iter);
      ++iter;
    }
  } catch (std::regex_error& e) {
    std::cerr << e.what();
  }
  return elements;
}

inline std::vector<std::string> delimSplit(const std::string &s, std::string delim= ",", bool keepDelim=false){
  std::vector<std::string> elements;
  std::string delimRe = delim + "+";
  std::string notDelimRe = "[^" + delim + "]+";
  std::string reString = keepDelim ? delimRe + "|" + notDelimRe : notDelimRe;
  try{
    std::regex re(reString);
    std::sregex_token_iterator iter(s.begin(), s.end(), re);//, -1);
    std::sregex_token_iterator end;
    while(iter != end){
      elements.push_back(*iter);
      ++iter;
    }
  } catch (std::regex_error& e) {
    std::cerr << e.what();
  }
  
  return elements;
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
