//
// Created by rsh on 2025-12-05.
//

#include <iostream>
#include <queue>
#include <tuple>
#include <type_traits>
#include <vector>
#include "../include/aoc2025_04.hpp"

typedef long long ll;

using namespace std;
namespace aoc_2025_04 {
    template<typename T> using Matrix = vector<vector<T>>;
    Matrix<char> field;
    void read_field() {
      string line;
      int row = 0, col = 0;
      while(getline(cin, line)) {
        col = 0;
        field.push_back(vector<char>());
        for(char c:line) {
          field.at(row).push_back(c);
          col++;
        }
        row++;
      }
    }
    template<typename T>
    ostream & operator<< (ostream &oss, const Matrix<T> m) {
      for(const auto &row:m) {
        for(const auto &c:row) {
          oss << c;
        }
        oss << endl;
      }
      return oss;
    }
    template<typename T>
    bool in_bounds(const Matrix<T> m, int r, int c) {
       return 0 <= r && r < (int)m.size() && 0 <= c && c < (int)m[0].size();
    }
    void cnt_left(Matrix<ll> &adjacences, int r, int c) {
      if(field[r][c] == '@' && in_bounds(adjacences, r,c+1) && field.at(r).at(c+1) == '@') {
        adjacences[r][c]++;
        adjacences[r][c+1]++;
      }
    }
    void cnt_left_down(Matrix<ll> &adjacences, int r, int c) {
      if(field[r][c] == '@' && in_bounds(adjacences, r+1, c+1) && field.at(r+1).at(c+1) == '@') {
        adjacences[r][c]++;
        adjacences[r+1][c+1]++;
      }
    }
    void cnt_right_down(Matrix<ll> &adjacences, int r, int c) {
      if(field[r][c] == '@' && in_bounds(adjacences, r+1, c-1) && field.at(r+1).at(c-1) == '@') {
        adjacences[r][c]++;
        adjacences[r+1][c-1]++;
      }
    }
    void cnt_down(Matrix<ll> &adjacences, int r, int c) {
      if(field[r][c] == '@' && in_bounds(adjacences, r+1, c) && field.at(r+1).at(c) == '@') {
        adjacences[r][c]++;
        adjacences[r+1][c]++;
      }
    }
    Matrix<ll> get_adjacences() {
      Matrix<ll> adjacences(field.size(), vector<ll>(field.at(0).size(), 0));
      for(int r=0; r < (int)field.size(); r++) {
        for(int c=0; c < (int)field[r].size(); c++) {
          cnt_left(adjacences, r,c);
          cnt_left_down(adjacences,r,c);
          cnt_right_down(adjacences,r,c);
          cnt_down(adjacences,r,c);
        }
      }
      return adjacences;
    }
    extern "C" void part1() {
      read_field();
      auto adjacences = get_adjacences();
      ll res = 0;
      for(int r=0; r < (int)field.size(); r++) {
        for(int c=0; c < (int)field[r].size(); c++) {
          if(field[r][c] == '@' && adjacences[r][c] < 4) {
            res += 1;
          }
        }
      }
      cout << "Part 1: " << res << endl;
    }
    extern "C" void part2() {
      read_field();
      auto adjacences = get_adjacences();

      queue<pair<int,int>> can_remove;
      for(int r=0; r < (int)field.size(); r++) {
        for(int c=0; c < (int)field[r].size(); c++) {
          if(field[r][c] == '@' && adjacences[r][c] < 4) {
            can_remove.push({r,c});
          }
        }
      }
      ll removes = 0;
      while(!can_remove.empty()) {
        auto [r,c]  = can_remove.front(); can_remove.pop();
        for(int dr=-1; dr<=1; dr++) {
          for(int dc=-1; dc<=1; dc++) {
            if(dr==0 && dc==0) continue;
            int rr = r+dr;
            int cc = c + dc;
            if(in_bounds(field, rr,cc) && field[rr][cc] == '@' && adjacences[rr][cc] >= 4) {
              adjacences[rr][cc]--;
              if(adjacences[rr][cc] < 4) {
                can_remove.push({rr,cc});
              }
            }
          }
        }

        field[r][c] = '.';
        removes++;
      }
      cout << "Part 2: " << removes << endl;
    }
}
