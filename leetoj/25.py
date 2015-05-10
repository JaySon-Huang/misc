#! /usr/bin/env python
#encoding=utf-8

# Reverse Nodes in k-Group

# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, x):
#         self.val = x
#         self.next = None
#     def __str__(self):
#         return '{%d}'%self.val

# def printList(head):
#     s = ''
#     while head is not None:
#         s += str(head)
#         head = head.next
#     return s

class Solution:
    # @param head, a ListNode
    # @param k, an integer
    # @return a ListNode
    def reverseKGroup(self, head, k):
        if head is None or k <= 1:
            return head

        tmpHead = ListNode(-1)
        tmpHead.next = head

        pre, front= tmpHead, head

        while front is not None:
            # test if it length >= k
            tail = front
            for x in range(k-1):
                if tail.next is None:
                    # len < k, left it remains
                    return tmpHead.next
                else:
                    tail = tail.next
            tailN = tail.next
            # print("%s -> %s"%(front, tail))
            
            # fix the link in/out this K Group
            pre.next = tail
            nextN = front.next
            frontN = front
            front.next = tail.next

            # reverse link list
            for x in range(k-1):
                t = nextN.next
                nextN.next = front
                front = nextN
                nextN = t

            # move to next K Group
            pre = frontN
            front = tailN
            # print(printList(tmpHead))
            # print("next: %s  %s"%(pre, front))

        # return head while the loop is over
        return tmpHead.next

# head = ListNode(-1)
# tail = head
# for x in xrange(1,3):
#     n = ListNode(x)
#     tail.next = n
#     tail = tail.next

# print(printList(head))
# ret = Solution().reverseKGroup(head.next, 2)
# print(printList(ret))
