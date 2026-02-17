#pragma once
#pragma GCC diagnostic push
#pragma GCC system_header
#pragma GCC diagnostic pop

#include <chrono>
#include <vector>
#include <cassert>
#include <vector>
#include <math.h>
#include <functional>
#include <utility>
#include <functional>
#include <ostream>
#include <optional>
#include <concepts>

namespace utils {
 template <typename F, typename... Args>
  auto measure(F&& f, Args&&... args) {
      auto start = std::chrono::high_resolution_clock::now();
      std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
      auto end = std::chrono::high_resolution_clock::now();
      return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  }


  template <class T>
  inline void hash_combine(T& seed, const T& v) {
      std::hash<T> hasher;
      // The magic constant 0x9e3779b9 helps distribute bits evenly
      seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  template<typename T>
  T hash_vec(const std::vector<T>& v) {
    T seed = 0;
    for(T e:v) {
      hash_combine(seed, e);
    }
    return seed;
  }

  template<typename T, typename S>
  T hash_pair(const std::pair<T,S>& p) {
    T seed = 0;
    hash_combine(seed, p.first);
    hash_combine(seed, p.second);
    return seed;
  }

  template<typename T, typename S>
  struct PairHash {
    T operator()(const std::pair<T,S>& p) const {
      return hash_pair(p);
    }
  };

  template<typename T>
  struct VecHash {
    T operator()(const std::vector<T>& v) const {
      return hash_vec(v);
    }
  };

  template<typename T>
  struct VecEqual {
    bool operator()(const std::vector<T>& lhs, const std::vector<T>& rhs) const {
      if(lhs.size() != rhs.size()) return false;
      for(std::size_t i=0; i < lhs.size(); i++) {
        if(lhs[i] != rhs[i]) return false;
      }
      return true;
    }
  };

  // 1. Concept to check if it's a standard vector
  template <typename T>
  concept IsVector = requires(T v) {
      typename T::value_type;
      v.begin();
      v.end();
  };

  // 2. Overload for "Flat" Vectors (Row Vectors)
  // We use a constraint to ensure the inner type is NOT another vector
  template <IsVector V>
  requires (!IsVector<typename V::value_type>)
  std::ostream& operator<<(std::ostream& os, const V& vec) {
      os << "[ ";
      for (const auto& el : vec) os << std::setw(2) <<  el << " ";
      os << "]";
      return os;
  }

  // 3. Overload for "Nested" Vectors (Matrices)
  template <IsVector V>
  requires IsVector<typename V::value_type>
  std::ostream& operator<<(std::ostream& os, const V& matrix) {
      os << "[\n";
      for (const auto& row : matrix) {
          os << "  " << row << "\n"; // This calls the flat vector overload!
      }
      os << "]";
      return os;
  }


  template<typename T>
  std::ostream &operator<<(std::ostream &oss, const std::optional<T>&r) {
    if(r.has_value()) {
      oss << *r;
    } else {
      oss << "optional is empty";
    }
    return oss;
  }
}
