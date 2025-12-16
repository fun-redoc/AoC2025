//
// Created by rsh on 2025-12-09.
//

#include <cassert>
#include <numeric>
#include <iomanip>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>
#include <map>
#include <queue>
#include <optional>
#include "../include/aoc2025_07.hpp"

typedef long long ll;

using namespace std;
namespace aoc_2025_07 {
    // returns Startfield S
    pair<int,int> read_diagram(vector<vector<char>> &diagram) {
      pair<int,int> res;
      string line;
      vector<char> cline;
      bool starting = true;
      int row = 0;
      while(getline(cin, line)) {
        if(starting) {
          cline = vector<char>(line.size());
          starting = false;
        }
        for(size_t i=0; i<line.size(); i++) {
          cline[i] = line[i];
          if(cline[i] == 'S') {
            res.first = row;
            res.second = i;
          }
        }
        diagram.push_back(cline);
        row++;
      }
      return res;
    }
        // Custom hash for pair<int,int>
    struct PairHash {
        std::size_t operator()(const std::pair<int,int>& p) const noexcept {
            // Combine hashes of first and second
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };

    // Custom equality comparator (optional, but good practice)
    struct PairEqual {
        bool operator()(const std::pair<int,int>& a, const std::pair<int,int>& b) const noexcept {
            return a.first == b.first && a.second == b.second;
        }
    };
        // Custom hash for pair<int,int>
    struct TimelineHash {
        std::size_t operator()(const std::tuple<int,int,ll>& p) const noexcept {
            // Combine hashes of first and second
            return std::hash<int>()(get<0>(p))
              ^ (std::hash<int>()(get<1>(p)) << 1)
              ; //^ (hash<ll>()(get<2>(p)) << 2);
        }
    };

    // Custom equality comparator (optional, but good practice)
    struct TimelineEqual {
        bool operator()(const std::tuple<int,int, ll>& a, const std::tuple<int,int, ll>& b) const noexcept {
            return get<0>(a)==get<0>(b) &&
                   get<1>(a)==get<1>(b)
            ; //&& get<2>(a)==get<2>(b) ;

        }
    };
    struct BeamHash {
        std::size_t operator()(const std::tuple<int,int,ll>& p) const noexcept {
            // Combine hashes of first and second
            return std::hash<int>()(get<0>(p))
              ^ (std::hash<int>()(get<1>(p)) << 1)
              ^ (hash<ll>()(get<2>(p)) << 2);
        }
    };

