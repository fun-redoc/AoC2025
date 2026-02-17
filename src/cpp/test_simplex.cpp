//
// Created by rsh on 2026-01-10.
//

#include <cassert>
#include <chrono>
#include <ios>
#include <ostream>
#include <math.h>
#include <iostream>
#include "../include/utils.hpp"
#include "../include/simplex.hpp"


typedef long long ll;

using namespace std;
using namespace utils;
using namespace simplex;

void test_linear_program(const LinearProgram<double>& lp, const optional<LinearProgram<double>::Result>& expected) {
    auto t = measure([&lp, &expected]() {
      optional<LinearProgram<double>::Result> res = LinearProgram<double>::simplex(lp);
      cout << lp << endl;
      cout << "res: "  << res << endl;
      assert(res == expected);
    });
    cout << unitbuf;
      cout << " Time: " << t.count() << " µs " << t.count()/1E6 << " s "  << endl << flush;
      cout << nounitbuf;
}

#define R(v) (LinearProgram<double>::restricted((v)))
#define U(v) (LinearProgram<double>::Restricted::unrestricted((v)))

#pragma gcc diagnostic ignored "-Wunused-parameter"
int main(int argc, char* argv[]) {

  // wenshenpsu lect. 23 time 55:00 "Integer LP"
    test_linear_program({{{{ 1,-1}, LinearProgram<double>::Comp::LE,  2},
                          {{ 2, 4}, LinearProgram<double>::Comp::LE, 15},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(2),R(-3)}, 0}
                        },LinearProgram<double>::Result{35,{5,0,0,15}});



  // wenshenpsu lect. 10, timstp: 1:04:25, redundancy
    test_linear_program({{{{ 1, 2, 0, 1}, LinearProgram<double>::Comp::EQ, 20},
                          {{ 2, 1, 1, 0}, LinearProgram<double>::Comp::EQ, 10},
                          {{-1, 4,-2, 3}, LinearProgram<double>::Comp::EQ, 40},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(1),R(4),R(3),R(2)}, 0}
                        },LinearProgram<double>::Result{35,{5,0,0,15}});

  // wenshenpsu lect. 10, timstp: 53:18, redundancy
    test_linear_program({{{{ 1,-2, 3,       1}, LinearProgram<double>::Comp::EQ, 6},
                          {{-1, 1, 2, 2.0/3.0}, LinearProgram<double>::Comp::EQ, 4},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(2),R(-1),R(1),R(-1)}, 0}
                        },LinearProgram<double>::Result{-6,{0,0,0,6}});

    test_linear_program({{{{6,8}, LinearProgram<double>::Comp::GE, 100},
                          {{7,12}, LinearProgram<double>::Comp::GE, 120},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(12),R(20)}, 0}
                        },LinearProgram<double>::Result{205,{15,1.25}});



    test_linear_program({{{{1,1}, LinearProgram<double>::Comp::GE, 2},
                          {{1,0}, LinearProgram<double>::Comp::GE, 1},
                          {{1,1}, LinearProgram<double>::Comp::LE, 5}
                         },
                         LinearProgram<double>::SimplexKind::MAXIMIZE,
                         {{R(2),R(1)},0}
                        },LinearProgram<double>::Result{10,{5,0}});


    test_linear_program({{{{1,1}, LinearProgram<double>::Comp::GE, 10},

                          {{1,-1}, LinearProgram<double>::Comp::LE, 15},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(3),R(2)},0}
                        },LinearProgram<double>::Result{20,{0,10}});


    // a favourable problem wenshenpsu lect.8 ts: 2:06
    test_linear_program({{{{ 3, 2, 0}, LinearProgram<double>::Comp::LE, 60},
                          {{-1, 1, 4}, LinearProgram<double>::Comp::LE, 10},
                          {{ 2,-2, 5}, LinearProgram<double>::Comp::LE, 50}
                         },
                         LinearProgram<double>::SimplexKind::MAXIMIZE,
                         {{R(2),R(3),R(3)},0}
                        },LinearProgram<double>::Result{70,{8,18,0,}});

  // wenshenpsu lect.10 time 4:41, no feasible solution
    test_linear_program({{{{ 1, 2, 1}, LinearProgram<double>::Comp::LE, 1},
                          {{-1, 0, 2}, LinearProgram<double>::Comp::GE, 4},
                          {{ 1,-1, 2}, LinearProgram<double>::Comp::EQ, 4},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(1),R(1),R(1)},0}
                        },nullopt);


    // wenshenpsu lect.9 time 51:00, 
    test_linear_program({{{{1,-2,-3,-2}, LinearProgram<double>::Comp::EQ,  3},
                          {{1,-1, 2, 1}, LinearProgram<double>::Comp::EQ, 11},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(+2),R(-3),R(1),R(1)},0}
                        },LinearProgram<double>::Result{14,{19,8,0,0}});

    // a degenerate problem wenshenpsu lect.8 ts: 31:30 'while simplex last row result will become 0.
    test_linear_program({{{{ 8,-2, 1,-1}, LinearProgram<double>::Comp::LE, 50},
                          {{ 3, 5, 0, 2}, LinearProgram<double>::Comp::LE, 150},
                          {{ 1,-1, 2,-4}, LinearProgram<double>::Comp::LE, 100}
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(2),R(4),R(-4), R(7)},0}
                        },LinearProgram<double>::Result{-200,{0,0,50,0}});

    test_linear_program({{{{6,3}, LinearProgram<double>::Comp::LE, 96},
                          {{1,1}, LinearProgram<double>::Comp::LE, 18},
                          {{2,6}, LinearProgram<double>::Comp::LE, 72}
                        },
                        LinearProgram<double>::SimplexKind::MAXIMIZE,
                        {{R(80),R(70)},0}
                      },LinearProgram<double>::Result{1400,{14,4}});

    test_linear_program({{{{0,0,0,0,1,1}, LinearProgram<double>::Comp::EQ, 3},
                          {{0,1,0,0,0,1}, LinearProgram<double>::Comp::EQ, 5},
                          {{0,0,1,1,1,0}, LinearProgram<double>::Comp::EQ, 4},
                          {{1,1,0,1,0,0}, LinearProgram<double>::Comp::EQ, 7},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(1),R(1),R(1),R(1),R(1),R(1)},0}
                         // positivity constraint of course
                        },LinearProgram<double>::Result{10,{1, 5, 0, 1, 3, 0}});

    // failing tests
    test_linear_program({{{{1,1}, LinearProgram<double>::Comp::GE, 10},
                          {{1,-1}, LinearProgram<double>::Comp::LE, 15},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(3),R(2)},0}
                        },LinearProgram<double>::Result{20,{0,10}});

    test_linear_program({{{{2,1}, LinearProgram<double>::Comp::GE, 4},
                          {{1,7}, LinearProgram<double>::Comp::GE, 7},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(1),R(1)}, 0}
                        },LinearProgram<double>::Result{31.0/13,{21.0/13,10.0/13}});



    test_linear_program({{{{1,1}, LinearProgram<double>::Comp::LE, 20},
                          {{2,-1}, LinearProgram<double>::Comp::GE, 10}
                         },
                         LinearProgram<double>::SimplexKind::MAXIMIZE,
                         {{R(5),R(10)},0}
                        },LinearProgram<double>::Result{150,{10,10}});

    test_linear_program({{{{2,3}, LinearProgram<double>::Comp::LE, 12},
                         {{-1,3}, LinearProgram<double>::Comp::EQ, 3},
                         },
                         LinearProgram<double>::SimplexKind::MAXIMIZE,
                         {{R(1),R(2)},0}
                        },LinearProgram<double>::Result{7,{3,2}});

    // the following fail because of "degenerate"
    test_linear_program({{{{1,0,1,1,0}, LinearProgram<double>::Comp::EQ, 7},
                          {{0,0,0,1,1}, LinearProgram<double>::Comp::EQ, 5},
                          {{1,1,0,1,1}, LinearProgram<double>::Comp::EQ, 12},
                          {{1,1,0,0,1}, LinearProgram<double>::Comp::EQ, 7},
                          {{1,0,1,0,1}, LinearProgram<double>::Comp::EQ, 2},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(1),R(1),R(1),R(1),R(1)},0}
                        //},LinearProgram<double>::Result{12,{0,3,0,7,2}});
                        },LinearProgram<double>::Result{12,{2,5,0,5,0}});

    test_linear_program({{{{1,0,0,0,0,0,0,0,0,1,1},LinearProgram<double>::EQ,44},
                          {{1,0,0,0,0,0,0,1,0,0,1},LinearProgram<double>::EQ,47},
                          {{1,0,0,0,1,1,1,0,0,0,0},LinearProgram<double>::EQ,57},
                          {{0,0,1,1,0,0,0,0,1,0,1},LinearProgram<double>::EQ,42},
                          {{1,0,1,1,0,1,1,0,0,0,1},LinearProgram<double>::EQ,69},
                          {{0,0,1,0,1,0,1,0,1,1,0},LinearProgram<double>::EQ,46},
                          {{1,1,0,0,0,0,0,0,1,0,0},LinearProgram<double>::EQ,40},
                          {{1,0,1,1,0,1,1,0,0,1,1},LinearProgram<double>::EQ,78},
                          {{1,0,1,0,0,0,0,0,0,0,0},LinearProgram<double>::EQ,21},
                          {{1,0,0,0,0,1,0,0,1,1,1},LinearProgram<double>::EQ,69} 
                         },
                         LinearProgram<double>::MINIMIZE,
                         {{R(1),R(1),R(1),R(1),R(1),R(1),R(1),R(1),R(1),R(1),R(1)} ,0}
                        },
                        LinearProgram<double>::Result{126,{21,6,0,15,17,12,7,12,13,9,14}});
    // wenshenpsu lect.9 time 35:26, finding feasible basic solution and canonical form
    test_linear_program({{{{1,-1, 0}, LinearProgram<double>::Comp::EQ, 1},
                          {{2, 1,-1}, LinearProgram<double>::Comp::EQ, 3},
                         },
                         LinearProgram<double>::SimplexKind::MINIMIZE,
                         {{R(0),R(0),R(0)},0}
                        },LinearProgram<double>::Result{0,{1.0+1.0/3,1.0/3,0}});

      


    return 0;
}
