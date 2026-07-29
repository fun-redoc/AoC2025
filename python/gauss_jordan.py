import math

type Matrix = list[list[int]]
type Vector = list[int]
type RRef = Matrix
type PivotVars = Vector
type FreeVars = Vector

def gauss_jordan_partial_pivot(A:Matrix, b:Vector|None=None) -> RRef:
    """
    Gauss–Jordan elimination with partial pivoting.
    If b is provided, solves Ax = b.
    Otherwise returns the RREF of A.

    A: list of lists (matrix)
    b: list (vector) or None
    """
    # Build augmented matrix if solving Ax=b
    if b is not None:
        A = [row + [rhs] for row, rhs in zip(A, b)]

    m = len(A)
    n = len(A[0])

    row = 0
    for col in range(n):
        # Find pivot row: largest absolute value in column 'col'
        pivot = max(range(row, m), key=lambda r: abs(A[r][col]))
        if abs(A[pivot][col]) < 1e-12:  # No usable pivot
            continue

        # Swap pivot row into position
        A[row], A[pivot] = A[pivot], A[row]

        # Normalize pivot row
        pivot_val = A[row][col]
        A[row] = [v / pivot_val for v in A[row]]

        # Eliminate all other rows
        for r in range(m):
            if r != row and abs(A[r][col]) > 1e-12:
                factor = A[r][col]
                A[r] = [A[r][c] - factor * A[row][c] for c in range(n)]

        row += 1
        if row == m:
            break

    A = [r for r in A if any(abs(x)>1e-12 for x in r[:-1])] # remove 0 rows
    return A


def gauss_jordan(A:Matrix, b:Vector|None=None)->RRef:
    """
    Perform Gauss–Jordan elimination.
    If b is given, solve Ax = b.
    If b is None, return the RREF of A.

    A: list of lists (matrix)
    b: list (vector) or None
    """
    # Build augmented matrix if needed
    if b is not None:
        A = [row + [rhs] for row, rhs in zip(A, b)]

    m = len(A)
    n = len(A[0])

    row = 0
    for col in range(n):
        # Find pivot
        pivot = None
        for r in range(row, m):
            if A[r][col] != 0:
                pivot = r
                break
        if pivot is None:
            continue

        # Swap pivot row into position
        A[row], A[pivot] = A[pivot], A[row]

        # Normalize pivot row
        pivot_val = A[row][col]
        A[row] = [v / pivot_val for v in A[row]]

        # Eliminate other rows
        for r in range(m):
            if r != row and A[r][col] != 0:
                factor = A[r][col]
                A[r] = [A[r][c] - factor * A[row][c] for c in range(n)]

        row += 1
        if row == m:
            break

    return A


def solve(rref:RRef, vars:Vector)->Vector:
    n = len(rref[0])-1
    pivot_vars = [r.index(1) for r in rref][::-1]
    #free_vars = [r for r in range(n) if r not in set(pivot_vars)]
    res = [v for v in vars]
    for i, r in enumerate(rref[::-1]):
        p = pivot_vars[i]
        summands = [r[j]*res[j] for j in range(p+1, len(r)-1)]
        res[p] = r[n] - sum(summands)

    return res


def diagonalize(A:Matrix, b:Vector|None) -> (RRef, PivotVars, FreeVars):
    rref = gauss_jordan_partial_pivot(A, b)
    #rref = gauss_jordan(A, b)

    for r,row in enumerate(rref):
        for c in range(len(row)):
            rref[r][c] = round(row[c]) if math.isclose(row[c],round(row[c]), rel_tol=1e-9) else row[c]

    print("????????????????????????????")
    print(rref)
    print("????????????????????????????")
    pivot_vars = [r.index(1) for r in rref][::-1]
    free_vars = [r for r in range(len(rref[0])-1) if r not in set(pivot_vars)][::-1]
    return (rref, pivot_vars, free_vars)
#   pivot_vars = [r.index(1) for r in rref][::-1]
#   free_vars = [r for r in range(len(rref[0])-1) if r not in set(pivot_vars)][::-1]
#   return (rref, pivot_vars, free_vars)

def solution_range(rref:RRef, free_vars:FreeVars) -> dict[int,int]:
    sols:dict[int,int] = dict()
    for v in free_vars:
        for r in rref:
            if r[-1]<=0: continue
            if r[v]>0:
                q = r[-1]//r[v]
                qq =  round(q) if math.isclose(q,round(q), rel_tol=1e-9) else q
                if v in sols.keys():
                    sols[v] = min(sols[v], qq)
                else:
                    sols[v] = qq
    return sols

def enumerate_solution_range(sols:dict[int,int], free_vars:FreeVars) -> list[Vector]:
    cands:list[list[int]] = []
    vars = [0]*(len(free_vars))
    l = 0
    cands.append([v for v in vars])
    while l < len(vars):
        vars[l] = (vars[l] + 1) % (sols[free_vars[l]] + 1)
        if l<=len(vars)-1 and vars[l] == 0: 
            l += 1
            if l == len(vars): break
        else:
            #print(f"{l} {vars}")
            cands.append([v for v in vars])
            l = 0
    return cands

def solutions(A:Matrix, b:Vector|None=None) -> list[Vector]:
    rref,_, free_vars = diagonalize(A,b)
    free_vars_ranges = solution_range(rref, free_vars)
    cands = enumerate_solution_range(free_vars_ranges, free_vars)

    # enumerate solutions
    vars = [0]*(len(rref[0])-1)
    ress = []*(len(cands))
    for cand in cands:
        for i,f in enumerate(free_vars):
            vars[f] = cand[i]
        res = solve(rref, vars)
        if all(x >= 0 for x in res): ress.append(res)
        #print(res, sum(res))

    return ress

def check_solution(A:Matrix, b:Vector, solution:Vector) -> True|(int,int):
    for j,r in enumerate(A):
        s = 0
        for i,c in enumerate(r):
            s += c*solution[i]
        #assert s == b[j], f"expected {b[j]} but got {s}"
        if s != b[j]: return ( j, i)
    return True


if __name__=='__main__':
    print("------------------------------")
    #line = sys.stdin.readline().strip()
    #print(line)
    print("------------------------------")
    A = [
        [0,0,0,0,1,1],
        [0,1,0,0,0,1],
        [0,0,1,1,1,0],
        [1,1,0,1,0,0]
    ]
    b = [3,5,4,7]

#    rref,_, free_vars = diagonalize(A,b)
#    free_vars_ranges = solution_range(rref, free_vars)
#    cands = enumerate_solution_range(free_vars_ranges, free_vars)
#
#    # enumerate solutions
#    vars = [0]*(len(rref[0])-1)
#    ress = []*(len(cands))
#    for cand in cands:
#        for i,f in enumerate(free_vars):
#            vars[f] = cand[i]
#        res = solve(rref, vars)
#        ress.append(res)
#        #print(res, sum(res))
#
    print("--- all solutions --")
    ress = solutions(A,b)
    for res in ress: print(f"{res} has sum {sum(res)}")
    print("--------------------")
    
    #check it
    for res in ress:
        check_result = check_solution(A,b, res)
        assert check_result == True
    
    minimal_sum_solutions = [(r, sum(r)) for r in ress if sum(r) == min([sum(r0) for r0 in ress])]
    print(f"minimal sum solutions: {minimal_sum_solutions}")

