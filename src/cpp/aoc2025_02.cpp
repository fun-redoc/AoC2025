//
// Created by rsh on 2025-12-02.
//

#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <cassert>
#include "../include/aoc2025_02.hpp"

typedef long long ll;

using namespace std;
namespace aoc_2025_02 {
    typedef pair<ll,ll> Range ;
    vector<Range> ranges;
    void read_ranges() {
      string line;
      regex re(R"((\d+)-(\d+))");
      while(getline(cin, line)) {
        auto begin =  sregex_iterator(line.begin(), line.end(), re);
        auto end   =  sregex_iterator();
        for(auto it = begin; it != end; ++it) {
          string sx = (*it)[1].str();
          string sy = (*it)[2].str();
          assert(sx.at(0) != '0' && sy.at(0) != '0');
          ll x = std::stoll((*it)[1].str());
          ll y = std::stoll((*it)[2].str());
          assert(x > 0 && y > 0);
          assert(x < y);
          ranges.push_back({x, y});
        }
      }
    }
    bool check_invalid_doubles(ll id) {
      string sid = to_string(id);
      size_t l = sid.length();
      if(l%2 != 0) return false;
      size_t l2 = l/2;
      for(size_t i=0; i < l2; i++) {
        if(sid.at(i) != sid.at(i+l2)) return false;
      }
      return true;
    }
    bool check_invalid(string sid, ll reps) {
      auto l = sid.length();
      assert(l%reps==0);
      auto lr = l / reps;
      for(size_t i=0; i<lr; i++) {
        char c = sid.at(i);
        for(int r=1; r < reps; r++) {
          if(c != sid.at(i + r*lr)) return false;
        }
      }
      // 121212
      // ^ ^ ^
      // 0 2 4
      //  1 3 5
      return true;
    }
    extern "C" void part1() {
      read_ranges();
      ll add_up = 0;
      for(const auto &range:ranges) {
        for(ll i=range.first; i<=range.second; i++) {
          if(check_invalid_doubles(i)) {
            add_up += i;
          }
        }
      }
      cout << "Part 1: " << add_up << endl;
    }
    extern "C" void part2() {
      read_ranges();
      ll add_up = 0;
      for(const auto &range:ranges) {
        for(ll i=range.first; i<=range.second; i++) {
          string sid = to_string(i);
          ll l = sid.length();
          for(int reps=l; reps>=2; reps--) {
            if((l%reps==0) && check_invalid(sid, reps)) {
              add_up += i;
              break;
            }
          }
        }
      }
      cout << "Part 2: " << add_up << endl;
    }
}
