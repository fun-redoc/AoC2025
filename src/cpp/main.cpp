#include <iostream>
#include <ctime>
#include <dlfcn.h>

#include "../include/io.hpp"

using namespace std;
using namespace aoc;

int main(int argc, char* argv[]) {
    FAST_STD_IO;
    redir_std_io(argc, argv);

    auto year = int_from_args(argc, argv, "-year");
    auto day = int_from_args(argc, argv, "-day");
    auto part = int_from_args(argc, argv, "-part");

    string fnName = format("part{}", part);
    string soName = format("aoc{}_{:0>2}.so",year, day);
    cout << soName << endl;

    // Load the library
    void* handle = dlopen(soName.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Could not open library: " << dlerror() << std::endl;
        std::cerr << "propably Environment LD_LIBRARY_PATH not set" << std::endl;
        return 1;
    }

    // Use dlsym to get the function pointer
    typedef void(*SolutionFunctionType)();  // Define a function pointer type
    SolutionFunctionType solution = (SolutionFunctionType)dlsym(handle, fnName.c_str()); // Avoid name mangling

    if (!solution) {
        std::cerr << "Could not load function: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }

    time_t now = time(0);
    tm *ltm = localtime(&now);

    // Call the function
    solution();

    // Close the library
    dlclose(handle);
    return 0;
}

