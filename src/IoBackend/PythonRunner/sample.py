#!/usr/bin/python3
# -*- coding: utf-8 -*-

import connector 

print("Version from Backend", connector.ver())

def multiply(a,b):
    print("Will compute", a, "times", b)
    c = 0
    for i in range(0, a):
        c = c + b
    return c

print("finally")