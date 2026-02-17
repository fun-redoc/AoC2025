//
// Created by rsh on 2026-01-10.
//

#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <ios>
#include <limits>
#include <sstream>
#include <vector>
#include <regex>
#include <cstdio>
#include <ostream>
#include <vector>
#include <regex>
#include <math.h>
#include <unordered_set>
#include <queue>
#include <tuple>
#include <iostream>
#include "../include/utils.hpp"
#include "../include/mymath.hpp"
#include "../include/simplex.hpp"
#include "../include/aoc2025_10.hpp"


typedef long long ll;

using namespace std;
using namespace utils;
using namespace simplex;
namespace aoc_2025_10 {
  using utils::operator<<;  // unclear why, but needed otherwhise the compiler does not find the overloaded operator from utils.hpp
  //using simplex::operator<<;  // unclear why, but needed otherwhise the compiler does not find the overloaded operator from utils.hpp


  typedef struct {
    size_t nr;
    string indicators;
    vector<vector<int>> buttons;
    vector<long> joltage;
    ll hash_joltage;
    vector<size_t> switch_sequence;
    ll joltage_steps; 
  } ManualEntry;

  void read_manual(vector<ManualEntry> &manual) {
    manual.clear();
    string line;
    regex pat1(R"(\[([\.\#]+)\])"); 
    regex pat2(R"(\d+)"); 
    regex pat3(R"(([\{\}\(\)\,\d+]+))"); 
    smatch match;
    size_t me_nr = 0;
    while(getline(cin, line)) {
      ManualEntry me;
      me.nr = me_nr++;
      istringstream iss(line);
      do {
        string part;
        iss >> part;
        if(regex_match(part, match, pat1)) {
          // [.##.] case
          me.indicators = match[1];  
        } 
        if(part[0] == '(') {
          auto begin = std::sregex_iterator(part.begin(), part.end(), pat2);
          auto end = std::sregex_iterator();
          assert(*(part.end() - 1) == ')');
          if(begin != end) {
            vector<int> button_list;
            for (std::sregex_iterator it = begin; it != end; ++it) {
                // Convert the matched string to an integer
                button_list.push_back(stoi(it->str()));
            }
            me.buttons.push_back(button_list);
          }
        }
        if(part[0] == '{') {
          auto begin = std::sregex_iterator(part.begin(), part.end(), pat2);
          auto end = std::sregex_iterator();
          assert(*(part.end() - 1) == '}');
          if(begin != end) {
            vector<int> button_list;
            for (std::sregex_iterator it = begin; it != end; ++it) {
                // Convert the matched string to an integer
                me.joltage.push_back(stol(it->str()));
            }
          }
        }
      } while(iss);
      me.switch_sequence = vector<size_t>();
      me.joltage_steps = 0;
      me.hash_joltage = hash_vec(me.joltage);
      manual.push_back(me);
    }
  }
  string flip(const string &indicators, int idx) {
    string flipped_indicators(indicators);
    flipped_indicators.replace(idx, 1, indicators[idx] == '.' ? "#":".");
    return flipped_indicators;
  }

