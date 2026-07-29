from sys import path
from sys import stdin
from sys import maxsize
import re
from functools import reduce
import pulp
import numpy as np

if __name__=='__main__':

    result = 0

    for line in stdin:
        line = line.rstrip("\n")  # Remove newline character
        if line.strip():  # Skip empty lines
            #print(f"Received line: {line}")
            print("----------------------------------")
            parts = line.split(" ")[1:]
            target = re.findall(r"\d+", parts[-1])
            jolts = [list(map(int,re.findall(r"\d+",j))) for j in parts[:-1]]
            n = len(target)
            m = len(jolts)
            A = [[0 for _ in range(m)] for _ in range(n)]
            for c in range(m):
                for i in jolts[c]:
                    A[i][c] = 1
            b = list(map(int,target))


# -----------------------------
# Example data
# -----------------------------
# Coefficient matrix A (m x n)
#A = np.array([
#    [1, 2, 3],
#    [4, 0, 1]
#])
            A = np.array(A)

# Right-hand side vector b (m)
#b = np.array([7, 8])
            b = np.array(b)

# Objective coefficients (n)
#c = np.array([3, 1, 2])
            c = np.array([1]*m)

# -----------------------------
# Create the problem
# -----------------------------
# We will minimize c^T x subject to A x = b, x >= 0, integer
            prob = pulp.LpProblem("Integer_LP_with_Matrix_Equality", pulp.LpMinimize)

# Decision variables: integer, non-negative
            x_vars = [
                pulp.LpVariable(f"x{i}", lowBound=0, cat=pulp.LpInteger)
                for i in range(len(c))
            ]

# Objective function
            prob += pulp.lpSum(c[i] * x_vars[i] for i in range(len(c))), "Total_Cost"

# -----------------------------
# Add matrix equality constraints
# -----------------------------
            for row_idx in range(A.shape[0]):
                prob += (
                    pulp.lpSum(A[row_idx, col_idx] * x_vars[col_idx] for col_idx in range(A.shape[1]))
                    == b[row_idx],
                    f"Constraint_row_{row_idx}"
                )

# -----------------------------
# Solve the problem
# -----------------------------
            status = prob.solve(pulp.PULP_CBC_CMD(msg=False))

# -----------------------------
# Output results
# -----------------------------
            #print("Status:", pulp.LpStatus[status])
            #for var in x_vars:
            #    print(f"{var.name} = {var.value()}")
            print("Objective value =", pulp.value(prob.objective))
            result += pulp.value(prob.objective)
    print(f"Result: {result}")
