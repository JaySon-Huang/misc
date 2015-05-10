# Best Time to Buy and Sell Stock II 
# https://leetcode.com/problems/best-time-to-buy-and-sell-stock-ii/

class Solution:
    # @param prices, a list of integer
    # @return an integer
    # O(n)
    def maxProfit(self, prices):
        if len(prices) == 0:
            return 0
        cur_min = prices[0]
        max_profit = 0
        for i in xrange(1, len(prices)):
            cur_min = min(cur_min, prices[i])
            max_profit = max(max_profit, prices[i] - cur_min)
        return max_profit

print Solution().maxProfit([1,2,3,4,5,6,])
print Solution().maxProfit([2,3,4,5,6,1])
print Solution().maxProfit([2,3,4,5,6,1,9])
