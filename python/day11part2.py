"""
AOC 2025 Day 10 Part 2 Solution
"""
from functools import lru_cache, cache
from typing import List, Sequence, Protocol, TypeVar
import sys
import os
from dataclasses import dataclass
from collections import deque
from enum import Enum
import pdb

class SupportsEq(Protocol):
    def __eq__(self, other: object) -> bool: ...

TEq = TypeVar("TEq", bound=SupportsEq)

def is_cycling(xs:Sequence[TEq])->bool:
    if len(xs) == 0: 
        return False

    j:int = len(xs)-1
    i:int = j-1

    # a b c b c
    #         ^
    #         j
    #     ^
    #     i

    check:TEq = xs[j]
    while i >= 0 and xs[i] != check:
        i -= 1

    if i < 0:
        return False

    l = i 
    for k in range(j, i, -1):
        if xs[k] != xs[l]:
            return False
        l -= 1

    return True

@dataclass
class StackElem:
    """
    Stack Element
    """
    node:str
    fft_dac:int
    path:List[str]

def alg_with_visited_path(adj:dict[str, set[str]]) -> int:
    st:list[StackElem] = []  # (cur, prev)
    visited:set[tuple[str,str]] = set()
    st.append(StackElem('svr', 0, []))
    path_count = 0
    while len(st) != 0:
        cur:StackElem = st.pop()
        #print(f"cur: {cur}/{fft_dac} visited {visited} paths_count: {path_count}")
        if cur.node == 'out':
            # path found
            if cur.fft_dac & 0b11 == 0b11:
                path_count += 1
                #print(f"path {cur.path + [cur.node]}")
            continue
        if (cur.node,'/'.join((set(cur.path)))) in visited:
            continue
        for n in adj[cur.node]:
            fft_dac_inc = 0
            if n == 'fft' :
                fft_dac_inc = 0b01
            if n == 'dac':
                fft_dac_inc = 0b10
            st.append(StackElem(n, cur.fft_dac | fft_dac_inc, cur.path + [cur.node]))
        visited.add((cur.node,'/'.join(set(cur.path))))
    return path_count

def alg_with_visited_path1(adj:dict[str, set[str]]) -> int:
    q:list[StackElem] = []  # (cur, prev)
    q.append(StackElem('svr', 0, []))
    path_count = 0
    while len(q) != 0:
        cur:StackElem = q.pop(-1)
        if cur.node == 'out':
            # path found
            if cur.fft_dac & 0b11 == 0b11:
                path_count += 1
                print(f"path {path_count}")#: {cur.path + [cur.node]}")
            continue

        
        if is_cycling(cur.path):
            print("cycling")
            continue

        for n in adj[cur.node]:
           fft_dac_inc = 0
           if n == 'fft' :
               fft_dac_inc = 0b01
           if n == 'dac':
               fft_dac_inc = 0b10
           q.append(StackElem(n, cur.fft_dac | fft_dac_inc, cur.path + [cur.node]))
    return path_count

def has_cycle(adj:dict[str, set[str]]) -> bool:
    class Color(Enum):
        WHITE = 0 # not yet checked
        GRAY = 1  # in progress
        BLACK = 2 # ready has no cycle so far
    @dataclass
    class Node:
        node:str
        prev:Node|None
    color:dict[str,Color] = dict((nd, Color.WHITE) for nd in adj.keys())
    def dfs(start:str)->bool:
        st:deque[Node] = deque()
        st.append(Node(start,None))
        while len(st) > 0:
            cur = st.pop()
            color[cur.node] = Color.GRAY
            have_to_check_next:bool = False
            for next in adj[cur.node]:
                if next in adj.keys():
                    if color[next] == Color.WHITE:
                        have_to_check_next = True
                        st.append(Node(next, cur)) # check neighbours
                    elif color[next] == Color.GRAY:
                        return True # cycle detected
            if not have_to_check_next:
                while cur != None:
                    color[cur.node] = Color.BLACK
                    cur = cur.prev

        color[start] = Color.BLACK
        return False

    for nd in color:
        if color[nd] == Color.WHITE:
            if dfs(nd):
                return True
    return False
            


