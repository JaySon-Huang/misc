#! /usr/bin/env python
#encoding=utf-8

# Remove Nth Node From End of List 

# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, x):
#         self.val = x
#         self.next = None
#     def __str__(self):
#         return "{%d}"%self.val

class Solution:
    # @return a ListNode
    def removeNthFromEnd(self, head, n):
        if head is None:
            return None

        fast, slow = head, head
        slowPre = slow
        # assume that n will always be valid
        # so go ahead n steps
        for x in range(n):
            fast = fast.next
        # if the node to be deleted is excatly the head
        # 处理倒数第n个为头指针的边界情况
        if fast is None:
            return head.next

        # let slow point to the nth node from the end of list
        while fast:
            fast = fast.next
            slowPre = slow
            slow = slow.next

        # delete the node from list
        slowPre.next = slow.next if slow else None

        return head

head = ListNode(1)
print(Solution().removeNthFromEnd(head, 1))
