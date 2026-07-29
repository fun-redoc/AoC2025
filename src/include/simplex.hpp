#pragma once
#include <algorithm>
#include <cmath>
#pragma GCC diagnostic push
#pragma GCC system_header
#pragma GCC diagnostic pop
#include <cassert>
#include <iomanip>
#include <vector>
#include <vector>
#include <limits>
#include <optional>
#include <cassert>
#include <limits>
#include <vector>
#include <cstdio>
#include <ostream>
#include <vector>
#include <math.h>
#include <iostream>
#include <unordered_set>
#include <numeric>
#include <memory>
#include <stack>
#include <queue>
#include <bitset>
#include "../include/utils.hpp"
#include "../include/mymath.hpp"

using namespace std;

namespace simplex {
  using utils::operator<<;
/**
 * Implementation of a full Simplex Tableau in C++.
 * Conversion from general Problems (Minimizing, $\ge$, $\eq$) are handeled
 * conversion into "Standard Form" is handeled by Two Phase Simplex
 * Integer Problems are not handeled.
 * Vanishing Floats are handled by not normalizing to 1 within the pivots
 *
 * "Standard Form" The optimization problem is presented as a matrix (the Tableau).
 *      Decision Variables: $x_1 \dots x_n$
 *      Slack Variables: $s_1 \dots s_n$
 *      Artificial Variables: $a_1 \dots a_4$ - one for each row to form an initial identity matrix
 *      Objective: Minimize $Z = \sum x_i$
 *
 * BEWARE: This implementation does not always yield correkt results. In some mixed problems it simply fails.
 *
 * TODO: implement unbounded target variables e.d. via optionals in target vector
 *
 * Usage:
 * 
      int main() {
        LinearProgram<double> lp3{ // standard maximizization problem
          {
            {{1,1}, LinearProgram<double>::Comp::GE, 10},
            {{1,-1}, LinearProgram<double>::Comp::LE, 15},
          },
          LinearProgram<double>::SimplexKind::MINIMIZE,
          {3,2}
          // positivity constraint of course
        };
        optional<typename LinearProgram<double>::Result> res3 = LinearProgram<double>::simplex(lp3);
        cout << "res3: " << res3 << endl;
        assert(res3 == (typename LinearProgram<double>::Result{20,{0,10}}));

        return 0;
      }
 *
 */

