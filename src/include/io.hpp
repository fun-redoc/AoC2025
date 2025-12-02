//
// Created by rsh on 10/6/25.
//
#pragma once
#include <string>

namespace aoc {
#define FAST_STD_IO ios::sync_with_stdio(0);cin.tie(0);cout.tie(0)
    int int_from_args(int argc, char* argv[], const std::string &argn);
    /**
    * helper function, redirects an input and an output file to cin resp. cout
    */
    void redir_std_io(int argc, char* argv[]);
}
