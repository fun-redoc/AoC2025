import sys
import re

if __name__=='__main__':
    print("AOC 2025 day 11 part 1")
    adj:dict[str, set[str]] = dict()
    for line in sys.stdin:
        line = line.rstrip("\n")  # Remove only the newline at the end
        if line.strip():  # Skip completely empty lines
            strs = line.split(" ")
            start_node = strs[0][:-1]
            neighbours = strs[1:]
            adj.update({start_node: set(neighbours)})

    st:list[tuple[str, str|None]] = []  # (cur, prev)
    visited:set[str] = set()
    st.append(('you', None))
    path_count = 0
    while not len(st) == 0:
        cur,prev = st.pop()
        if cur in visited: continue
        if cur == 'out':
            # path found
            path_count += 1
            continue
        neighbours = adj[cur]
        for n in neighbours:
            st.append((n, cur))

    print(f"Result: {path_count}")

