from sys import path
from sys import stdin
from sys import maxsize
import re
from functools import reduce
path.append('/home/rsh/devel/Projects/AoC2025/python')
from gauss_jordan import Matrix
from gauss_jordan import Vector
from gauss_jordan import solutions
from gauss_jordan import check_solution


#    print("--- all solutions --")
#    ress = solutions(A,b)
#    for res in ress: print(f"{res} has sum {sum(res)}")
#    print("--------------------")
#    
#    #check it
#    for res in ress:
#        check_result = check_solution(A,b, res)
#        assert check_result == True
#
#    minimal_sum_solutions = [(r, sum(r)) for r in ress if sum(r) == min([sum(r0) for r0 in ress])]
if __name__=='__main__':


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
            A:Matrix = [[0 for _ in range(m)] for _ in range(n)]
            for c in range(m):
                for i in jolts[c]:
                    A[i][c] = 1
            b:Vector = list(map(int,target))

        print(A)
        print(b)
        ress = solutions(A,b)
        min_sum = maxsize
        for res in ress:
            s = sum(res)
            if s.is_integer():
                assert check_solution(A,b, res)
                min_sum = min(min_sum, s)

        #for res in ress: print(f"{res} has sum {sum(res)}")
        print(f"min sum: {min_sum}")



