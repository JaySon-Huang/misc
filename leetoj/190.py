# Reverse Bits 
# https://leetcode.com/problems/reverse-bits/

class Solution:
    # @param n, an integer
    # @return an integer
    def reverseBits(self, n):
        # print '{0:032b}'.format(n)
        ret = 0
        for x in range(32):
            ret <<= 1
            ret |= n&1
            n >>= 1
        # print '{0:032b}'.format(ret)
        return ret

# print Solution().reverseBits(43261596)
# print Solution().reverseBits(1)