    // Custom equality comparator (optional, but good practice)
    struct BeamEqual {
        bool operator()(const std::tuple<int,int, ll>& a, const std::tuple<int,int, ll>& b) const noexcept {
            return get<0>(a)==get<0>(b) &&
                   get<1>(a)==get<1>(b)
            && get<2>(a)==get<2>(b) ;

        }
    };
    optional<pair<int,int>> beam_old(const map<int,int> &splitters, const pair<int,int> &from) {
      auto col = from.second;
      try {
        auto hit_row = splitters.at(col);
        return make_pair(hit_row, col);
      }
      catch(const std::out_of_range& _)
      {
          return nullopt;
      }
    }
    optional<pair<int,int>> beam_to_next_splitter(const multimap<int,int> &splitters, const pair<int,int> &from) {
      auto [row,col] = from;
      auto hit_row_it = splitters.upper_bound(row);
      optional<int> hit_row = nullopt;
      for(; hit_row_it != splitters.end(); ++hit_row_it) {
        if(hit_row_it->second == col) {
          hit_row = hit_row_it->first;
          break;
        }
      }
      if(!hit_row) {
        return nullopt;
      } else {
        return make_pair(*hit_row, col);
      }
      //return hit_row.and_then([col](auto row) {
      //    return make_optional(make_pair(row, col));
      //});
    }
    vector<pair<int,int>> beam_to_next_splitters(const multimap<int,int> &splitters, const pair<int,int> &from) {
      vector<pair<int,int>> res;
      auto [row,col] = from;
      auto hit_row_it = splitters.upper_bound(row);
      int new_row = hit_row_it->first;
      for(; hit_row_it != splitters.end(); ++hit_row_it) {
        if(hit_row_it->first != new_row) break;
        if(hit_row_it->second == col - 1) {
          res.push_back(*hit_row_it);
        }
        if(hit_row_it->second == col + 1) {
          res.push_back(*hit_row_it);
        }
      }
      return res;
    }
    pair<optional<pair<int,int>>,vector<pair<int,int>>> beam_to_next_beams(const vector<vector<char>> &diagram, const pair<int,int> &from) {
      auto [row,col] = from;
      for(int r=row+1;r<(int)diagram.size(); r++) {
        if(diagram[r][col] == '^') {
          return {{{r,col}},{{r,col-1},{r,col+1}}};
        }
      }
      return {nullopt,{}};
    }
    bool in_range(const vector<vector<char>> &diagram, const tuple<int,int> &pos) {
      return 0 <= get<0>(pos)  && get<0>(pos)  < (int)diagram.size() &&
             0 <= get<1>(pos) && get<1>(pos) < (int)diagram.at(0).size();
    }
    bool in_range(const vector<vector<char>> &diagram, const tuple<int,int, ll> &pos) {
      return 0 <= get<0>(pos)  && get<0>(pos)  < (int)diagram.size() &&
             0 <= get<1>(pos) && get<1>(pos) < (int)diagram.at(0).size();
    }
    bool in_range(const vector<vector<char>> &diagram, const pair<int,int> &pos) {
      return 0 <= get<0>(pos)  && get<0>(pos)  < (int)diagram.size() &&
             0 <= get<1>(pos) && get<1>(pos) < (int)diagram.at(0).size();
    }
    ostream & operator<<(ostream & oss, const pair<int,int> p) {
      oss << "[" << p.first <<  "," << p.second << "]";
      return oss;
    }
    ostream & operator<<(ostream & oss, const tuple<int,int,ll> p) {
      oss << "[" << get<0>(p) <<  "," << get<1>(p) << "," << get<2>(p) <<  "]";
      return oss;
    }
    ostream & operator<<(ostream & oss, const unordered_set<pair<int,int>, BeamHash, BeamEqual> &ss) {
      for(const auto &s:ss) {
        oss << s << " ";
      }
      oss << endl;
      return oss;
    }
    ostream & operator<<(ostream & oss, const unordered_set<tuple<int,int, ll>, BeamHash, BeamEqual> &ss) {
      for(const auto &s:ss) {
        oss << s << " ";
      }
      oss << endl;
      return oss;
    }
    ostream & operator<<(ostream & oss, const multiset<tuple<int,int, ll>> &ss) {
      for(const auto &s:ss) {
        oss << s << " ";
      }
      oss << endl;
      return oss;
    }
    ostream & operator<<(ostream & oss, const vector<pair<int,int>> &ss) {
      int old_row = 0;
      for(const auto &s:ss) {
        if(old_row != s.first) oss << endl;
        oss << s << " ";
          old_row = s.first;
      }
      oss << endl;
      return oss;
    }
    ostream & operator<<(ostream & oss, const vector<tuple<int,int, ll>> &ss) {
      int old_row = 0;
      for(const auto &s:ss) {
        if(old_row != get<0>(s)) oss << endl;
        oss << s << " ";
          old_row = get<0>(s);
      }
      oss << endl;
      return oss;
    }
    ostream & operator<<(ostream & oss, const vector<vector<char>> &diagram) {
      int row = 0;
      for(const auto &r:diagram) {
        oss << setw(5) << setfill('0') << row;
        for(char c:r) {
          oss << c;
        }
        oss << endl;
        row++;
      }
      oss << endl;
      return oss;
    }
    extern "C" void part1() {
      vector<vector<char>> diagram;
      auto startfield = read_diagram(diagram);
      // create a set of spitters
      multimap<int,int> splitters;
      for(size_t row=0; row<diagram.size(); row++) {
        for(size_t col=0; col<diagram[row].size(); col++) {
          if(diagram[row][col] == '^') {
            splitters.insert({row, col});
          }
        }
      }

      unordered_set<pair<int,int>, PairHash, PairEqual>  all_splits;
      unordered_set<pair<int,int>, PairHash, PairEqual> all_beams;
      queue<pair<int,int>> current_splits;
      //auto first_splitter =  beam_to_next_splitter(splitters, startfield);
      //current_splits.push(first_splitter.value());
      current_splits.push(startfield);
      all_beams.insert(startfield);
      while(!current_splits.empty()) {
        auto beam_start = current_splits.front(); current_splits.pop();
        all_beams.insert(beam_start);
        //auto next_splitters =  beam_to_next_splitters(splitters, beam_start);
        auto [cur_spliltter, next_beams] =  beam_to_next_beams(diagram, beam_start);
        if(cur_spliltter && !all_splits.count(*cur_spliltter)) all_splits.insert(*cur_spliltter);
        for(const auto &next_beam:next_beams) {
          if(in_range(diagram, next_beam)  && !all_beams.count(next_beam)) {
            all_beams.insert(next_beam);
            current_splits.push(next_beam);
          }          
        }
      }
      int res = all_splits.size();
      cout << "Part 1: " << res  << endl;
      
    }

    extern "C" void part2() {
      vector<vector<char>> diagram;
      auto [start_row, start_col] = read_diagram(diagram);

      vector<vector<ll>> timeline(diagram.size(), vector<ll>(diagram[0].size(), 0));

      timeline[start_row][start_col] = 1LL;
      for(size_t r=0; r<timeline.size()-1; r++) {
        for(size_t c=0; c <timeline[r].size(); c++) {
          if(diagram[r][c] == '^') {
            timeline[r+1][c-1] += timeline[r][c];
            timeline[r+1][c+1] += timeline[r][c];
          } else {
            timeline[r+1][c] += timeline[r][c];
          }
        }
      }

      ll num_timlines = reduce(timeline[timeline.size()-1].begin(), timeline[timeline.size()-1].end());

      cout << "Part 2: " <<  num_timlines << endl;
    }
}
