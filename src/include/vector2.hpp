//
// Created by rsh on 10/22/25.
//

#ifndef AOC_VECTOR2_H
#define AOC_VECTOR2_H
#include <utility>
#include <hash_set>
#include "mymath.hpp"

using namespace std;
namespace algebra {
    template<typename T> using Vec2d =  pair<T, T>;
    //struct Vec2dHash;
    //struct Vec2dEqual;
    struct Vec2dHash {
        template <class T>
        size_t operator() (const Vec2d<T>& p) const {
            auto h1 = hash<T>{}(p.first);
            auto h2 = hash<T>{}(p.second);
            return h1 ^ h2;
        }
    };

    struct Vec2dEqual {
        template <class T>
        bool operator() (const Vec2d<T>& lhs, const Vec2d<T>& rhs) const {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };
    //template<typename T> Vec2d<T> add(const Vec2d<T> &pos, const Vec2d<T> &dir);
    // for some reason template function don't link if implemented separatelly in cpp file
    template<typename T> Vec2d<T> add(const Vec2d<T> &pos, const Vec2d<T> &dir) {
        return {pos.first+dir.first, pos.second+dir.second};
    }
    template<typename T> Vec2d<T> sub(const Vec2d<T> &pos, const Vec2d<T> &dir) {
        return {pos.first-dir.first, pos.second-dir.second};
    }
    template<typename T> Vec2d<T> smul(const T c,const Vec2d<T> &v) {
        return {c*v.first, c*v.second};
    }
    template<typename T> T manhattan_dist(const Vec2d<T> &v1, const Vec2d<T> &v2) {
        return abs(v1.first-v2.first) + abs(v1.second-v2.second);
    }
    template<typename T> Vec2d<T> sgn(const Vec2d<T> &v) {
        return {mymath::sgn(v.first), mymath::sgn(v.second)};
    }

}

#endif //AOC_VECTOR2_H
