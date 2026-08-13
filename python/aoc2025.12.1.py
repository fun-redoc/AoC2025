import sys
import re
import array
from functools import cache
from typing import IO
from enum import Enum
from dataclasses import dataclass
from typing import List, Tuple
from collections import deque

"""
AoC 2025 Day 12
run with: aoc -y 2025 -d 12 | python aoc2025.12.1.py
"""

Present = array.array

class Field:
    present_shapes:List[Present]   
    #w:int
    #h:int
    #present_counts:array.array
    #presents:List[Tuple[int, int, int]] = []
        
    def __init__(self, w:int, h:int, present_counts:array.array):
        self.w = w
        self.h = h
        self.field:array.array = array.array('b',[0]*w*h)
        self.present_counts = present_counts
        self.presents:deque[Tuple[int,int,int,int]] = deque()
    def __str__(self):
        print_field:List[str] = ['.'*self.w for _ in range(self.h)]
        for r in range(self.h):
            for c in range(self.w):
                pass
        return '\n'.join(print_field)


def read_puzzle(stream:IO[str]) -> Tuple[List[Present], List[Field]]:
    class State(Enum):
        present_idx = 0
        present_line = 1
        tree = 2

    presents:List[Present] = []
    trees:List[Field] = []

    state:State = State.present_idx
    cnt:int = 0
    for raw_line in stream:
        line = raw_line.strip()
        if not line:
            continue
        if state == State.present_idx:
            if re.match(r'\d:', line):
                presents.append(array.array('b', [0]*3*3))
                state = State.present_line
                continue
            else:
                state = State.tree
        if state == State.present_line:
            for i, c in enumerate(line):
                presents[-1][i+cnt*3] = 1 if c=='#' else 0
            cnt += 1
            if cnt == 3:
                cnt = 0
                state = State.present_idx
                continue
        if state == State.tree:
            nums = re.findall(r'\d+', line)
            tree = Field(int(nums[0]), int(nums[1]), \
                        array.array('b', map(int, nums[2:])))
            trees.append(tree)
    return presents, trees


def rotate(p:Present)->Present:
    """
     1 2 3 left   3 6 9  
     4 5 6 ---->  2 5 8  
     7 8 9        1 4 7
    """
    return array.array('b', [p[i-1] for i in [3,6,9,2,5,8,1,4,7]]) 
def flipv(p:Present)->Present:
    """
     1 2 3 flip   3 2 1  
     4 5 6 ---->  6 5 4  
     7 8 9        9 8 7
    """
    return array.array('b', [p[i-1] for i in [3,2,1,6,5,4,9,8,7]]) 
def fliph(p:Present)->Present:
    """
     1 2 3 flip   7 8 9  
     4 5 6 ---->  4 5 6  
     7 8 9        1 2 3
    """
    return array.array('b', [p[i-1] for i in [7,8,9,4,5,6,1,2,3]]) 


def print_present(p:Present) -> None:
    for r in range(3):
        print_row:str = ""
        for c in range(3):
            e = p[3*r + c]
            print_row += '.' if e == 0 else '#'
        print(print_row)

if __name__=='__main1__':
    raw_puzzles = [
    """
    ##.
    .##
    ..#
    ""","""
    ###
    .#.
    ###
    ""","""
    .##
    ##.
    ###
    ""","""
    ###
    #.#
    #.#
    ""","""
    .##
    .##
    ###
    ""","""
    ###
    .##
    ..#
    """]
    puzzles = [Present('b',[1 if c=='#' else 0 for c in re.sub(r'[^#\.]','',s)]) for s in raw_puzzles]

    distinct_puzzles:List[set[Present]] = [set() for _ in puzzles]
    for i,puzzle in enumerate(puzzles):
        p = puzzle
        distinct_puzzles[i].add(p.tobytes())
        for _ in range(3):
            p = rotate(p)
            distinct_puzzles[i].add(p.tobytes())

        p = flipv(puzzle)
        distinct_puzzles[i].add(p.tobytes())
        for _ in range(3):
            p = rotate(p)
            distinct_puzzles[i].add(p.tobytes())

        p = fliph(puzzle)
        distinct_puzzles[i].add(p.tobytes())
        for _ in range(3):
            p = rotate(p)
            distinct_puzzles[i].add(p.tobytes())

        p = fliph(flipv(puzzle))
        distinct_puzzles[i].add(p.tobytes())
        for _ in range(3):
            p = rotate(p)
            distinct_puzzles[i].add(p.tobytes())
        
        p = flipv(fliph(puzzle))
        distinct_puzzles[i].add(p.tobytes())
        for _ in range(3):
            p = rotate(p)
            distinct_puzzles[i].add(p.tobytes())


    print([len(s) for s in distinct_puzzles])
    for i in range(6):
        #print(re.sub(r'[^#\.]', '', raw_puzzles[i].strip()))
        print_present(puzzles[i])
    for i in range(6):
        print("===================")
        for p in distinct_puzzles[i]:
            arr = array.array('b')
            arr.frombytes(p)
            print("--------------------")
            print_present(arr)

    field_counts = [sum(p) for p in puzzles]
    print(field_counts)

if __name__=='__main__':
    presents, fields = read_puzzle(sys.stdin)
    field_counts = [sum(p) for p in presents]
    print(f"field counts: {field_counts}")
    Field.present_shapes = presents
    cnt = 0
    for f, field in enumerate(fields):
        field_size = field.w*field.h
        space_at_least_needed = sum(c*field_counts[i] for (i,c) in enumerate(field.present_counts))
        print(f"field {f}: size {field_size} space needed {space_at_least_needed}")
        if field_size >= space_at_least_needed:
            cnt += 1
    print(f"{cnt} of {len(fields)} regions can probably fit")
    # Thats it?
