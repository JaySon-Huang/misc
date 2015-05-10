#! /usr/bin/env python
#encoding=utf-8

# TODO have not fix this problem

class Solution:
    # @return a float
    def findMedianSortedArrays(self, A, B):
        m = len(A)
        n = len(B)
        # 保证A不比B短
        if m < n:
            return self.findMedianSortedArrays(B, A)

        print("A:", A)
        print("B:", B)
        if n == 0:
            return A[m/2]

        print("A[%d]:%d - B[%d]:%d"% (m/2, A[m/2], n/2, B[n/2]))
        if m == 1 and n == 1:
            return (A[0]+B[0])/2.0

        if A[m/2] == B[n/2]:
            return A[m/2]
        elif A[m/2] < B[n/2]:
            return self.findMedianSortedArrays(A[m/2+1:], B[:n/2])
        else :
            return self.findMedianSortedArrays(A[:m/2], B[n/2+1:])

# a = [1,2,3,7,9,10]
# b = [5,7,7,9,10,20,100,108]
#TDD case 1:result 1.5
a = [1]
b = [2]
ret = Solution().findMedianSortedArrays(a, b)
print(ret)
assert ret == 1.5
# case 2:result 1
a = []
b = [1]
ret = Solution().findMedianSortedArrays(a, b)
print(ret)
assert ret == 1
# case 3:result 3.5
a = [1,2,3]
b = [4,5,6]
ret = Solution().findMedianSortedArrays(a, b)
print(ret)
assert ret == 3.5
# case 4:result 3.5
a = [4,5,6]
b = [1,2,3]
ret = Solution().findMedianSortedArrays(a, b)
print(ret)
assert ret == 3.5
# case 5:result 3
b = [1,2,3]
a = [5,6]
ret = Solution().findMedianSortedArrays(a, b)
print(ret)
assert ret == 3
