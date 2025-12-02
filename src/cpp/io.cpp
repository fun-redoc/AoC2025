//
// Created by rsh on 10/6/25.
//
#include "../include/io.hpp"
#include <iostream>
#include <optional>

using namespace std;

namespace aoc {
# define BREAKPOINT __asm__("int $3")
    int int_from_args(int argc, char* argv[], const string &argn) {
        for (auto i=0; i<argc; i++) {
            if (__builtin_strcmp(argv[i], argn.c_str()) == 0) {
                if (i+1<argc) {
                    return atoi(argv[i+1]);
                } else {
                    cerr << "Argument for " << argn << "not found." << endl;
                }
            }
        }
        cerr << "Argument " << argn << "not found." << endl;
        exit(1);
    }
    void redir_std_io(int argc, char *argv[]) {
        optional<int> argc_in = nullopt;
        optional<int> argc_out = nullopt;
        for (int i = 0; i < argc; i++) {
            if (string(argv[i]) == "-in" && ((i + 1) < argc)) {
                argc_in = i;
            }
            if (string(argv[i]) == "-out" && ((i + 1) < argc)) {
                argc_out = i;
            }
        }
        if (argc_in.has_value()) {
            try {
                // redirect files to cin and cout
                freopen(argv[argc_in.value() + 1], "r", stdin);
                //freopen(argv[argc_out.value() + 1], "w", stdout);
            } catch (const std::runtime_error &e) {
                std::cerr << "redirecting stdin: "  << e.what() << std::endl;
            }
        }
        if (argc_out.has_value()) {
            //cerr << "-in and -out parameters for resp. files have to be provided both." << endl;
            //exit(1);
            try {
                // redirect files to cin and cout
                //freopen(argv[argc_in.value() + 1], "r", stdin);
                freopen(argv[argc_out.value() + 1], "w", stdout);
            } catch (const std::runtime_error &e) {
                std::cerr << "redirecing stdout: "  << e.what() << std::endl;
            }
        }
        // otherwise use cin and cout directly
    }
}
