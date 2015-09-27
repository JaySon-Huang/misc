# Lowest Common Ancestor of a Binary Search Tree
# https://leetcode.com/problems/lowest-common-ancestor-of-a-binary-search-tree

# Definition for a binary tree node.
# class TreeNode(object):
#     def __init__(self, x):
#         self.val = x
#         self.left = None
#         self.right = None

class Solution(object):
    def lowestCommonAncestor(self, root, p, q):
        """
        :type root: TreeNode
        :type p: TreeNode
        :type q: TreeNode
        :rtype: TreeNode
        """
        if root is None:
            return None

        if root.val > p.val and root.val > q.val:
            return self.lowestCommonAncestor(
                root.left, p, q
            )
        elif root.val < p.val and root.val < q.val:
            return self.lowestCommonAncestor(
                root.right, p, q
            )
        else:
            # 由于题目限定树为BST, 当根结点val在查询的两个结点val范围之内,
            # 等价于此根结点为p, q的最近公共祖先
            return root
