#! /usr/bin/env python
#encoding=utf-8

# Max Points on a Line

# Definition for a point
# class Point:
#     def __init__(self, a=0, b=0):
#         self.x = a
#         self.y = b

class Solution:
    # @param points, a list of Points
    # @return an integer
    def maxPoints(self, points):
        pass
# LOGIC : sort the points based on x value. 
# Now, calculate the slopes with points after the current point.
# Keep on adding to points with same slope. 
# Once done with current points, we'll have a set of slopes m
# with the number of points with those slopes. Any possible slope
# having current point is already in the set of slopes.
# If we take another point say j after i, then we have already 
# calculated points with slope that is between j and i.
# thus we just need to look after i. 
