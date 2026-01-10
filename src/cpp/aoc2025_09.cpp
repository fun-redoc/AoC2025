//
// Created by rsh on 2025-12-29.
//

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <cassert>
#include <vector>
#include <regex>
#include <math.h>
#include <optional>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include "../include/vector2.hpp"
#include "../include/aoc2025_09.hpp"

typedef long long ll;
typedef algebra::Vec2d<ll> V2;

using namespace std;
using namespace algebra;

namespace aoc_2025_09 {
    typedef pair<V2, V2> Vertex;
    template <class T>
    inline void hash_combine(ll& seed, const T& v) {
        std::hash<T> hasher;
        // The magic constant 0x9e3779b9 helps distribute bits evenly
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <typename T>
    struct NestedPairHash {
        ll operator()(const std::pair<std::pair<T, T>, std::pair<T, T>>& p) const {
            ll seed = 0;
            hash_combine(seed, p.first.first);
            hash_combine(seed, p.first.second);
            hash_combine(seed, p.second.first);
            hash_combine(seed, p.second.second);

            return seed;
        }
    };
    typedef struct {
      ll area;
      ll from_col, to_col, from_row, to_row;
      Vec2d<ll> coord1, coord2;
    } Rect;
    //vector<tuple<ll, ll, ll,ll,ll>> rects;
    struct RectHash {
        ll operator()(const Rect& p) const {
            ll seed = 0;
            hash_combine(seed, p.from_col);
            hash_combine(seed, p.to_col);
            hash_combine(seed, p.from_row);
            hash_combine(seed, p.to_col);
            return seed;
        }
    };
    struct RectEqual {
       bool operator()(const Rect& lhs, const Rect& rhs) const {
            return 
              lhs.from_col == rhs.from_col &&
              lhs.to_col == rhs.to_col &&
              lhs.from_row == rhs.from_row &&
              lhs.to_col == rhs.to_col;
        }
    };
    bool operator==(const Rect& lhs, const Rect& rhs) {
      RectEqual eq;
      return eq(lhs, rhs);
    }
    
    void read_red_tiles(vector<pair<ll,ll>> &t) {
      t.clear();
      string line;
      regex rx(R"((\d+),(\d+))");
      smatch match;
      while(getline(cin, line)) {
        if(regex_match(line, match, rx)) {
            ll x = stoll(match[1].str());
            ll y = stoll(match[2].str());
            t.push_back({x,y});
        } else {
          cerr << "Invalid line format: " << line << endl;
        }
      }

    }
    ostream & operator<<(ostream &oss, const Vec2d<ll> &v) {
      oss << "(" << v.first << "," << v.second << ")";
      return oss;
    }
    extern "C" void part1() {
      vector<Vec2d<ll>> red_tiles;
      read_red_tiles(red_tiles);

      //vector<pair<size_t,size_t, ll>> rects;
      ll max_area = 0;
      for(int i=0; i<(int)red_tiles.size(); i++) {
        for(int j=i+1; j<(int)red_tiles.size(); j++) {
          pair<ll, ll> coord1 = red_tiles[i];
          pair<ll, ll> coord2 = red_tiles[j];
          ll horz = abs(coord1.second - coord2.second) + 1; // +1 because number of tiles, eg. x1=1, x=1 => len 0 but 1 Tile
          ll vert = abs(coord1.first - coord2.first) + 1;
          ll area = horz*vert;
          //edges.push_back({i,j, area});
          max_area = max(max_area, area);
        }
      }
      // too low: 2147444600
      cout << "Part 1: " << max_area << endl;
    }

  pair<V2,V2> get_dims(const vector<Vec2d<ll>> &red_tiles) {
      pair<V2, V2> dims({{LLONG_MAX, LLONG_MAX},{0,0}});;
      for(int i=0; i<(int)red_tiles.size(); i++) {
          dims.second.first  = max(dims.second.first, red_tiles[i].first);
          dims.second.second = max(dims.second.second, red_tiles[i].second);
          dims.first.first   = min(dims.first.first, red_tiles[i].first);
          dims.first.second  = min(dims.first.second, red_tiles[i].second);
      }
      return {{dims.first.first, dims.first.second},{dims.second.first+1, dims.second.second+1}};
    }

    void print_field(vector<string> &out, const vector<V2> &red_tiles) {
      for(size_t i=0; i<red_tiles.size(); i++) {
        out[red_tiles[i].second].replace(red_tiles[i].first, 1, "#");
      }
    }
    void print_field(const vector<V2> &red_tiles) {
      const auto& [coord_min, coord_max] = get_dims(red_tiles);
      const auto& [cmin, rmin] = coord_min;
      const auto& [cmax, rmax] = coord_max;
      int cols = cmax - cmin;
      int rows = rmax - rmin;
      vector<string> out(rmin+rows+1, string(cmin + cols+1, '.'));
      print_field(out, red_tiles);
      for(int r=rmin; r<rmax; r++) {
        cout << out[r] << endl;
      }
    }
    void print_field(vector<string> &out, 
                     const unordered_set<pair<Vec2d<ll>, Vec2d<ll>>, NestedPairHash<ll>> &vts) {
      for(const auto &v:vts) {
        auto r1 = v.first.second;
        auto c1 = v.first.first;
        auto r2 = v.second.second;
        auto c2 = v.second.first;
        if(out[r1][c1] == '.') out[r1].replace(c1, 1, "O");
        if(out[r2][c2] == '.') out[r2].replace(c2, 1, "O");
      }
    }
    void print_field(const vector<V2> &red_tiles,
                     const unordered_set<pair<Vec2d<ll>, Vec2d<ll>>, NestedPairHash<ll>> &vts) {
      const auto& [coord_min, coord_max] = get_dims(red_tiles);
      const auto& [cmin, rmin] = coord_min;
      const auto& [cmax, rmax] = coord_max;
      int cols = cmax - cmin;
      int rows = rmax - rmin;
      vector<string> out(rmin+rows+1, string(cmin + cols+1, '.'));
      print_field(out, red_tiles);
      print_field(out, vts);
    
      for(int r=rmin; r<rmax; r++) {
        cout << out[r] << endl;
      }
    
    }
    template<typename Container>
    void print_field(const vector<Vec2d<ll>> &red_tiles,
                     const unordered_set<pair<Vec2d<ll>, Vec2d<ll>>, NestedPairHash<ll>> &vts,
                     const Container &within) {
      using ElementType = typename Container::value_type;
      static_assert(is_same_v<ElementType,Vec2d<ll>>, "Container must hold Vec2d<ll>.");
      const auto& [coord_min, coord_max] = get_dims(red_tiles);
      const auto& [cmin, rmin] = coord_min;
      const auto& [cmax, rmax] = coord_max;
      int cols = cmax - cmin;
      int rows = rmax - rmin;
      vector<string> out(rmin+rows+1, string(cmin + cols+1, '.'));
      print_field(out, red_tiles);
      print_field(out, vts);
      for(const auto &v:within) {
        auto r = v.second;
        auto c = v.first;
        out[r].replace(c, 1, "o");
      }
    
      for(int r=rmin; r<rmax; r++) {
        cout << out[r] << endl;
      }
    
    }
    tuple<V2,V2,V2, V2> vectorize_rect(const V2 &c1,
                                       const V2 &c2) {
      V2 tl; //  top left
      V2 tr; //  top right
      V2 bl; //  bottom left
      V2 br; //  bottom right
      if(c1.first <= c2.first && c1.second <= c2.second) {
        /*
         * 1...
         * ....
         * ...2
         */
        tl = c1;
        tr = {c2.first, c1.second};
        bl = {c1.first, c2.second};
        br = c2;
        return {tl, tr, bl, br};
      } 
      if(c1.first <= c2.first && c1.second >= c2.second) {
        /*
         * ...2
         * ....
         * 1...
         */
        tl = {c1.first, c2.second};
        tr = c2;
        bl = c1;
        br = {c2.first, c1.second};
        return {tl, tr, bl, br};
      }
      if(c1.first >= c2.first && c1.second <= c2.second) {
        /*
         * ...1
         * ....
         * 2...
         */
        tl = {c2.first, c1.second};
        tr = c1;
        bl = c2;
        br = {c1.first, c2.second};
        return {tl, tr, bl, br};
      }
      if(c1.first >= c2.first && c1.second >= c2.second) {
        /*
         * 2...
         * ....
         * ...1
         */
        tl = c2;
        tr = {c1.first, c2.second};
        bl = {c2.first, c1.second};
        br = c1;
        return {tl, tr, bl, br};
      }
#pragma GCC diagnostic ignored "-Wreturn-type"
    }


    /**
     * @brief finds all neighbouring red tiles, clockwize
     * @param red_tiles all given red tiles
     * @param t tile for which the neighbours are sought
     * @return tuple of all neighbours in the sequence left, right, up, down. Not available neightbour is nullopt.
     */
    template<typename RedTilesContainer>
    tuple<optional<V2>, optional<V2>, optional<V2>, optional<V2>>
      //  left          right         up            down
    find_neighbours(const RedTilesContainer &red_tiles, const V2 &t) {
      using ElementType = typename RedTilesContainer::value_type;
      static_assert(is_same_v<ElementType,Vec2d<ll>>, "Container must hold Vec2d<ll>.");
      ll dist_left  = LLONG_MAX;
      ll dist_right = LLONG_MAX;
      ll dist_up    = LLONG_MAX;
      ll dist_down  = LLONG_MAX;
      tuple<optional<V2>, optional<V2>, optional<V2>, optional<V2>> nbs = {nullopt, nullopt, nullopt, nullopt};
      for(const auto &rt:red_tiles) {
        if(t == rt) continue;
        if(rt.second == t.second && rt.first < t.first) { // left
          auto new_dist = abs(rt.first - t.first);
          if(new_dist < dist_left) {
            get<0>(nbs) = rt;
            dist_left = new_dist;
          }
        }
        if(rt.second == t.second && rt.first > t.first) { // right
          auto new_dist = abs(rt.first - t.first);
          if(new_dist < dist_right) {
            get<1>(nbs) = rt;
            dist_right = new_dist;
          }
        }
        if(rt.first == t.first && rt.second < t.second) { // up
          auto new_dist = abs(rt.second - t.second);
          if(new_dist < dist_up) {
            get<2>(nbs) = rt;
            dist_up = new_dist;
          }
        }
        if(rt.first == t.first && rt.second > t.second) { // down
          auto new_dist = abs(rt.second - t.second);
          if(new_dist < dist_down) {
            get<3>(nbs) = rt;
            dist_down = new_dist;
          }
        }
      }
      return nbs;
    }

    // 1. The Iteration Helper
    // This function takes a tuple and a generic function (lambda)
    // and applies the function to every element.
    template <typename Tuple, typename Func>
    void for_each_in_tuple(Tuple&& t, Func&& f) {
        std::apply(
            [&f](auto&&... args) {
                // Fold expression: calls f(args) for every argument in the pack
                (f(std::forward<decltype(args)>(args)), ...);
            },
            std::forward<Tuple>(t)
        );
    }


    template<typename RedTilesContainer>
    void vectorize_field(unordered_set<pair<Vec2d<ll>, Vec2d<ll>>, NestedPairHash<ll>> &vertices, const RedTilesContainer &red_tiles) {
      using ElementType = typename RedTilesContainer::value_type;
      static_assert(is_same_v<ElementType,Vec2d<ll>>, "Container must hold Vec2d<ll>.");
      auto it = red_tiles.begin();
      Vec2d<ll> first_tile = *it;
      Vec2d<ll> last_tile = *it;
      ++it;
      while(it != red_tiles.end()) {
        Vec2d<ll> tile = *it;
        auto vertex = (last_tile < tile) ? make_pair(last_tile,tile) : make_pair(tile,last_tile);
        //if(vertices.count(vertex) == 0) {
          vertices.insert(vertex);
        //}
        last_tile = *it;
        ++it;
      }
      auto vertex = (last_tile < first_tile) ? make_pair(last_tile,first_tile) : make_pair(first_tile,last_tile);
      //if(vertices.count(vertex) == 0) {
        vertices.insert(vertex);
      //}
    }
    bool check_inside(const vector<V2> &red_tiles,
                      const V2 &c1,
                      const V2 &c2) {

      vector<tuple<V2,V2>> vts; // vectorized tiles

      return false;
      
    }

    /**
      @brief  makes the vector canonical ((x1,y2),(x2,y2)) so that if x1==x2 follows y1<=y2 otherwise x1 <= x2
     * */
    pair<Vec2d<ll>, Vec2d<ll>> canonize(const pair<Vec2d<ll>,Vec2d<ll>> &r) {
      auto vertex = (r.first < r.second) ? make_pair(r.first,r.second) : make_pair(r.second,r.first);
      return vertex;
    }
    bool check_canonical(const pair<Vec2d<ll>,Vec2d<ll>> &r) {
      return  (r.first < r.second);
    }

    optional<pair<Vec2d<ll>, Vec2d<ll>>> is_on_a_vertex(const unordered_set<pair<Vec2d<ll>, Vec2d<ll>>, NestedPairHash<ll>> &vertices,
                                                const Vec2d<ll> &point) {
      // TODO better Data structure to search for a point can help accelerate
      for(const auto &v:vertices) {
        assert(check_canonical(v));
        if(v.first.first==point.first && point.first == v.second.first
            && v.first.second <= point.second && point.second <= v.second.second) {
          // on vert vertex
          return v;
        }
        if(v.first.first<=point.first && point.first <= v.second.first
            && v.first.second == point.second && point.second == v.second.second) {
          // on horz vertex
          return v;
        }
      }
      return nullopt;
    }

    bool is_vertical(const pair<Vec2d<ll>, Vec2d<ll>> &vertex) {
      return vertex.first.first == vertex.second.first; // colums are equal
    }
    bool is_horizontal(const pair<Vec2d<ll>, Vec2d<ll>> &vertex) {
      return vertex.first.second == vertex.second.second; // rows are equal
    }


    template<typename BorderContainer>
    bool is_in(const BorderContainer &border, const Vec2d<ll> &point) {
      using ElementType = typename BorderContainer::value_type;
      static_assert(is_same_v<ElementType,Vec2d<ll>>, "Container must hold Vec2d<ll>.");

      static unordered_map<Vec2d<ll>, bool, Vec2dHash, Vec2dEqual> memoize;
      auto m = memoize .find(point);
      if(m != memoize.end()) {
        return m->second;
      }

      // check up // check down // check left // check right
      auto neighbours = find_neighbours(border, point);
      bool is_within =  get<0>(neighbours) && get<1>(neighbours) && get<2>(neighbours) && get<3>(neighbours);
      memoize[point] = is_within;
      return is_within;
    }

    extern "C" void part2() {
      unordered_set<pair<Vec2d<ll>, Vec2d<ll>>, NestedPairHash<ll>> vertices;
      vector<V2> red_tiles;
      //vector<vector<char>> field;
      //if(false) {
      //  read_field("field1.txt",field);
      //  field_to_red_tiles(field, red_tiles);
      //}

      optional<Rect> max_area_rect = nullopt;
      
      //if(red_tiles.empty()) {
        read_red_tiles(red_tiles);
      //}
      vectorize_field(vertices, red_tiles);
      
      //print_field(red_tiles, vertices);

      //// points on border
      //unordered_set<Vec2d<ll>, Vec2dHash> border;
      vector<Vec2d<ll>> border;
      for(const auto &v:vertices) {
        auto from = v.first;
        auto to   = v.second;
        if(is_vertical(v)) {
          for(ll r=from.second; r <= to.second; r++) {
            //border.insert({from.first, r});
            border.push_back({from.first, r});
          }
        } else if(is_horizontal(v)) {
          for(ll c=from.first; c <= to.first; c++) {
            //border.insert({c, from.second});
            border.push_back({c, from.second});
          }
        }
      }
      
      //print_field(red_tiles, vertices, border);

      unordered_map<ll,vector<ll>> cols_rows;
      for(const auto& b:border) {
        if(cols_rows[b.first].empty()) {
          cols_rows[b.first] = vector<ll>();
        }
        auto& col = cols_rows[b.first];
        col.push_back(b.second);
      }
      for(auto& c:cols_rows) {
        sort(c.second.begin(), c.second.end());
        // Remove consecutive duplicates
        auto it = std::unique(c.second.begin(), c.second.end());
        // Erase redundant elements
        c.second.erase(it, c.second.end());
      }

      cout << "borders ready" << endl;

      const auto& [coord_min, coord_max] = get_dims(red_tiles);
      const auto& [cmin, rmin] = coord_min;
      const auto& [cmax, rmax] = coord_max;
      ll cols = cmax - cmin;
      ll rows = rmax - rmin;

      cout << "dims: [" << coord_min << "," <<coord_max << "]" << "size: " << cols*rows << endl;

      vector<Rect> rects;
      //unordered_set<Rect, RectHash, RectEqual> rects;
      for(int i=0; i<(int)red_tiles.size(); i++) {
        //for(int j=i+1; j<(int)red_tiles.size(); j++) {
        for(int j=0; j<(int)red_tiles.size(); j++) {
          Vec2d<ll> coord1 = red_tiles[i];
          Vec2d<ll> coord2 = red_tiles[j];

          auto from_col = min(coord1.first, coord2.first);
          auto to_col   = max(coord1.first, coord2.first);
          auto from_row = min(coord1.second, coord2.second);
          auto to_row   = max(coord1.second, coord2.second);

          ll a = 0, b = 0;
          if(to_col>from_col) {
            a = to_col-from_col;
          } else {
            a = from_col-to_col;
          }
          if(to_row>from_row) {
             b = to_row - from_row;
          } else {
             b = from_row - to_row;
          }
          ll area = (a+1)*(b+1);
          rects.push_back({area, from_col, to_col, from_row, to_row, coord1, coord2});
          //rects.insert({area, from_col, to_col, from_row, to_row, coord1, coord2});
        }
      }
      cout << "rects " << "(cnt: " << rects.size() << ") " << "not sorted" << endl;
      
      sort(rects.begin(), rects.end(), [](const auto& r1, const auto& r2) { return r1.area > r2.area;});
      
      //// Remove consecutive duplicates
      auto it = std::unique(rects.begin(), rects.end());
      // Erase redundant elements
      rects.erase(it, rects.end());
      
      cout << "rects " << "(cnt: " << rects.size() << ") " << "sorted by size" << endl;

      // exclude not fitting rects, until found the first fitting -> sorted by area -> the biggest one
      typedef struct {
        ll col;
        ll row;
        size_t idx; // idx in colums row array
      } Rect_Embedding;
      size_t rect_idx = 0;
      for(const auto& r:rects) {
        max_area_rect = r;
        auto& [area, c1, c2, r1, r2,_,_] = r;
        bool slice_covers_rect = true;
        for(ll c=c1; slice_covers_rect &&  c<=c2; c++) {
          auto rows = cols_rows[c];
          optional<Rect_Embedding> upper = nullopt;
          optional<Rect_Embedding> lower = nullopt;
          for(size_t i=0; i<rows.size(); i++ ) {
            if(rows[i] <= r1 && (i < rows.size()-1 && rows[i+1]>=r1)) {
              upper = {c, rows[i],i};
              continue;
            } 
            if(!upper && rows[i] > r2) {
              //  BAD CASE
              slice_covers_rect = false;
              break;
            }
            if(upper) {
              if(rows[i] >= r2) {
                // GOOD CASE: slice covers whole rect height
                lower = {c, rows[i], i};
                slice_covers_rect = true;
                break;
              }
              if(rows[i] < r2) {
                //if(rows[i-1] !=  rows[i]-1) {
                //  //  BAD CASE there is gap
                //  slice_covers_rect = false;
                //  break;
                //}
                if(i==rows.size()-1) {
                  // BAD CASE: after that nothing comes, falling to bottom
                  slice_covers_rect = false;
                  break;
                }
                if(rows[i+1] > rows[i]+1) {
                  //  BAD CASE: there is a gap
                  slice_covers_rect = false;
                  break;
                }
              }
            } 
          }
          if(!(upper && lower && lower->row >= r1 && upper->row <= r2)) {
            slice_covers_rect = false;
            break;
          }
        }
        if(slice_covers_rect) {
          max_area_rect = r;
          break;
        }
        rect_idx++;
      }
      

    if(max_area_rect) {
      cout  << "Part 2: " 
          << "idx: " << rect_idx 
          << " Max Area: " << max_area_rect->area 
          << " has (" << max_area_rect->coord1 << ","  << max_area_rect->coord2 << ")"   
          << endl;
    } else {
      cout  << "Part 2: " 
          << "no solution found."
          << endl;
    }
  }
}
