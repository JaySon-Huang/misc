#! /usr/bin/env python
#encoding=utf-8

# Merge Two Sorted Lists

# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, x):
#         self.val = x
#         self.next = None

class Solution:
    # @param two ListNodes
    # @return a ListNode
    def mergeTwoLists(self, l1, l2):
        t1, t2 = l1, l2
        head, tail = None, None
        # 如果其中一个是空的链表
        if t1 is None :
            return t2
        elif t2 is None:
            return t1

        # 当两个指针都不为空的时候，选择小的插入到新的链表中
        while t1 is not None and t2 is not None:
            # cur指向用来插入的结点
            if t1.val < t2.val:
                cur = t1
                t1 = t1.next
            else:
                cur = t2
                t2 = t2.next

            if head is None:
                head = cur
                tail = head
            else:
                tail.next = cur
                tail = tail.next

        # 把剩下的接到新链表中
        if t1 is not None:
            tail.next = t1
        elif t2 is not None:
            tail.next = t2

        return head
