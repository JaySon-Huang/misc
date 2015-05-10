# Number of 1 Bits
# https://leetcode.com/problems/number-of-1-bits/

class Solution:
    # @param n, an integer
    # @return an integer
    def hammingWeight(self, n):
        cnt = 0
        while n:
            cnt += n&1
            n >>= 1
        return cnt

# print Solution().hammingWeight(11)
