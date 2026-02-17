//
// Created by rsh on 2025-12-09.
//

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <regex>
#include <numeric>
#include <cassert>
#include <climits>
#include "../include/aoc2025_06.hpp"

typedef long long ll;

using namespace std;
namespace aoc_2025_06 {
    typedef struct {
      char op;
      vector<ll> numbers;
      vector<string> cephalopods;
      int max_len;
      ll result;
    } Task;
    vector<Task> tasks;
    void read_task() {
      vector<vector<string>> rows;
      string line;
      regex r_number(R"((\d+))");
      regex r_op(R"(([+*]))");
      while(getline(cin, line)) {
        sregex_iterator begin = sregex_iterator(line.begin(), line.end(), r_number);
        sregex_iterator end   = sregex_iterator();
        if(begin == end) {
          // last line reached
          begin = sregex_iterator(line.begin(), line.end(), r_op);
          end   = sregex_iterator();
          for(auto it=begin; it!=end; ++it) {
            if(it==begin) {
              rows.push_back(vector<string>());
            }
            rows.back().push_back((*it)[1].str());
          }
        } else {
          for(auto it=begin; it!=end; ++it) {
            if(it==begin) {
              rows.push_back(vector<string>());
            }
            rows.back().push_back((*it)[1].str());
          }
        }
      }   
      // now transpose
      for(size_t i=0; i<rows.front().size(); i++) {
        tasks.push_back({'_', vector<ll>(), vector<string>(), INT_MIN, 0});
      }
      for(size_t i=0; i<rows.size() - 1; i++) {
        for(size_t j=0; j<rows[i].size(); j++) {
          tasks[j].numbers.push_back(stoll(rows[i][j]));
          tasks[j].cephalopods.push_back(rows[i][j]);
          tasks[j].max_len = std::max(tasks[j].max_len, (int)(rows[i][j].size()));
        }
      }
      size_t i = rows.size() -1;
      for(size_t j=0; j<rows[i].size(); j++) {
        tasks[j].op = rows[i][j][0];
      }
    }
    extern "C" void part1() {

      read_task();
      ll res = 0;
      for(auto &t:tasks) {
        switch(t.op) {
          case '*':
            t.result = reduce(t.numbers.begin(), t.numbers.end(), 1LL, multiplies<ll>());
            break;
          case '+':
            t.result = reduce(t.numbers.begin(), t.numbers.end(), 0LL);
            break;
          default:
            cerr << "unknown: " << t.op << "." << endl;
            exit(1);
            break;
        }
        assert(t.result > 0);
        res += t.result;
      }

      cout << "Part 1: " << res << endl;
    }
    extern "C" void part2() {
      ll res = 0;
      // read
      string line;
      vector<string> lines;
      //int cols = 0;
      while(getline(cin, line)) {
        lines.push_back(line);
      }
      
      vector<Task> tasks2;
      
      //int c = 0;
      bool first_col = true;
      int col = -1;
      for(int c=0; c < (int)lines[0].size(); c++) {
        string cephalopod;
        char op = '_';
        for(int r=lines.size()-1; r>=0; r--) {
          if((size_t)r == lines.size()-1) {
            if(lines[r].at(c) == '*' || lines[r].at(c) == '+') {
              op = lines[r].at(c);
              first_col = true;
              col++;
            } else {
              first_col = false;
            }
          } else {
          if(r<(int)lines.size()-1) {
            if(lines[r].at(c) != ' ') {
              cephalopod.push_back(lines[r].at(c)); }
            }
          }
        }
        if(first_col) {
          col++;
          assert(op != '_');
          reverse(cephalopod.begin(), cephalopod.end());
          tasks2.push_back({op, vector<ll>(), vector<string>(),-1,0});
          tasks2.back().cephalopods.push_back(cephalopod);
          tasks2.back().numbers.push_back(stoll(cephalopod));
        } else {
          if(!cephalopod.empty()) {
            reverse(cephalopod.begin(), cephalopod.end());
            tasks2.back().cephalopods.push_back(cephalopod);
            tasks2.back().numbers.push_back(stoll(cephalopod));
          }
        }
      }
      for(auto &t:tasks2) {
        switch(t.op) {
          case '+':
            {
              t.result = reduce(t.numbers.begin(), t.numbers.end(), 0LL);
              res += t.result;
            }
            break;
          case '*':
            {
              t.result = reduce(t.numbers.begin(), t.numbers.end(), 1LL, multiplies<ll>());
              res += t.result;
            }
            break;
          default: {
                     cerr << "unknown: " << t.op << "." << endl;
                      exit(1);
                   }
        }
      }



      cout << "Part 2: " << res << endl;
    }
}
