//
// Created by rsh on 2025-11-29.
//

#include <iostream>
#include <vector>
#include <cassert>
#include "../include/mymath.hpp"
#include "../include/aoc2025_01.hpp"

typedef long long ll;

using namespace std;
using namespace mymath;
namespace aoc_2025_01 {
    typedef struct {
      char dir;
      int  dist;
    } Entry;
    vector<Entry> doc;
    void read_document() {
      string line;
      while(getline(cin, line)) {
        auto dir = line.at(0);
        auto sdist = line.substr(1);
        doc.push_back({dir, atoi(sdist.c_str())});
      }
    }
    extern "C" void part1() {
      read_document();
      int pos = 50;
      int cnt_poiting_at_zero = 0;
      for(const auto &entry:doc) {
        switch(entry.dir) {
          case 'L': {
              pos = mymath::mod(pos - entry.dist, 100);
            }
            break;
          case 'R': {
              pos = mymath::mod(pos + entry.dist, 100);
            }
            break;
        }
        if(pos == 0) {
          cnt_poiting_at_zero++;
        }
      }
      cout << "Part 1: " << cnt_poiting_at_zero << endl;
    }
    extern "C" void part2() {
      read_document();
      int pos = 50;
      int next_pos = 0;
      int cnt_poiting_at_zero = 0;
      for(const auto &entry:doc) {
        switch(entry.dir) {
          case 'L': {
              next_pos = pos - entry.dist;
              // determin how many numbers mod 100 are present in interval [next_pos, pos)
              // [next_pos,mod,..,next_mod,pos)
              /*
              auto next_mod = pos - (mod(pos,100));
              assert(mod(next_mod,100)==0);
              while(in_lc_ro_interval(next_pos, pos, next_mod)) {
                cnt_poiting_at_zero++;
                next_mod -= 100;
              }
              */
              for(int i = next_pos; i < pos; i++) {
                if(mod(i,100) == 0) cnt_poiting_at_zero++;
              }
            }
            break;
          case 'R': {
              next_pos = pos + entry.dist;
              // determin how many numbers mod 100 are present in interval (pos, next_pos]
              // (pos,next_mod,..,mod,next_pos]
              /*
              auto next_mod = pos + (100 - mod(pos,100));
              assert(mod(next_mod,100)==0);
              while(in_lo_rc_interval(pos, next_pos, next_mod)) {
                cnt_poiting_at_zero++;
                next_mod += 100;
              }
              */
              for(int i = pos+1; i <= next_pos; i++) {
                if(mod(i,100) == 0) cnt_poiting_at_zero++;
              }
            }
            break;
        }
        pos = mymath::mod(next_pos, 100);
      }
      // 6561 too high
      cout << "Part 2: " << cnt_poiting_at_zero << endl;
    }
}
