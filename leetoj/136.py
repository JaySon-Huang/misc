#encoding=utf-8
# Single Number
# https://leetcode.com/problems/single-number/
class Solution:
    # @param A, a list of integer
    # @return an integer
    # 全部数异或一下就出来了
    def singleNumber(self, A):
        ret = 0
        for x in A:
            ret ^= x
        return ret

print Solution().singleNumber([1,1,4,4,-4,-4,-421312,8,7382913,9,9,8,-421312])
