#!/usr/bin/python3
# -*- coding: utf-8 -*-

#connector came over Backend
import connector 
import sys
import json

print("Version from Backend", connector.Ver())
print(sys.path)
print ("The arguments are: " , str(sys.argv))

connector.LogEntry("DEBUG", "Script Startet")

def multiply(a,b):
    print("Will compute", a, "times", b)
    c = 0
    for i in range(0, a):
        c = c + b
    return c

def simpleFunc():
    print("simpleFunc called")
    print("Version from Backend", connector.Ver())
    connector.LogEntry("DEBUG", "simpleFunc Startet")
    return

def callbackMqtt(text):
    print("callbackMqtt called " + text)
    connector.LogEntry("DEBUG", "callbackMqtt")
    params = json.loads(text)
    if(params["topic"] == "SimpleIo/Io/RelaisBoard"):
        connector.LogEntry("DEBUG", params["value"])
        connector.WriteMqtt("herbiOs/lights/Licht1/set", params["value"])
    if(params["topic"] == "herbiOs/lights/Licht1/set"):
        connector.LogEntry("DEBUG", params["value"])
        connector.ChangeValue("RelaisBoard0", params["value"])
    return

def callbackValueChanged(text):
    print("callbackValueChanged called " + text)
    connector.LogEntry("DEBUG", "callbackValueChanged")
    params = json.loads(text)
    if(params["name"] == "RelaisBoard0"):
        connector.LogEntry("DEBUG", params["value"])
        connector.WriteMqtt("herbiOs/lights/Licht1/state", params["value"])
    return

print("finally")