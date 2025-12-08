//
// Created by rsh on 2025-12-06.
//

#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <vector>
#include <regex>
#include "../include/aoc2025_05.hpp"

typedef long long ll;

using namespace std;
namespace aoc_2025_05 {
    typedef pair<ll,ll> Range;
    vector<Range> ranges;
    ll range_min = LLONG_MAX;
    ll range_max = LLONG_MIN;
    vector<ll> ingredients;
    void read_database() {
      ranges.clear();
      ingredients.clear();
      string line;
      // read ranges
      regex rx_ranges(R"((\d+)-(\d+))");
      while(getline(cin, line) && !line.empty()) {
        auto begin =  sregex_iterator(line.begin(), line.end(), rx_ranges);
        auto end   =  sregex_iterator();
        int i = 0;
        for(auto it = begin; it != end; ++it) {
          string sfrom = (*it)[1].str();
          string sto   = (*it)[2].str();
          ll from = std::stoll((*it)[1].str());
          ll to = std::stoll((*it)[2].str());
          assert(from > 0 && to > 0 && from <= to);
          ranges.push_back({from, to});
          range_min = min(range_min, from);
          range_max = max(range_max, to);
          i++;
        }
        assert(i==1);
      }
      // read ingredients
      while(getline(cin, line)) {
          ll id = std::stoll(line.c_str());
          ingredients.push_back(id);
      }
      
    }
    ostream & operator<<(ostream &oss, const vector<Range> &rs) {
      for(const auto &r:rs) {
        oss << "[" << r.first << "-" << r.second << "] ";
      }
      return oss;
    }

    extern "C" void part1() {
      read_database();

      ll res = 0;
      
      sort(ranges.begin(), ranges.end(),[](auto a, auto b){ return a.first < b.first;});

      for(const auto i:ingredients) {
        for(const auto r:ranges) {
          if(r.first <= i && i <= r.second) {
            res++;
            break;
          }
        }
      }

      cout << "Part 1: " << res << endl;

    }

    ll naive1_part2(vector<Range> rs, ll min, ll max) {
      auto d = max-min+1;
      ll cnt_fresh = 0;
      //vector<bool> fresh(d,false);  
      for(ll i=min; i<=max; i++) {
        for(const auto &r:rs) {
          if(r.first <= i && i <= r.second) {
            //fresh[i] = true;
            cnt_fresh++;
            break;
          }
        }
      }
      return cnt_fresh;
    }

    ll naive2_part2(vector<Range> rs, ll min, ll max) {
      auto d = max-min+1;
      ll cnt_fresh = 0;
      vector<bool> fresh(d,false);  
      for(const auto &r:rs) {
        for(ll i=r.first; i<=r.second; i++) {
          if(!fresh[i-min]) {
            fresh[i-min] = true;
            cnt_fresh++;
          }
        }
      }
      return cnt_fresh;
    }

    void intersect(const Range &v1, const Range &v2, vector<Range> &intov) {
      if(v1.second < v2.first) {
        return;
      }
      if(v2.second < v1.first) {
        return;
      }
      // number of cases: 3*3
      if(v1.first < v2.first && v1.second < v2.second) {
        //.. v1 and v2 intersect
        intov.push_back({v1.first, v2.first-1});
        intov.push_back({v2.first, v1.second-1});
        intov.push_back({v2.second, v2.second});
        return;
      }     
      if(v1.first < v2.first && v1.second > v2.second) {
        //.. v1 contains v2 => v2 disapears
        intov.push_back({v1.second, v1.second});
        return;
      }     
      if(v1.first < v2.first && v1.second == v2.second) {
        //..
        intov.push_back({v1.first, v2.first-1});
        intov.push_back({v2.first, v2.second});
        return;
      }     
      if(v1.first > v2.first && v1.second < v2.second) {
        //.. v2 constains v1 -> v1 disappears
        intov.push_back({v2.second, v2.second});
        return;
      }     
      if(v1.first > v2.first && v1.second > v2.second) {
        //.. v1 and v2 interset
        intov.push_back({v2.first, v1.first-1});
        intov.push_back({v1.first, v2.second-1});
        intov.push_back({v2.second, v1.second});
        return;
      }     
      if(v1.first > v2.first && v1.second == v2.second) {
        //..
        intov.push_back({v2.first, v1.first-1});
        intov.push_back({v1.first, v2.second});
        return;
      }     
      if(v1.first == v2.first && v1.second < v2.second) {
        //..
        intov.push_back({v1.first, v1.second-1});
        intov.push_back({v1.second, v2.second});
        return;
      }     
      if(v1.first == v2.first && v1.second > v2.second) {
        //..
        intov.push_back({v1.first, v2.second-1});
        intov.push_back({v2.second, v1.second});
        return;
      }     
      if(v1.first == v2.first && v1.second == v2.second) {
        //..intervals are equal one of them disapears
        intov.push_back({v1.first, v1.second});
        return;
      }     
    }

    ll try_part2(vector<Range> rs, ll min, ll max) {
      if(rs.empty()) return 0;

      ll cnt_fresh = 0;

      sort(rs.begin(), rs.end(), [](auto a, auto b){ return (a.first < b.first ) || (a.first == b.first && a.first < b.first);});

      vector<Range> merged;
      Range r = rs[0];
      for (size_t i = 1; i < rs.size(); ++i) {
          if (rs[i].first <= r.second) {
              // Overlap → extend
              r.second = std::max(r.second, rs[i].second);
          } else {
              // No overlap → push r and reset
              merged.push_back(r);
              r = rs[i];
          }
      }
      merged.push_back(r);

      //cout << rs << " ## " << merged << endl;

      for(const auto &r:merged) {
        cnt_fresh += (r.second - r.first + 1);
      }

      return cnt_fresh;
    }
    

    extern "C" void part2() {
      read_database();

      //  1-12 2-4 6-8 7-9                      

      //  1-12 | 2-14 5-18 7-12                      

      //ll res_naive = naive1_part2(ranges, range_min, range_max);
      ll res = try_part2(ranges, range_min, range_max);
      //assert(res_naive == res);

      cout << "Part 2: " << res << endl;

    }
}
