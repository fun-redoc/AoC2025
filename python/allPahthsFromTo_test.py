from os import wait
import unittest
from timeout import TestTimeoutError, timeout
from allPahthsFromTo import allPathsSourceTarget, allPathsSourceTarget_rec

class TestAllPath(unittest.TestCase):
#    @timeout(3)
#    def test_rec_1(self):
#        test_case_1 = [[1,2],[3],[3],[]]
#        self.assertListEqual(allPathsSourceTarget_rec(test_case_1), \
#                             [[0,1,3],[0,2,3]] )
#    @timeout(3)
#    def test_rec_2(self):
#        test_case_2 = [[1, 2, 3, 4], [2, 3, 4], [3, 4], [4], []]
#        self.assertListEqual(allPathsSourceTarget_rec(test_case_2), \
#                            [[0,1,2,3,4],[0,1,2,4],[0,1,3,4],[0,1,4],[0,2,3,4],[0,2,4],[0,3,4],[0,4]])
#
#    @timeout(3)
#    def test_rec_3(self):
#        test_case_2 = [[1, 2], [3], [3], [4], []]
#        self.assertListEqual(allPathsSourceTarget_rec(test_case_2), \
#                            [[0,1,3,4],[0,2,3,4]])
##
#    @timeout(3)
#    def test_1(self):
#        test_case_1 = [[1,2],[3],[3],[]]
#        self.assertListEqual(allPathsSourceTarget(test_case_1), \
#                             [[0,1,3],[0,2,3]] )
#    @timeout(3)
#    def test_2(self):
#        test_case_2 = [[1, 2, 3, 4], [2, 3, 4], [3, 4], [4], []]
#        self.assertListEqual(allPathsSourceTarget(test_case_2), \
#                            [[0,1,2,3,4],[0,1,2,4],[0,1,3,4],[0,1,4],[0,2,3,4],[0,2,4],[0,3,4],[0,4]])
#
#    @timeout(3)
#    def test_3(self):
#        test_case_2 = [[1, 2], [3], [3], [4,5], [6], [6], [7], []]
#        self.assertListEqual(allPathsSourceTarget(test_case_2), 
#                            [[0,1,3,4,6,7],[0,1,3,5,6,7],[0,2,3,4,6,7],[0,2,3,5,6,7]])
#
#    @timeout(3)
#    def test_4(self):
#        test_case_2 = [[1, 2], [3], [3], [4], []]
#        self.assertListEqual(allPathsSourceTarget(test_case_2), \
#                            [[0,1,3,4],[0,2,3,4]])
#
#    @timeout(3)
#    def test_rec_5(self):
#        test_case_2 = [[1, 2, 3, 4], [2, 3, 4], [3, 4], [4], []]
#        self.assertListEqual(allPathsSourceTarget_rec(test_case_2), \
#                            [[0,1,2,3,4],[0,1,2,4],[0,1,3,4],[0,1,4],[0,2,3,4],[0,2,4],[0,3,4],[0,4]])
#    @timeout(3)
#    def test_5(self):
#        test_case_2 = [[1, 2, 3, 4], [2, 3, 4], [3, 4], [4], []]
#        self.assertListEqual(allPathsSourceTarget(test_case_2), \
#                            [[0,1,2,3,4],[0,1,2,4],[0,1,3,4],[0,1,4],[0,2,3,4],[0,2,4],[0,3,4],[0,4]])
    @timeout(3)
    def test_5(self):
        test_case = [[1, 2, 3, 4], [2, 3, 4], [3, 4], [4], []]
        self.assertListEqual(allPathsSourceTarget(test_case), \
                             allPathsSourceTarget_rec(test_case))
