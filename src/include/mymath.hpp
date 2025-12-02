//
// Created by rsh on 10/24/25.
//

#ifndef AOC_MATH_H
#define AOC_MATH_H

#include <type_traits>

// c€(a,b]
#define in_lo_rc_interval(a,b,c) (((a)<(c))&&((c)<=(b)))
// c€[a,b)
#define in_lc_ro_interval(a,b,c) (((a)<=(c))&&((c)<(b)))

namespace mymath {
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
}

#endif //AOC_MATH_H