  extern "C" void part1() {
    vector<ManualEntry> manual;
    read_manual(manual);

    for(ManualEntry &me:manual) {
      queue<pair<string, vector<size_t>>> q;
      for(size_t i=0; i<me.buttons.size(); i++) {
        string initial_indicators(me.indicators.size(), '.');
        for( int btn:me.buttons[i]) {
          initial_indicators = flip(initial_indicators, btn);
        }
        q.push({initial_indicators,{i}});
      }
      while(!q.empty()) {
        auto [inds, steps] = q.front(); q.pop();
        if(inds == me.indicators &&
                   (me.switch_sequence.empty() ||
                    steps.size() < me.switch_sequence.size())) {
          me.switch_sequence = steps;
          break;
        }
        for(size_t i=0; i<me.buttons.size(); i++) {
          string new_inds = inds;
          for(const auto& btn:me.buttons[i]) {
            new_inds = flip(new_inds,btn);
          }
          vector<size_t> new_steps(steps);
          new_steps.push_back(i);
          q.push({new_inds, new_steps});
        }
      }
    }
    ll res = 0;
    for(const auto& me:manual) {
      res += me.switch_sequence.size();
    }
    cout << "Part 1: "
         << res
         << endl;
  }
  template<typename T>
  vector<T> switch_joltage(const vector<T> &joltage, int idx) {
    vector<T> new_joltage(joltage);
    new_joltage[idx]++;
    return new_joltage;
  }

 
  void switches_for_joltage_to_much_memory(ManualEntry &me) {
      stack<pair<vector<long>, ll>> q; // until now reached joltage, steps
      unordered_set<vector<long>, VecHash<long>> ready;
      for(size_t i=0; i<me.buttons.size(); i++) {
        vector<long> initial_joltage(me.joltage.size(), 0);
        for( int btn:me.buttons[i]) {
          initial_joltage = switch_joltage(initial_joltage, btn);
        }
        q.push({initial_joltage,1});
      }
      while(!q.empty()) {
        auto [jltgs, steps] = q.top(); q.pop();
        if(me.joltage_steps != 0 && steps >= me.joltage_steps) continue;
        if(ready.contains(jltgs)) continue;
        if(jltgs == me.joltage &&
                   (me.joltage_steps == 0 ||
                    steps < me.joltage_steps)) {
          me.joltage_steps = steps;
          continue;
        }
        ready.insert(jltgs);
        for(size_t i=0; i<me.buttons.size(); i++) {
          vector<long> new_jltgs = jltgs;
          for(const auto& btn:me.buttons[i]) {
            new_jltgs = switch_joltage(new_jltgs,btn);
          }
          bool do_push = true;
          for(size_t j=0; j<me.joltage.size(); j++) {
            if(new_jltgs[j] > me.joltage[j]) {
              do_push = false;
              break;
            }
          }
          if(do_push) {
            //if(ready.count(new_jltgs) == 0) {
              q.push({new_jltgs, steps+1});
            //}
          }
        }
      }
  }




  void switches_for_joltage(ManualEntry &me) {
    size_t rows = me.joltage.size();
    size_t cols = me.buttons.size();
    vector<vector<long>> btn_matrix(rows, vector<long>(cols, 0));
    for(size_t c=0; c<cols; c++) {
      for(size_t btn:me.buttons[c]) {
        btn_matrix[btn][c]++;
      }
    }
    //cout << btn_matrix << endl;

    vector<LinearProgram<double>::Constraint> constraints;
    for(size_t r=0; r<btn_matrix.size(); r++) {
      LinearProgram<double>::Constraint constraint;
      constraint.lhs = vector<double>(cols);
      for(size_t c=0; c<btn_matrix[0].size(); c++) {
        constraint.lhs[c] = btn_matrix[r][c];
      }
      constraint.rel = LinearProgram<double>::Comp::EQ;
      constraint.rhs = me.joltage[r];
      constraints.push_back(constraint);
    }
    vector<LinearProgram<double>::Restricted> target(cols, LinearProgram<double>::restricted(1.0));
    LinearProgram<double> lp{constraints, LinearProgram<double>::SimplexKind::MINIMIZE, {target, 0.0}};
    auto res = LinearProgram<double>::simplex(lp);
    //cout << lp << endl;
    //cout << "res: ";
    //LinearProgram<double>::print(cout, *res);
    //cout << endl; 
    assert(res);
    me.joltage_steps = res->opt_val;
  }
 
  extern "C" void part2() {
    vector<ManualEntry> manual;
    read_manual(manual);

    for(ManualEntry &me:manual) {
      auto t = measure([&me]() {switches_for_joltage(me);});
      cout << unitbuf;
      cout << "Manual Entry: " << me.nr <<  " Time: " << t.count() << " µs " << t.count()/1E6 << " s "  << endl << flush;
      cout << nounitbuf;
    }
    ll res = 0;
    for(const auto& me:manual) {
      cout << me.joltage << " needs " << me.joltage_steps << " steps." << endl;
      res += me.joltage_steps;
    }
    cout << "Part 2: "
         << res
         << endl;
  }
}