  template<typename T> struct LinearProgram {
    static_assert(is_floating_point<T>::value, "generic type has to be arithmetic");
    public:
    typedef enum {MINIMIZE=0, MAXIMIZE, INTEGER, COUNT} SimplexKind;
    typedef enum {LE,GE,EQ} Comp;
    typedef struct {
      T opt_val;
      vector<T> vars;
    } Result;
    typedef struct {
      vector<T> lhs;
      Comp rel;
      T rhs;
    } Constraint;
    typedef struct {
      // indices of the unrestricted vars (aka x_i for which the >= rule does not apply)
      // they are handled by introducin of x_1' and x_i'' which x_i = x_1' - x_i''
      enum {RESTRICTED, UNRESTRICTED} restriction;
      T val;
    } Restricted;
    typedef struct {
      vector<LinearProgram::Restricted> coeffs;
      T const_term;
    } Target; // c1*x1 + ... + cn*xn + const_term;

    vector<LinearProgram::Constraint> constraints;

    //SimplexKind kind;

    static constexpr size_t to_index(LinearProgram::SimplexKind k) {
        return static_cast<size_t>(k);
    }

    bitset<LinearProgram::to_index(SimplexKind::COUNT)> kinds;
    LinearProgram::Target target;


    static constexpr LinearProgram::Restricted unrestricted(T v) {
      return {LinearProgram::Restricted::UNRESTRICTED, v};
    }

    static constexpr LinearProgram::Restricted restricted(T v) {
      return {LinearProgram::Restricted::RESTRICTED, v};
    }

    friend ostream &operator<<(ostream &oss, const typename LinearProgram<T>::Comp &c) {
      switch(c) {
        case LE: oss << "<="; break;
        case GE: oss << ">="; break;
        case EQ: oss << "=="; break;
      }
      return oss;
    }

    friend ostream &operator<<(ostream &oss, const typename LinearProgram<T>::SimplexKind &k) {
      switch(k) {
        case MINIMIZE: oss << "Minimize"; break;
        case MAXIMIZE: oss << "Maximize"; break;
      }
      return oss;
    }

    friend ostream &operator<<(ostream &oss, const typename LinearProgram<T>::Restricted &r) {
      if(r.restriction == LinearProgram::Restricted::RESTRICTED) {
        oss << r.val;
      } else {
        oss << "unrestricted " << r.val;
      }
      return oss;
    }

    friend ostream &operator<<(ostream &oss, const typename LinearProgram<T>::Target &t) {
      oss << "{" << t.coeffs << t.const_term << "}";
      return oss;
    }

    friend ostream &operator<<(ostream &oss, const LinearProgram<T> &lp)
    {
      // freind lets the compiler omit the !hidden! this parameter, f*ck OO
      oss << "Linear Program: " << lp.kinds << std::setw(3) << lp.target << endl;
      for(size_t r=0; r<lp.constraints.size(); r++) {
        oss << lp.constraints[r].lhs << " " << lp.constraints[r].rel << " " << lp.constraints[r].rhs << endl;
      }
      return oss;
    }

    friend ostream &operator<<(ostream &oss, const typename LinearProgram<T>::Result &r)
    {
      // freind lets the compiler omit the !hidden! this parameter, f*ck OO
      oss << "[" << r.opt_val << "," << r.vars << "]";
      return oss;
    }

    friend bool operator==(const typename LinearProgram::Result &lhs, const typename LinearProgram::Result &rhs) {
      //return lhs.opt_val == rhs.opt_val && lhs.vars == rhs.vars;
      auto zt = std::views::zip_transform([](const auto &l, const auto &r) {
            return EQ_ZERO(fabs(l-r));
          }, lhs.vars, rhs.vars);
      bool cmp_res = reduce(zt.begin(), zt.end(), true, [](bool acc, bool z) {
            return acc && z;
          });
      return EQ_ZERO(fabs(lhs.opt_val - rhs.opt_val)) && cmp_res;
    }

    static void print(ostream &oss, const LinearProgram<T>::Result &r) {
      oss << "[" << r.opt_val << "," << r.vars << "]";
    }


    static optional<LinearProgram::Result> simplex(const LinearProgram &lp) {
      // 0. checks
#ifdef DEBUG
      assert(GT_ZERO(lp.constraints.size()));
      assert(lp.target.coeffs.size() == lp.constraints[0].lhs.size());
#endif
      shared_ptr<Tableau> t = make_shared<Tableau>(create_tableau(lp));

      //auto canonical = make_canonical_constraints(t);

      auto raw_res = simplex_standard(t);
      assert(check_feasible(t));

      if(!raw_res) return nullopt;
      T opt_val = lp.target.const_term;
      for(size_t v=0; v<t->num_vars; v++) {
        opt_val += (*raw_res)[v]*lp.target.coeffs[v].val;
      }
      return make_optional(LinearProgram::Result{opt_val, *raw_res});
    }

    static optional<LinearProgram::Result> dual_simplex(const LinearProgram &lp) {
      // 0. checks
#ifdef DEBUG
      assert(GT_ZERO(lp.constraints.size()));
      assert(lp.target.coeffs.size() == lp.constraints[0].lhs.size());
#endif
      shared_ptr<Tableau> t = make_shared<Tableau>(create_dual_tableau(lp));

      auto raw_res = dual_simplex_standard(t);
      assert(check_feasible(t));

      if(!raw_res) return nullopt;
      T opt_val = lp.target.const_term;
      for(size_t v=0; v<t->num_vars; v++) {
        opt_val += (*raw_res)[v]*lp.target.coeffs[v].val;
      }
      return make_optional(LinearProgram::Result{opt_val, *raw_res});
    }

    static constexpr bool GT_ZERO(T a) {return a>eps;}
    static constexpr bool LT_ZERO(T a) {return a<-eps;}
    static constexpr bool EQ_ZERO(T a) {return -eps<=a && a<=eps;}
//    static constexpr bool EQ_ZERO(T a) {return -1e-9<=a && a<=1e-9;}

    private:


    static constexpr T eps = 1e-9; // 100*numeric_limits<T>::epsilon(); // TODO how to calculate with floats preciselly

    struct Tableau { 
    /* Tableau Form
         vars  slack  artif.
        |x y | u v w | a b | rhs
       ------------------------
       u|    | 1     |     |
       v|    |   1   |     |
       w|    |     1 |     |
       a|    |       | 1   |
       b|    |       |   1 |
       ------------------------
       W|    |       |     |
       Z|    |       |     |
    */
      size_t                   num_vars       ;//= lp.target.size();
      size_t                   num_slack      ;//= lp.constraints.size();
      size_t                   num_art        ;//= all constraint no LE
      size_t                   idx_slack_col  ;
      size_t                   idx_art_col    ;
      size_t                   idx_rhs_col    ;//= num_vars + num_slack + num_art;
      size_t                   num_constraints;// 
      size_t                   idx_Z_row      ;// phase 2 -> resolve to the min solution
      size_t                   idx_W_row      ;// phase 1 -> resolve to feasible basic form
      size_t                   total_cols     ;// 
      size_t                   total_rows     ;//
      vector<vector<T>>        t              ;//
      vector<optional<size_t>> basic_vars     ; // var index and CB

      Tableau(size_t num_vars, size_t num_constraints, size_t num_slack, size_t num_art) {
        this->num_vars        = num_vars;
        this->num_slack       = num_slack;
        this->num_art         = num_art;
        this->idx_slack_col   = num_vars;
        this->idx_art_col     = this->idx_slack_col + num_slack;
        this->idx_rhs_col     = this->num_vars + this->num_slack + this->num_art;
        this->num_constraints = num_constraints;
        this->idx_Z_row       = num_constraints;
        this->idx_W_row       = this->idx_Z_row+(num_art>0?1:0); // no artificial vars, W row is the Z row
        this->total_cols      = this->num_vars + this->num_slack + this->num_art + 1; 
        this->total_rows      = this->idx_W_row + 1; 
        this->t               = vector(this->total_rows, vector<T>(this->total_cols, static_cast<T>(0)));
        this->basic_vars      = vector<optional<size_t>>(num_constraints, nullopt);
        for(size_t r=0; r<this->num_slack; r++) {
          basic_vars[r] = this->num_vars+r;
        }
      }
      Tableau(size_t num_vars, size_t num_slack, size_t num_art, size_t idx_slack_col, size_t idx_art_col, size_t idx_rhs_col, size_t num_constraints, size_t idx_Z_row, size_t idx_W_row, size_t total_cols, size_t total_rows, const vector<vector<T>>& t, const vector<optional<size_t>> &basic_vars) {
        this->num_vars        = num_vars;    
        this->num_slack       = num_slack;   
        this->num_art         = num_art;   
        this->idx_slack_col   = idx_rhs_col; 
        this->idx_art_col     = idx_rhs_col; 
        this->idx_rhs_col     = idx_rhs_col; 
        this->num_constraints = num_constraints;
        this->idx_Z_row       = idx_Z_row;   
        this->idx_W_row       = idx_W_row;   
        this->total_cols      = total_cols;  
        this->total_rows      = total_rows;  
        this->t               = t;
        this->basic_vars      = basic_vars;
      }
      Tableau(const Tableau &orig) {
        this->num_vars        = orig.num_vars;    
        this->num_slack       = orig.num_slack;   
        this->num_art         = orig.num_art;   
        this->idx_slack_col   = orig.idx_slack_col; 
        this->idx_art_col     = orig.idx_art_col; 
        this->idx_rhs_col     = orig.idx_rhs_col; 
        this->num_constraints = orig.num_constraints;
        this->idx_Z_row       = orig.idx_Z_row;   
        this->idx_W_row       = orig.idx_W_row;   
        this->total_cols      = orig.total_cols;  
        this->total_rows      = orig.total_rows;  
        this->t               = orig.t;
        this->basic_vars      = orig.basic_vars;
      }
      Tableau() {
        this->num_vars        = 0;
        this->num_slack       = 0;
        this->num_art         = 0;
        this->idx_slack_col   = 0;
        this->idx_art_col     = 0;
        this->idx_rhs_col     = 0;
        this->num_constraints = 0;
        this->idx_Z_row       = 0;
        this->idx_W_row       = 0;
        this->total_cols      = 0;
        this->total_rows      = 0;
        //this->t;
      }
      ~Tableau() {
        this->num_vars        = 0;
        this->num_slack       = 0;
        this->num_art         = 0;
        this->idx_slack_col   = 0;
        this->idx_art_col     = 0;
        this->idx_rhs_col     = 0;
        this->num_constraints = 0;
        this->idx_Z_row       = 0;
        this->idx_W_row       = 0;
        this->total_cols      = 0;
        this->total_rows      = 0;
        for(auto &r:this->t) r.clear();
        this->t.clear();
        this->basic_vars.clear();
      }
    };

    /**
     * returns pivot rows:  indicates if pivot point is degenerate (ratio == 0)
     *                      gives additional informaton in PivotRow Struct to
     *                      decide what to to next
     */
    typedef struct {
      bool is_degenerate;
      size_t row;
      optional<size_t> idx_var;
      T ratio;
    } PivotRow;
    static vector<PivotRow> find_pivot_rows(shared_ptr<Tableau> t, size_t pc) {
      T min_ratio = std::numeric_limits<T>::max();
      vector<PivotRow> ratios;
      for(size_t r=0; r<t->num_constraints; r++) {
        auto var_i = t->basic_vars[r];
        //if(!EQ_ZERO(t->t[r][pc])) {
        if(GT_ZERO(t->t[r][pc])) {
          auto ratio = t->t[r][t->idx_rhs_col]/t->t[r][pc];
          if(GT_ZERO(ratio) || EQ_ZERO(ratio)) { // beware, eq zero makes degenrate tableau
            assert(var_i);
            ratios.push_back(PivotRow{EQ_ZERO(t->t[r][t->idx_rhs_col]), r, var_i, t->t[r][t->idx_rhs_col]/t->t[r][pc]});
          }
        } 
      }
      sort(ratios.begin(), ratios.end(), [](const auto &lhs, const auto &rhs) {
            return LT_ZERO(lhs.ratio - rhs.ratio);
          });
      return ratios;
    }
    /**
     * returns pair first:  indicates if pivot point is degenerate (ratio == 0)
     *              second: is the pivot row, if existing, optional nothing otherwise
     */
    static optional<PivotRow> find_pivot_row_1(shared_ptr<Tableau> t, size_t pc) {
      auto pivot_rows = find_pivot_rows(t, pc);
      if(pivot_rows.empty()) return nullopt;
      auto pivot_row = pivot_rows.front();
      return make_optional(pivot_row);
    }
    static optional<pair<bool, size_t>> find_pivot_row(shared_ptr<Tableau> t, size_t pc) {
      optional<size_t> pr = nullopt;
      T min_ratio = std::numeric_limits<T>::max();
      vector<tuple<size_t, optional<size_t>, double>> ratios;
      for(size_t r=0; r<t->num_constraints; r++) {
        auto var_i = t->basic_vars[r];
        //if(!EQ_ZERO(t->t[r][pc])) {
        if(GT_ZERO(t->t[r][pc])) {
          auto ratio = t->t[r][t->idx_rhs_col]/t->t[r][pc];
          if(GT_ZERO(ratio) || EQ_ZERO(ratio)) { // beware, eq zero makes degenrate tableau
            assert(var_i);
            ratios.push_back({r, var_i, t->t[r][t->idx_rhs_col]/t->t[r][pc]});
          }
        } 
      }
      if(ratios.size() == 0) return nullopt;
      sort(ratios.begin(), ratios.end(), [](const auto &lhs, const auto &rhs) {
            //return get<2>(lhs) < get<2>(rhs);
            return LT_ZERO(get<2>(lhs) - get<2>(rhs));
          });
      return make_optional(make_pair(EQ_ZERO(get<2>(ratios.front())), get<0>(ratios.front())));
    }

    static optional<vector<size_t>> find_pivot_col(shared_ptr<Tableau> t, size_t ref_row_idx) {
      T min_val = 0;
      optional<size_t> pc = nullopt;
      //for(size_t c=0; c<t->idx_rhs_col; c++) { // optimization: no pivoting over artificials
      for(size_t c=0; c<t->idx_art_col; c++) {
        // was:if(LT_ZERO(t->t[ref_row_idx][c] - min_val)) {
        if(LT_ZERO(t->t[ref_row_idx][c]) && LT_ZERO(t->t[ref_row_idx][c] - min_val)) {
          pc = c;
          min_val = t->t[ref_row_idx][c];
        }
      }
      if(!pc) return nullopt;
      vector<size_t> res;
      res.push_back(*pc);
      // find furher äquivalent results to try
      //for(size_t c=0; c<t->idx_rhs_col; c++) { // optimization: no pivoting over artificials
      for(size_t c=0; c<t->idx_art_col; c++) {
        if(*pc != c && EQ_ZERO(t->t[ref_row_idx][c] - min_val)) {
          res.push_back(c);
        }
      }
      return res;
    }

    static void  pivot(shared_ptr<Tableau> t, const size_t pr, const size_t pc) {
      t->basic_vars[pr] = pc; 
      // mutates the tableau
      // 1. work pivot row, pivot elemnt (pr,pc) yields 1, adjust the rest
      auto pivot_val = t->t[pr][pc];
      //for(size_t c=0; c<t->total_cols; c++) {
      //  t->t[pr][c] /= pivot_val;
      //}
      for(size_t c=0; c<t->total_cols; c++) { // optimization: artificail columns need not to be considered
        if(c<t->idx_art_col || c<=t->idx_rhs_col) {
          t->t[pr][c] /= pivot_val;
        }
      }
      
      // 2. all other rows: pivot colum yields 0, adjust rest
      for(size_t r=0; r<t->total_rows; r++) {
        if(r != pr) { // pivot row already ready in step 1.
          auto factor = t->t[r][pc];
          for(size_t c=0; c<t->total_cols; c++) {
            if(c<t->idx_art_col || c<=t->idx_rhs_col) { //  optimization: artificail columns need not to be considered
              t->t[r][c] = t->t[r][c] - (factor*t->t[pr][c]); 
            }
          }
        }
      }
    }

    static optional<vector<T>> simplex_standard(shared_ptr<Tableau> t) {
      // mutates the tableau
      unordered_set<pair<size_t, size_t>, utils::PairHash<size_t, size_t>> finished_pivots;
      bool has_solution = false;
      bool has_no_solution = false;
      bool degenerate = false;
      bool phase1 = t->num_art > 0;
      bool phase2 = false;
      while (!has_solution && !has_no_solution) {
          optional<vector<size_t>> pcs;
          phase1 = (t->num_art > 0) && !EQ_ZERO(t->t[t->idx_W_row][t->idx_rhs_col]);
          // TODO even if W is 0, there can still remain artificial variables 
          //      in the basic solution, this can be the case, if the set of constraints is redundant
          //      means one constraint is a linear combination of some other constraints
          if(!phase1 && !phase2) {
            queue<size_t> redundancy_rows;
            vector<bool> basic_non_art(t->num_constraints, false);
            for(size_t r=0;r<t->num_constraints; r++) {
              auto bv = t->basic_vars[r];
              if(t->idx_art_col <= bv && bv <t->idx_rhs_col) {
                redundancy_rows.push(r);
              }
              if(bv && 0<=*bv && *bv<t->num_vars) {
                basic_non_art[r] = true;
              }
            }
            //assert(redundancy_rows.empty()); // TODO handle redundancies
            if(!redundancy_rows.empty()) {
              // replace the artificials from redundancy by on of the vars which are not already basic.
              assert((t->num_art > 0) && EQ_ZERO(t->t[t->idx_W_row][t->idx_rhs_col]));
              size_t num_redundancy_finished = redundancy_rows.size();
              while(!redundancy_rows.empty()) {
                size_t r = redundancy_rows.front(); redundancy_rows.pop();
                for(size_t ibv=0; ibv<t->num_vars; ibv++) {
                  if(!basic_non_art[r] && !EQ_ZERO(t->t[r][ibv])) {
                    pivot(t, r, ibv);
                    basic_non_art[r] = true;
                    num_redundancy_finished--;
                    break;
                  }
                }
              }
              //assert((basic_non_art | views::filter([](auto e) {return !e;}) | ranges::to<vector>()).size() == 0);
              assert(EQ_ZERO(t->t[t->idx_W_row][t->idx_rhs_col])); // should not change
              if(num_redundancy_finished != 0) { 
                // one row is redundant iff all basic vars are 0 and rhs is also 0
                for(size_t r=0; r<basic_non_art.size(); r++) {
                  bool is_redundant = true;
                  if(!basic_non_art[r]){
                    is_redundant &= EQ_ZERO(t->t[r][t->idx_rhs_col]);
                    for(size_t c=0; c<t->num_vars; c++) {
                      is_redundant &= EQ_ZERO(t->t[r][c]);
                    }
                    if(!is_redundant) {
                      has_no_solution = true;
                      break;
                    }
                  }
                }
              } 
            }
            phase2 = true;
          }

          if(has_no_solution) break;

          if(phase1) {
            //assert(!EQ_ZERO(t->t[t->idx_W_row][t->idx_rhs_col]));
            // in phase1
            pcs = find_pivot_col(t, t->idx_W_row); // phase 1 first
            if(!pcs) { 
                has_no_solution = false;
                break;
            }
          }
          if(!phase1 && phase2) {
            // in phase2
            pcs = find_pivot_col(t, t->idx_Z_row);
            if (!pcs) {
              has_solution = true;
              break;
            } 
          }
          //assert(!degenerate); // in case of prvious tableau was degenerate the solution should be ready
                               // because the degenerate case doen not further minimizes the objective function
                               // if not, something should be wrong here....lets see in tests:

          //optional<PivotRow> pr = nullopt;
          optional<pair<bool, size_t>> pr = nullopt;
          for(const auto& pc:*pcs) {
            //auto prs = find_pivot_rows(t, pc);
            //pr = find_pivot_row_1(t, pc).transform([](const auto& pr){ return make_pair(pr.is_degenerate, pr.row);});
            pr = find_pivot_row(t, pc);
            if (!pr) {
              continue;  // try next peer
            }
            //assert(!finished_pivots.contains({pr->second,pc})); // check for loops
            //if(finished_pivots.contains({pr->second,pc})) asm("int3"); // check for loops
            has_no_solution = false;
            degenerate = pr->first;
            pivot(t, pr->second, pc);
            finished_pivots.insert({pr->second, pc});
            break; 
          }
          if(!pr) has_no_solution = true;  // no appropriate pivot row found
      }

      //cout << "Tableau prior to colleting solution:" << endl << t->t << endl;

      if(has_solution) {
        vector<T> solution_vars(t->num_vars, 0);
        for(size_t r=0; r<t->num_constraints; r++) {
          size_t row_var = *(t->basic_vars[r]);
          if(row_var < t->num_vars) {
            solution_vars[row_var] = t->t[r][t->idx_rhs_col]/t->t[r][*(t->basic_vars[r])];
          }
        }
        return solution_vars;
      }

      return nullopt;
    }

    static optional<vector<T>> dual_simplex_standard(shared_ptr<Tableau> t) {
      // mutates the tableau
      unordered_set<pair<size_t, size_t>, utils::PairHash<size_t, size_t>> finished_pivots;
      bool has_solution = false;

      while(!has_solution) {
        // 1. find a row with negative rhs, I'm searching for the smallest rhs
        optional<size_t> pivot_row = nullopt;
        for(size_t r=0; r<t->idx_Z_row; r++) {
          if(t->t[r][t->idx_rhs_col] < 0) {
            if(!pivot_row || t->t[r][t->idx_rhs_col] < t->t[*pivot_row][t->idx_rhs_col]) {
              pivot_row = r;
            }
          }
        }

        if(pivot_row) {
          // 2. for the pivot row search for a columns with negative coefs
          //    and among them choose the on with the largest (least negative!) coef/z_i ratio
          optional<T> ratio;
          optional<size_t> pivot_col = nullopt;
          for(size_t c=0; c<t->idx_rhs_col; c++) {
            if(t->t[*pivot_row][c] < 0) {
              T new_ratio = t->t[t->idx_Z_row][c]/t->t[*pivot_row][c];
              if(!ratio || new_ratio > ratio) {
                ratio = new_ratio;
                pivot_col = c;
              }
            }
          }
          if(!pivot_col) return nullopt; // no dual solution so far

          pivot(t, *pivot_row, *pivot_col);
        } else {
          has_solution = true;
        }
      }

      vector<T> solution_vars(t->num_vars, 0);
      for(size_t r=0; r<t->num_constraints; r++) {
        size_t row_var = *(t->basic_vars[r]);
        if(row_var < t->num_vars) {
          solution_vars[row_var] = t->t[r][t->idx_rhs_col]/t->t[r][*(t->basic_vars[r])];
        }
      }
      return solution_vars;

    }

    /**
     * having EQ and GE relations two pases are needed
     * in the first phase (phase1) with help of artificial variables 
     * the canonical form of the linear program is reached (havind a feasible basic solution)
     * the tableau for the phase1 have its on artificial target function, so for phase2
     * the tableau has to be adjusted with the given target function
     */
    static Tableau create_tableau(const LinearProgram& lp) {
      // 0. count constraints, determin artificial vars
      size_t num_art = 0;
      size_t num_slack = 0;
      for(const auto& c:lp.constraints) {
        switch(c.rel) {
          case LinearProgram::Comp::LE: num_slack++;            break;
          case LinearProgram::Comp::GE: num_slack++; num_art++; break;
          case LinearProgram::Comp::EQ:              num_art++; break;
          default:
            cerr << "unkown case" << endl;
            exit(1);
        }
      }

      size_t num_targets = lp.target.coeffs.size() + // include additional target var for unbouded vars
                           2*count_if(lp.target.coeffs.begin(), lp.target.coeffs.end(),
                                   [](const auto &c){return c.restriction == LinearProgram::Restricted::UNRESTRICTED;}); 


      // 1. build the tableau
      size_t num_vars = num_targets;
      Tableau t(num_vars, lp.constraints.size(), num_slack, num_art);
      // 1.1. rows and vars and rhs
      size_t cnt_slack_vars = 0;
      size_t cnt_artif_vars = 0;
      for(size_t r=0; r<lp.constraints.size(); r++) {
        t.t[r][t.idx_rhs_col] = lp.constraints[r].rhs;
        size_t var_num = 0;
        for(size_t c=0; c<lp.target.coeffs.size(); c++) {
          t.t[r][var_num]   = lp.constraints[r].lhs[c];
          var_num++;
          if(lp.target.coeffs[c].restriction == LinearProgram::Restricted::UNRESTRICTED) {
            t.t[r][var_num]   = -lp.constraints[r].lhs[c];
            var_num++;
          } 
        }
        switch(lp.constraints[r].rel) {
          case LinearProgram::Comp::EQ:
            // EQ add artificial vars (needed to create a feasible basic form in phase 1)
            t.t[r][t.idx_art_col+cnt_artif_vars] =  static_cast<T>(1);
            t.basic_vars[r] = t.idx_art_col+cnt_artif_vars;
            cnt_artif_vars++;
            break;
          case LinearProgram::Comp::LE:
            // LE only positive slack var
            t.t[r][t.idx_slack_col+cnt_slack_vars] =  static_cast<T>(1);
            t.basic_vars[r] = t.idx_slack_col+cnt_slack_vars;
            cnt_slack_vars++;
            break;
          case LinearProgram::Comp::GE:
            // GE negative slack var and artificial var
            t.t[r][t.idx_slack_col+cnt_slack_vars] =  static_cast<T>(-1);
            cnt_slack_vars++;
            t.t[r][t.idx_art_col+cnt_artif_vars]   =  static_cast<T>(1);
            t.basic_vars[r] = t.idx_art_col+cnt_artif_vars;
            cnt_artif_vars++;
            break;
        }
      }

      T kind_factor = lp.kinds.test(to_index(LinearProgram::SimplexKind::MINIMIZE))
                               ? static_cast<T>( 1)
                               : static_cast<T>(-1);   // in case of Maximizing simply flip the sign of the target function

      // 1.2 W row for phase 1
      for(size_t c=0; c<t.idx_art_col; c++) {
        t.t[t.idx_W_row][c] = static_cast<T>(0);
        for(size_t r=0; r<t.num_constraints; r++) {
          if(t.idx_art_col<=t.basic_vars[r] && t.basic_vars[r]<t.idx_rhs_col) {
            t.t[t.idx_W_row][c] += t.t[r][c];
          }
        }
        t.t[t.idx_W_row][c] *= static_cast<T>(-1);
      }

      t.t[t.idx_W_row][t.idx_rhs_col] = static_cast<T>(0);
      for(size_t r=0; r<t.num_constraints; r++) {
        if(t.idx_art_col<=t.basic_vars[r] && t.basic_vars[r]<t.idx_rhs_col) {
          t.t[t.idx_W_row][t.idx_rhs_col] += t.t[r][t.idx_rhs_col];
        }
      }
      t.t[t.idx_W_row][t.idx_rhs_col] *= static_cast<T>(-1);

      for(size_t c=t.idx_art_col; c<t.idx_rhs_col; c++) {
        t.t[t.idx_W_row][c] = static_cast<T>(0);
      }

      // 1.3 Z row for phase 2
      t.t[t.idx_Z_row][t.idx_rhs_col] = lp.target.const_term;
      size_t var_num = 0;
      for(size_t c=0; c<lp.target.coeffs.size(); c++) {
        t.t[t.idx_Z_row][var_num] = +kind_factor*lp.target.coeffs[c].val;
        var_num++;
        if(lp.target.coeffs[c].restriction == LinearProgram::Restricted::UNRESTRICTED) {
          t.t[t.idx_Z_row][var_num] = -kind_factor*lp.target.coeffs[c].val;
          var_num++;
        }
      }

      return t;
    }

    static Tableau create_dual_tableau(const LinearProgram& lp) {
      // 0. count constraints, determin artificial vars
      size_t num_art = 0;
      size_t num_slack = 0;
      for(const auto& c:lp.constraints) {
        switch(c.rel) {
          case LinearProgram::Comp::LE: num_slack++;            break;
          //case LinearProgram::Comp::GE: num_slack++; num_art++; break;
          case LinearProgram::Comp::GE: num_slack++;            break;
          case LinearProgram::Comp::EQ:              num_art++; break;
          default:
            cerr << "unkown case" << endl;
            exit(1);
        }
      }

      size_t num_targets = lp.target.coeffs.size() + // include additional target var for unbouded vars
                           2*count_if(lp.target.coeffs.begin(), lp.target.coeffs.end(),
                                   [](const auto &c){return c.restriction == LinearProgram::Restricted::UNRESTRICTED;}); 


      // 1. build the tableau
      size_t num_vars = num_targets;
      Tableau t(num_vars, lp.constraints.size(), num_slack, num_art);
      // 1.1. rows and vars and rhs
      size_t cnt_slack_vars = 0;
      size_t cnt_artif_vars = 0;
      for(size_t r=0; r<lp.constraints.size(); r++) {
        t.t[r][t.idx_rhs_col] = lp.constraints[r].rhs;
        size_t var_num = 0;
        for(size_t c=0; c<lp.target.coeffs.size(); c++) {
          t.t[r][var_num]   = lp.constraints[r].lhs[c];
          var_num++;
          if(lp.target.coeffs[c].restriction == LinearProgram::Restricted::UNRESTRICTED) {
            t.t[r][var_num]   = -lp.constraints[r].lhs[c];
            var_num++;
          } 
        }
        switch(lp.constraints[r].rel) {
          case LinearProgram::Comp::EQ:
            // EQ add artificial vars (needed to create a feasible basic form in phase 1)
            t.t[r][t.idx_art_col+cnt_artif_vars] =  static_cast<T>(1);
            t.basic_vars[r] = t.idx_art_col+cnt_artif_vars;
            cnt_artif_vars++;
            break;
          case LinearProgram::Comp::LE:
            // LE only positive slack var
            t.t[r][t.idx_slack_col+cnt_slack_vars] =  static_cast<T>(1);
            t.basic_vars[r] = t.idx_slack_col+cnt_slack_vars;
            cnt_slack_vars++;
            break;
          case LinearProgram::Comp::GE:
            // GE in dual case no artificial vars for GE case
            t.t[r][t.idx_slack_col+cnt_slack_vars] =  static_cast<T>(-1);
            cnt_slack_vars++;
            // Flip signs
            for(size_t c=0; c<t.total_cols; c++) {
              t.t[r][c] *= static_cast<T>(-1);
            }
            break;
        }
      }

      T kind_factor = lp.kinds.test(to_index(LinearProgram::SimplexKind::MINIMIZE))
                               ? static_cast<T>( 1)
                               : static_cast<T>(-1);   // in case of Maximizing simply flip the sign of the target function

      // 1.2 W row for phase 1
      for(size_t c=0; c<t.idx_art_col; c++) {
        t.t[t.idx_W_row][c] = static_cast<T>(0);
        for(size_t r=0; r<t.num_constraints; r++) {
          if(t.idx_art_col<=t.basic_vars[r] && t.basic_vars[r]<t.idx_rhs_col) {
            t.t[t.idx_W_row][c] += t.t[r][c];
          }
        }
        t.t[t.idx_W_row][c] *= static_cast<T>(-1);
      }

      t.t[t.idx_W_row][t.idx_rhs_col] = static_cast<T>(0);
      for(size_t r=0; r<t.num_constraints; r++) {
        if(t.idx_art_col<=t.basic_vars[r] && t.basic_vars[r]<t.idx_rhs_col) {
          t.t[t.idx_W_row][t.idx_rhs_col] += t.t[r][t.idx_rhs_col];
        }
      }
      t.t[t.idx_W_row][t.idx_rhs_col] *= static_cast<T>(-1);

      for(size_t c=t.idx_art_col; c<t.idx_rhs_col; c++) {
        t.t[t.idx_W_row][c] = static_cast<T>(0);
      }

      // 1.3 Z row for phase 2
      t.t[t.idx_Z_row][t.idx_rhs_col] = lp.target.const_term;
      size_t var_num = 0;
      for(size_t c=0; c<lp.target.coeffs.size(); c++) {
        t.t[t.idx_Z_row][var_num] = +kind_factor*lp.target.coeffs[c].val;
        var_num++;
        if(lp.target.coeffs[c].restriction == LinearProgram::Restricted::UNRESTRICTED) {
          t.t[t.idx_Z_row][var_num] = -kind_factor*lp.target.coeffs[c].val;
          var_num++;
        }
      }

      return t;
    }

    static bool check_feasible(shared_ptr<Tableau> t) {
      // TODO implement me

      // 1. check if basic solution
      // 1.1 n variables, m constraints, n>=m
      if(!(t->idx_rhs_col >= t->idx_Z_row)) return false;
      for(const auto& bv:t->basic_vars) {
        if(!bv) return false;
      }
      // 1.2 n-m (basic) variables are non zero
      //     check basic vars
      //     check if bfs (basic feasible solution)
      //           all basic variable have non negative values
      for(size_t vr=0; vr<t->idx_Z_row; vr++) {
        auto bv = t->basic_vars[vr];
        if(!bv) return false;
        // basic var has excactly one 1 and 0 otherwise in the column
        size_t cnt_ones = 0;
        size_t cnt_zeroes = 0;
        for(size_t r=0; r<t->total_rows; r++) {
          if(EQ_ZERO(t->t[r][*bv])) cnt_zeroes++;
          if(EQ_ZERO(fabs(t->t[r][*bv]) - 1)) cnt_ones++;
        }
        if(!(cnt_ones == 1&& cnt_zeroes == t->total_rows -1)) return false;
        if(!EQ_ZERO(t->t[t->idx_Z_row][*bv])) return false;
        if(!(GT_ZERO(t->t[vr][t->idx_rhs_col]) || EQ_ZERO(t->t[vr][t->idx_rhs_col]))) return false;
      }

      return true;
    }
    //static unique_ptr<Tableau> make_canonical_constraints(const unique_ptr<Tableau>& t) {
    static shared_ptr<Tableau> make_canonical_constraints(const shared_ptr<Tableau> t) {
      assert(t->idx_rhs_col >= t->idx_Z_row);

      // maybe already a feasible solution
      //if(check_feasible(t)) return t;

      // find how many basic vars are missing
      size_t free_basic_vars = count(t->basic_vars.begin(), t->basic_vars.end(), nullopt);
      vector<size_t> additional_basic_vars(free_basic_vars);
      iota(additional_basic_vars.begin(), additional_basic_vars.end(), 0);
      shared_ptr<Tableau> initial_tableau;
      shared_ptr<Tableau> work_tableau;
      shared_ptr<Tableau> canonical;
      bool feasible = true;
      do {
        initial_tableau =  make_shared<Tableau>(Tableau(*t));
        for(size_t r=0; r<initial_tableau->basic_vars.size(); r++) {
          auto c= t->basic_vars[r];
          if(c) {
            pivot(initial_tableau, r, *c);
          }
        }
        if(additional_basic_vars.size() > 0) {
          feasible = true;

          stack<tuple<size_t, size_t, size_t, shared_ptr<Tableau>>> s;
          size_t cur  = 0;
          size_t i    = 0;
          size_t r    = 0;
          size_t nr   = 0;
          size_t c = additional_basic_vars[i];
          bool ready = false;
          work_tableau = make_shared<Tableau>(Tableau(*initial_tableau));
          //s.push({i, r, nr, make_shared<Tableau>(Tableau(*work_tableau))});
          do {
            if(r<work_tableau->basic_vars.size() &&
               !work_tableau->basic_vars[r] &&
               !EQ_ZERO(work_tableau->t[r][c]) &&
               !work_tableau->basic_vars[r]) {
              // row is free an fits criteria
              work_tableau->basic_vars[r] = c;
              pivot(work_tableau, r, c);

              // push intermediate result
              s.push({i, r, nr, make_shared<Tableau>(Tableau(*work_tableau))});

              // take next column
              if(i < additional_basic_vars.size()-1) {
                i++;
                r    = 0;
                nr   = 0;
                c = additional_basic_vars[i];
              } else {
                // last column worked on
                feasible = check_feasible(work_tableau);
                if(feasible) {
                  canonical = make_shared<Tableau>(Tableau(*work_tableau));
                  ready = true;
                } else {
                  ready = false;
                  // not feasible, backtrack
                  if(work_tableau->basic_vars[r] &&
                     *work_tableau->basic_vars[r] == c) {
                    work_tableau->basic_vars[r] = nullopt;
                  }
                  s.pop();
                  if(r+1 < work_tableau->basic_vars.size()) {
                     s.push({i, r+1, nr, make_shared<Tableau>(Tableau(*work_tableau))});
                  } else {
                    auto cur     = s.top();
                    i            = get<0>(cur);
                    r            = get<1>(cur);
                    nr           = get<2>(cur);
                    c            = additional_basic_vars[i];
                    work_tableau = make_shared<Tableau>(Tableau(*get<3>(cur))); // each step its own improved copy
                    assert(!(r<work_tableau->basic_vars.size()) || work_tableau->basic_vars[r] == nullopt);
                    r++;
                  }
                }
              }
            } else {
              // row already taken or not fitting, try next
              r++;
              if(!(r<work_tableau->basic_vars.size())) {
                // last row tested, backtrace
                if(s.empty()) {
                  feasible = false;
                  ready = true; // all checked, no solution?
                } else {
                  auto cur     = s.top();
                  i            = get<0>(cur);
                  r            = get<1>(cur);
                  nr           = get<2>(cur);
                  c            = additional_basic_vars[i];
                  s.pop(); 
                  if(!s.empty()) {
                    work_tableau = make_shared<Tableau>(Tableau(*get<3>(s.top()))); // get unpivoted tableau from previous step
                  } else {
                    // get unpivoted tableau from initial
                    work_tableau = make_shared<Tableau>(Tableau(*initial_tableau));
                  }
                  assert(!(r<work_tableau->basic_vars.size()) || work_tableau->basic_vars[r] == nullopt);
                  r++;
                }
              }
            }
          } while(!ready);
          while(!s.empty()) {
            s.pop();
          }
        } else {
          if(check_feasible(initial_tableau)) {
            canonical = make_shared<Tableau>(Tableau(*initial_tableau));
          }
        }

        if(!feasible &&
          !mymath::choose_next(initial_tableau->num_vars, additional_basic_vars.size(), additional_basic_vars)) {
          break;
        }
      } while(!feasible);

      if(feasible) return canonical;
      else return nullptr;
    }
  };
}
