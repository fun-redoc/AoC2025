//
// Created by rsh on 2025-12-03.
//

#include <iostream>
#include <vector>
#include <cmath>
#include "../include/aoc2025_03.hpp"

typedef long long ll;

using namespace std;
namespace aoc_2025_03 {
    typedef vector<ll> Bank;
    typedef vector<Bank> Pack;
    Pack pack;
    void read_puzzle() {
      pack.clear();
      string line;
      while(getline(cin,line)) {
        Bank bank;
        for(char c:line) {
          bank.push_back(c - '0');
        }
        pack.push_back(bank);
      }
    }
    ll solve_for(int batteries) {
      ll res = 0;
      int BANK_BATTERIS_MAX_CNT = batteries;
      for(const auto &bank:pack) {
        ll bank_max = 0;
        int pos = 0;
        int max_idx = 0;
        while(pos < BANK_BATTERIS_MAX_CNT) {
          int limit = (int)bank.size() - BANK_BATTERIS_MAX_CNT + pos;
          for(int i=max_idx; i <= limit; i++) {
            if(bank[i] > bank[max_idx]) {
              max_idx = i;
            } 
          }
          if(max_idx <= limit) { 
            bank_max = bank_max + bank[max_idx]*pow(10LL,BANK_BATTERIS_MAX_CNT - pos - 1);
            max_idx++;
            pos++;
          } else {// means no value found in loop above
            for(int i=limit; i< (int)bank.size(); i++) {
              bank_max = bank_max + bank[i]*pow(10LL,BANK_BATTERIS_MAX_CNT - pos - 1);
              pos++;
            }
          }
        }
        res += bank_max;
      }
      return res;
    }
    extern "C" void part1() {
      read_puzzle();
      cout << "Part 1: " << solve_for(2) << endl;
    }
    extern "C" void part2() {
      // 234234234234278
      //     ^  
      //   4  
      read_puzzle();
      cout << "Part 2: " << solve_for(12) << endl;
    }
}
