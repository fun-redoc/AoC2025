//
// Created by rsh on 10/24/25.
//
#include <vector>
#include "../include/mymath.hpp"

//
// choose(5,3) = 10
//  0 1 2 -> 0 1 3 -> 0 1 4 
//        -> 0 2 3 -> 0 2 4 -> 0 3 4 
//        -> 1 2 3 -> 1 2 4 -> 1 3 4  
//                          -> 2 3 4
bool mymath::choose_next(size_t m, size_t n, std::vector<size_t>& prev) {
  // choose_next(5,3, [0 1 2]) -> [0 2
  assert(prev.size() == n);
  if(n==0) return false; // nothing to choose
  int r = prev.size()-1;
  if(prev[r] < m-1) {
    prev[r]++;
    return true;
  } else {
    r--;
    while(r>=0) {
      if(prev[r] < m-(n-r)) {
        prev[r]++;
        r++;
        while(r<(int)n) {
          prev[r] = prev[r-1]+1;
          r++;
        }
        return true;
      }
      r--;
    }
    if(r<0) return false;
  }
}

