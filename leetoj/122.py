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
        total_profit = 0
        isAscending = True
        for index in range(1, len(prices)):
            if prices[index] < prices[index-1]:
                isAscending = False
                total_profit += prices[index-1] - cur_min

                cur_min = prices[index]
        if cur_min != prices[-1]:
            total_profit += prices[-1] - cur_min
        return total_profit

print(Solution().maxProfit([1,2,3,4,5,6]))
print(Solution().maxProfit([2,3,4,5,6,1]))
print(Solution().maxProfit([2,3,4,5,6,1,2]))

# FIXME: Shoule return 12 but not 8
print(Solution().maxProfit([2,3,4,5,6,1,9]))