def alg_with_visited_path2(adj:dict[str, set[str]]) -> int:
    def bfs(start:str, end:str)->List[List[str]]:
        res:List[List[str]] = []
        @dataclass
        class QElem:
            node:str
            prev:QElem|None
        q:deque[QElem] = deque()
        visited:set[tuple[str,str|None]] = set()
        q.append(QElem(start, None))
        while len(q) > 0:
            cur = q.popleft()
            if cur.node == end:
                res.append([])
                while cur != None:
                    res[-1].insert(0,cur.node)
                    cur = cur.prev
                continue
            else:
                if (cur.node, None if cur.prev is None else cur.prev.node) in visited: continue
                visited.add((cur.node, None if cur.prev is None else cur.prev.node))
                if cur.node in adj.keys():
                    for n in adj[cur.node]:
                        q.append(QElem(n, cur))
        return res

    

    paths_svr_to_fft = len(bfs('svr', 'fft'))
    paths_svr_to_dac = len(bfs('svr', 'dac'))
    paths_dac_to_out = len(bfs('dac', 'out'))
    paths_fft_to_out = len(bfs('fft', 'out'))
    paths_dac_to_fft = len(bfs('dac', 'fft'))
    paths_fft_to_dac = len(bfs('fft', 'dac'))
    path_count = paths_svr_to_dac*paths_dac_to_fft*paths_fft_to_out + \
                 paths_svr_to_fft*paths_fft_to_dac*paths_dac_to_out
    print(f"{path_count}\n \
            {paths_svr_to_dac} {paths_dac_to_fft} {paths_fft_to_out}\n \
            {paths_svr_to_fft} {paths_fft_to_dac} {paths_dac_to_out}")
    # 288 too low
    # 5508 too low
    return path_count

def alg_with_visited_path3(adj:dict[str, set[str]]) -> int:
    #@cache
    memo = {}
    def allPaths(start:str, end:str)->List[List[str]]:
        if start == 'out' and end != 'out':
            return []
        if start in memo.keys():
            return memo[start]
        if start == end:
            memo[end] = [[end]]
            return [[end]]
        else:
            res:List[List[str]] = []
            for neighbour in adj[start]:
                for sub_path in allPaths(neighbour, end):
                    if sub_path:
                        full_path = [start] + sub_path
                        res.append(full_path)
            memo[start] = res
            return res

    paths_dac_to_fft = 0
    paths_fft_to_dac = 0
    paths_svr_to_fft = 0
    paths_dac_to_out = 0
    paths_fft_to_out = 0
    paths_svr_to_dac = 0
    
    paths_dac_to_fft = len(allPaths('dac', 'fft'))
    print(f"paths_dac_to_fft={paths_dac_to_fft}")

    memo.clear()
    paths_fft_to_dac = len(allPaths('fft', 'dac'))
    print(f"paths_fft_to_dac={paths_fft_to_dac}")

    memo.clear()
    paths_svr_to_fft = len(allPaths('svr', 'fft'))
    print(f"paths_svr_to_fft={paths_svr_to_fft}")
    
    memo.clear()
    paths_dac_to_out = len(allPaths('dac', 'out'))
    print(f"paths_dac_to_out={paths_dac_to_out}")

    if paths_dac_to_fft > 0:
        # i found out, that path fft->dac exists, thus there can not exist
        # dac->fft path because otherwise there would exist a cycle
        # thus one can ommit the following
        memo.clear()
        paths_fft_to_out = len(allPaths('fft', 'out'))
        print(f"paths_fft_to_out={paths_fft_to_out}")

        memo.clear()
        paths_svr_to_dac = len(allPaths('svr', 'dac'))
        print(f"paths_svr_to_dac={paths_svr_to_dac}")


    path_count = paths_svr_to_dac*paths_dac_to_fft*paths_fft_to_out + \
                 paths_svr_to_fft*paths_fft_to_dac*paths_dac_to_out
    print(f"{path_count}\n \
            {paths_svr_to_dac} {paths_dac_to_fft} {paths_fft_to_out}\n \
            {paths_svr_to_fft} {paths_fft_to_dac} {paths_dac_to_out}")
    # 288 too low
    # 5508 too low
    return path_count

if __name__=='__main__':
    print(f"AOC 2025 day 11 part 2 {sys.argv}")
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <test file name>")

    if not os.path.isfile(sys.argv[1]):
        print(f"Error: {sys.argv[1]} does not exists")

    with open(sys.argv[1], "r", encoding="ascii") as infile:

        adj:dict[str, set[str]] = {}
        for line in infile:
            line = line.rstrip("\n")  # Remove only the newline at the end
            if line.strip():  # Skip completely empty lines
                strs = line.split(" ")
                start_node = strs[0][:-1]
                neighbours = strs[1:]
                adj.update({start_node: set(neighbours)})

        print(f"has cycle: {has_cycle(adj)}")

        print(f"Result: {alg_with_visited_path3(adj)}")

#        xs = [1,2,3,2,3]
#        print(f"{xs} {is_cycling(xs)}")
#        xs = [1,2,3,1,2,3]
#        print(f"{xs} {is_cycling(xs)}")
#        xs = [1,2,3,4,2,3]
#        print(f"{xs} {is_cycling(xs)}")
