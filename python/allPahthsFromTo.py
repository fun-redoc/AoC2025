from dataclasses import dataclass
from collections import deque
from typing import List
import pdb

test_case_1 = [[1,2],[3],[3],[]]
test_case_2 = [[1, 2, 3, 4], [2, 3, 4], [3, 4], [4], []]

def allPathsSourceTarget_rec(graph: List[List[int]]) -> List[List[int]]:

    #@lru_cache
    memo = [[] for _ in range(len(graph))]
    def allPath(node:int)->List[List[int]]:
      if memo[node]:
        return memo[node]
      res:List[List[int]] = []
      if node == len(graph)-1:
        memo[node] = [[node]]
        res.append([node])
        return res
      else:
        for neighbour in graph[node]:
          for sub_path in allPath(neighbour):
              full_path = [node] + sub_path
              res.append(full_path)
        memo[node] = res
        return res

    return allPath(0)

def allPathsSourceTarget_no_memo(graph: List[List[int]]) -> List[List[int]]:
    @dataclass
    class StackElem:
        node:int
        prev:StackElem|None

    memo:List[List[List[int]]] = [[] for _ in range(len(graph))]
    def allPath(start_node:int)->List[List[int]]:
      res:List[List[int]] = []
      st:deque[StackElem] = deque()
      st.append(StackElem(start_node, None))
      #pdb.set_trace()
      # TODO MEMOIZATION
      while st:
          cur = st.pop()
          if cur.node == len(graph)-1:
              res.append([])
              p = cur
              while p:
                res[-1].insert(0, p.node)
                p = p.prev
          else:
              if memo[cur.node]:
                  #pdb.set_trace()
                  pass # TODO
              else:
                  for neighbour in reversed(graph[cur.node]):
                    st.append(StackElem(neighbour, cur))
      return res

    return allPath(0)

def allPathsSourceTarget(graph: List[List[int]]) -> List[List[int]]:
    @dataclass
    class StackElem:
        node:int
        path:List[int]

    memo:List[List[List[int]]] = [[] for _ in range(len(graph))]
    def allPath(start_node:int)->List[List[int]]:
      res:List[List[int]] = []
      st:deque[StackElem] = deque()
      st.append(StackElem(start_node, []))
      memo:List[List[List[int]]] = [[] for _ in range(len(graph))]
      while st:
          cur = st.pop()
          if memo[cur.node]:
              # TODO Problem not all pathes from cur.node to destination ready
              # at this point in time
              print(f"cur.node {cur}")
              print(f"memo: {memo}")
              print(f"res before: {res}")
              cur_full_path = cur.path + [cur.node]
              for memo_path in memo[cur.node]:
                  res.append(cur_full_path + memo_path)
              print(f"res after: {res}")
              continue
          if cur.node == len(graph)-1:
              full_path = cur.path + [cur.node]
              for i, path_elem in enumerate(full_path):
                  memo[path_elem].append(full_path[i+1:])
              res.append(full_path)
          else:
              for neighbour in reversed(graph[cur.node]):
                st.append(StackElem(neighbour, cur.path + [cur.node]))
      return res

    return allPath(0)

if __name__=="__main__":
    assert [[0,1,3],[0,2,3]] == allPathsSourceTarget(test_case_1)

    res = allPathsSourceTarget(test_case_2)
    assert [[0,1,2,3,4],[0,1,2,4],[0,1,3,4],[0,1,4],[0,2,3,4],[0,2,4],[0,3,4],[0,4]] == res, f" got {res}"
