#encoding=utf-8
# Rotate Array
# https://leetcode.com/problems/rotate-array/
# [数组左旋转k位 —— C++标准算法库中最悲剧的函数：rotate](http://www.cnblogs.com/flyinghearts/archive/2011/05/27/2060265.html)
# [Summary of C++ solutions](https://leetcode.com/discuss/27387/summary-of-c-solutions)

class Solution:
    # @param nums, a list of integer
    # @param k, num of steps
    # @return nothing, please modify the nums list in-place.
    # 需要本地修改数组，即O(1)空间复杂度

    # 时间复杂度为O(n^2)的:一步一步地向右移,移动k次
    # def rotate(self, nums, k):
    #     for _ in xrange(k):
    #         last = nums[-1]
    #         for i in xrange(len(nums)-1, 0, -1):
    #             nums[i] = nums[i-1]
    #         nums[0] = last

    # O(n)的空间复杂度的
    def rotate(self, nums, k):
        nums_copy = nums[:]
        length = len(nums)
        for i in xrange(length):
            nums[(i+k)%length] = nums_copy[i]

    # 分组进行互换
    # def rotate(self, nums, k):
    #     length = len(nums)
    #     idx = 0
    #     distance = 0
    #     cur = nums[0]
    #     for x in range(length):
    #         # cur的值被换到的位置
    #         next = (idx + k) % length
    #         # 保存next位置原来的值
    #         temp = nums[next]
    #         nums[next] = cur
    #         # 把next位置原来的值作为下一个值
    #         idx = next
    #         cur = temp

    #         distance = (distance + k) % length
    #         if distance == 0:
    #             idx = (idx + 1) % length
    #             cur = nums[idx]

l = [x for x in xrange(1,8)]
Solution().rotate(l, 3)
print l
