# Move Zeroes
# https://leetcode.com/problems/move-zeroes/

class Solution(object):
    def moveZeroes(self, nums):
        """
        :type nums: List[int]
        :rtype: void Do not return anything, modify nums in-place instead.
        """
        zeroes_cnt = 0
        for index, num in enumerate(nums):
            if num == 0:
                zeroes_cnt += 1
            else:
                nums[index-zeroes_cnt] = num
            # print(nums)

        # print('set zeroes')
        for index in range(1, zeroes_cnt+1):
            nums[-index] = 0
            # print(nums)

sol = Solution()

nums = [0, 1, 0, 3, 12]
sol.moveZeroes(nums)
print(nums)

nums = [0, 0, 0, 0, 0]
sol.moveZeroes(nums)
print(nums)
