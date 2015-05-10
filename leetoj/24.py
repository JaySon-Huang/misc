#! /usr/bin/env python
#encoding=utf-8

# Swap Nodes in Pairs

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
    # @param a ListNode
    # @return a ListNode
    def swapPairs(self, head):
        # edge case
        if head is None:
            return None

        tmpHead = ListNode(0)
        tmpHead.next = head

        pre, front, back = tmpHead, head, head.next

        while head is not None and back is not None:
            # swap相邻的两个结点
            pre.next = back
            front.next = back.next
            back.next = front

            # 指向下一组需要操作的结点
            pre = front
            front = front.next
            if front is None:
                back = None
                break
            else:
                back = front.next

        return tmpHead.next

# head = ListNode(0)
# tail = head
# for x in xrange(11):
#     n = ListNode(x)
#     tail.next = n
#     tail = tail.next

# print(printList(head.next))
# ret = Solution().swapPairs(head.next)
# print(printList(ret))
