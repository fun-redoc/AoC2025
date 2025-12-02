//
// Created by rsh on 10/22/25.
//

#include "../include/vector2.hpp"

using namespace std;
namespace algebra {

    // MOVED TO hpp, because of templates
    //struct Vec2dHash {
    //    template <class T>
    //    std::size_t operator() (const Vec2d<T>& p) const {
    //        auto h1 = std::hash<T>{}(p.first);
    //        auto h2 = std::hash<T>{}(p.second);
    //        return h1 ^ h2;
    //    }
    //};

    //struct Vec2dEqual {
    //    template <class T>
    //    bool operator() (const Vec2d<T>& lhs, const Vec2d<T>& rhs) const {
    //        return lhs.first == rhs.first && lhs.second == rhs.second;
    //    }
    //};


}

