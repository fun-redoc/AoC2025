//
// Created by rsh on 2025-12-16.
//

#include <algorithm>
#include <map>
#include <iostream>
#include <vector>
#include <regex>
#include <cmath>
#include <optional>
#include <cassert>
#include <deque>
#include "../include/aoc2025_08.hpp"

typedef long long ll;

using namespace std;
namespace aoc_2025_08 {
    typedef tuple<ll,ll,ll> V3;
    bool operator==(const V3 &lhs, const V3 &rhs) {
            return get<0>(lhs) == get<0>(rhs) &&
                   get<1>(lhs) == get<1>(rhs) &&
                   get<2>(lhs) == get<2>(rhs) ;
    }

    struct JunctionKeyEqual {
        bool operator()(const pair<int,int>& a, const pair<int,int>& b) const noexcept {
            return get<0>(a) == get<0>(b) &&
                   get<1>(a) == get<1>(b);
                   //&&
                   //get<2>(a) == get<2>(b) ;
        }
    };
    struct JunctionKeyHash {
        bool operator()(const pair<int,int>& p) const noexcept {
            return std::hash<int>()(get<0>(p)) ^
                  (std::hash<int>()(get<1>(p)) << 1); 
                  //^ 
                  //(std::hash<int>()(get<2>(p)) << 2)
                  //;
        }
    };
    typedef vector<V3> Puzzle;
    ostream & operator<<(ostream &oss, const V3 &v) {
      oss << "[" << get<0>(v) << "," <<  get<1>(v) << "," << get<2>(v) << "]";
      return oss;
    }
    void read_puzzle(Puzzle &puzzle) {
      string line;
      regex rx(R"((\d+),(\d+),(\d+))");
      smatch match;
      while(getline(cin, line)) {
        if(regex_match(line, match, rx)) {
            int x = stoi(match[1].str());
            int y = stoi(match[2].str());
            int z = stoi(match[3].str());
            puzzle.push_back({x,y,z});
        } else {
          cerr << "Invalid line format: " << line << endl;
        }
      }
    }
    double euclidian(const V3 &a, const V3 &b) {
      return (pow(get<0>(a) - get<0>(b), 2) +
              pow(get<1>(a) - get<1>(b), 2) +
              pow(get<2>(a) - get<2>(b), 2));
//      return sqrt(pow(get<0>(a) - get<0>(b), 2) +
//                  pow(get<1>(a) - get<1>(b), 2) +
//                  pow(get<2>(a) - get<2>(b), 2));
//
    }
    bool in_cirquit(const deque<int> &circuit, int box_idx) {
      for(const auto &b:circuit) {
        if(b == box_idx) {
          return true;
        }
      }
      return false;
    }
    extern "C" void part1() {
      const size_t BATCH_SIZE = 1000;
      Puzzle puzzle;
      read_puzzle(puzzle);

      int cnt = puzzle.size();
      map<pair<int, int>, double> junctions;
      for(int i=0; i<cnt; i++) {
        for(int j=i+1; j<cnt;j++) {
          double dist = euclidian(puzzle[i], puzzle[j]);
          junctions[make_pair(i,j)] = dist;
        }
      }
      assert(junctions.size() == puzzle.size()*(puzzle.size() - 1)/2);
      vector<tuple<int,int,double>> sorted_junctions;

#pragma GCC diagnostic ignored "-Wrange-loop-construct"
      for(const auto j:junctions) {
        sorted_junctions.push_back({j.first.first, j.first.second, j.second});
      }
      assert(sorted_junctions.size() == puzzle.size()*(puzzle.size() - 1)/2);
      
      sort(sorted_junctions.begin(), sorted_junctions.end(), [](auto &j1, auto &j2) { return get<2>(j1) < get<2>(j2); });
      assert(sorted_junctions.size() == puzzle.size()*(puzzle.size() - 1)/2);

      size_t i = 0;
      //for(const auto &j:sorted_junctions) {
      //  if(i >= BATCH_SIZE) break;
      //  cout << i << ": "  << "[" << puzzle[get<0>(j)] << "," << puzzle[get<1>(j)] << "," << get<2>(j) << "]" << endl;
      //  i++;
      //}

      vector<pair<deque<int>, double>> cirquits;
      for(int i=0; i<(int)puzzle.size(); i++) {
          cirquits.push_back(make_pair(deque<int>({i}), 0));
      }
      i = 0;
      //auto jit = sorted_junctions.cbegin();
      //if(jit == sorted_junctions.cend()) exit(1); // no junctions!!
      //while(i <= batch && jit != sorted_junctions.cend()) {
      while(i < BATCH_SIZE) {
        int j0 = get<0>(sorted_junctions[i]);
        int j1 = get<1>(sorted_junctions[i]);
        optional<int> in_c[] = {nullopt, nullopt};
        for(size_t ci=0; ci<cirquits.size() && (!in_c[0] || !in_c[1]); ci++){
          auto c  = cirquits[ci];
          if(in_cirquit(c.first, j0)) {
            assert(!in_c[0]);
            in_c[0] = ci;
          }
          if(in_cirquit(c.first, j1)) {
            assert(!in_c[1]);
            in_c[1] = ci;
          }
        }
        if(in_c[0] && in_c[1] && *in_c[0] != *in_c[1]) {
              // merge 2 cirquits to back
            for(const auto &b:cirquits[*in_c[1]].first) {
              cirquits[*in_c[0]].first.push_back(b);
            }
            cirquits[*in_c[0]].second += get<2>(sorted_junctions[i]);
            cirquits[*in_c[1]].first.clear();
            cirquits.erase(cirquits.begin() + *in_c[1]);

        }
        else
          if(in_c[0] && in_c[1] && *in_c[0] == *in_c[1]) {
            // skip
          }
        else {
          // unexpeced
          asm("int $3");
        }
        ++i;
      }
      sort(cirquits.begin(), cirquits.end(), [](const auto &c1, const auto &c2) {
          return c1.first.size() > c2.first.size();
      });
      i = 0;
      ll res = 1;
      for(const auto &c:cirquits) {
        if(i>=3) break;
        cout << "cirquit size = " << c.first.size() << "cirquit len" << c.second << endl;
        res *= c.first.size();
        i++;
      }
      cout << "Part 1:" << res << endl;
    }
    extern "C" void part2() {
      Puzzle puzzle;
      read_puzzle(puzzle);

      int cnt = puzzle.size();
      map<pair<int, int>, double> junctions;
      for(int i=0; i<cnt; i++) {
        for(int j=i+1; j<cnt;j++) {
          double dist = euclidian(puzzle[i], puzzle[j]);
          junctions[make_pair(i,j)] = dist;
        }
      }
      assert(junctions.size() == puzzle.size()*(puzzle.size() - 1)/2);
      vector<tuple<int,int,double>> sorted_junctions;

#pragma GCC diagnostic ignored "-Wrange-loop-construct"
      for(const auto j:junctions) {
        sorted_junctions.push_back({j.first.first, j.first.second, j.second});
      }
      assert(sorted_junctions.size() == puzzle.size()*(puzzle.size() - 1)/2);
      
      sort(sorted_junctions.begin(), sorted_junctions.end(), [](auto &j1, auto &j2) { return get<2>(j1) < get<2>(j2); });
      assert(sorted_junctions.size() == puzzle.size()*(puzzle.size() - 1)/2);

      size_t i = 0;

      vector<deque<int>> cirquits;
      for(int i=0; i<(int)puzzle.size(); i++) {
          cirquits.push_back(deque<int>({i}));
      }
      i = 0;
      int j0;
      int j1;
      while(cirquits.size() > 1) {
        j0 = get<0>(sorted_junctions[i]);
        j1 = get<1>(sorted_junctions[i]);
        optional<int> in_c[] = {nullopt, nullopt};
        for(size_t ci=0; ci<cirquits.size() && (!in_c[0] || !in_c[1]); ci++){
          auto c  = cirquits[ci];
          if(in_cirquit(c, j0)) {
            assert(!in_c[0]);
            in_c[0] = ci;
          }
          if(in_cirquit(c, j1)) {
            assert(!in_c[1]);
            in_c[1] = ci;
          }
        }
        if(in_c[0] && in_c[1] && *in_c[0] != *in_c[1]) {
              // merge 2 cirquits to back
            for(const auto &b:cirquits[*in_c[1]]) {
              cirquits[*in_c[0]].push_back(b);
            }
            cirquits[*in_c[1]].clear();
            cirquits.erase(cirquits.begin() + *in_c[1]);

        }
        else
          if(in_c[0] && in_c[1] && *in_c[0] == *in_c[1]) {
            // skip
          }
        else {
          // unexpeced
          asm("int $3");
        }
        ++i;
      }
      cout << "Part 2:"<< puzzle[j0] << "*" << puzzle[j1] << "=" << get<0>(puzzle[j0]) * get<0>(puzzle[j1]) << endl;
    }
}
