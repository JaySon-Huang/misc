#! /usr/bin/env python
#encoding=utf-8

# Add Two Numbers

# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, x):
#         self.val = x
#         self.next = None

class Solution:
    # @return a ListNode
    def addTwoNumbers(self, l1, l2):
        t1 = l1
        t2 = l2
        head, tail, cur = None, None, None
        hasIncr = False
        while t1 is not None or t2 is not None:
            # 考虑
            curVal = 0
            curVal += t1.val if t1 else 0
            curVal += t2.val if t2 else 0
            curVal += 1 if hasIncr else 0
            hasIncr = curVal/10 > 0
            curVal %= 10

            cur = ListNode(curVal)
            if head is None:
                head = cur
                tail = head
            else:
                tail.next = cur
                tail = tail.next

            t1 = t1.next if t1 else None
            t2 = t2.next if t2 else None

        if hasIncr :
            tail.next = ListNode(1)

        return head
