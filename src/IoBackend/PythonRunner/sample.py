#!/usr/bin/python3
# -*- coding: utf-8 -*-

#connector came over Backend
import connector 
import sys

print("Version from Backend", connector.ver())
print(sys.path)
print ("The arguments are: " , str(sys.argv))

def multiply(a,b):
    print("Will compute", a, "times", b)
    c = 0
    for i in range(0, a):
        c = c + b
    return c

def simpleFunc():
    print("simpleFunc called")
    print("Version from Backend", connector.ver())
    return

print("finally")