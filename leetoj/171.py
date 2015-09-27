# Excel Sheet Column Number
# https://leetcode.com/problemset/algorithms/

class Solution(object):
    def titleToNumber(self, s):
        """
        :type s: str
        :rtype: int
        """
        front_char_ord = ord('A') -1
        num = 0
        for ch in s:
            num *= 26
            num += ord(ch) - front_char_ord
        return num
