//
// Created by rsh on 10/24/25.
//

#ifndef AOC_MATH_H
#define AOC_MATH_H

#include <type_traits>
#include <vector>
#include <cassert>

// c€(a,b]
#define in_lo_rc_interval(a,b,c) (((a)<(c))&&((c)<=(b)))
// c€[a,b)
#define in_lc_ro_interval(a,b,c) (((a)<=(c))&&((c)<(b)))

namespace mymath {
    bool choose_next(size_t m, size_t n, std::vector<size_t>& prev);

    template<typename T>
    concept Signed =std::is_signed_v<T>;
    template<Signed T>
    constexpr T sgn(const T &c) {
        if (c>0) return 1;
        if (c<0) return -1;
        return 0;
    }
    
    /**
     * mathematical modulo operation
     **/
    template<typename I>
    concept Integral = std::is_integral_v<I>;
    template <Integral I>
    constexpr  I mod(I a, I b) {
          return (a % b + b) % b;
      }

    /**
     * transpose a vector
     */
    template<typename T>
      constexpr void transpose(const std::vector<std::vector<T>> &orig, std::vector<std::vector<T>> &transposed) {
        size_t rows = orig.size();
        size_t cols = orig[0].size();
        size_t trows = transposed.size();
        size_t tcols = transposed[0].size();
        assert (rows == tcols && cols == trows);
        for(int r=0; r<rows; r++) {
          for(int c=0; c<cols; c++) {
            transposed[c][r] = orig[r][c];
          }
        }
      }
}

#endif //AOC_MATH_H
