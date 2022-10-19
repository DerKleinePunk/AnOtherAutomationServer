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
    connector.LogEntry("DEBUG", "callbackMqtt with " + text)
    params = json.loads(text)
    if(params["topic"] == "SimpleIo/Io/RelaisBoard"):
        connector.LogEntry("DEBUG", params["value"])
        connector.WriteMqtt("herbiOs/lights/Licht1/set", params["value"])
    if(params["topic"] == "herbiOs/lights/Licht1/set"):
        connector.LogEntry("DEBUG", params["value"])
        value = json.loads(params["value"]) 
        if(value["state"] == "ON"):
            connector.ChangeValue("RelaisBoard0", "on")
        if(value["state"] == "OFF"):
            connector.ChangeValue("RelaisBoard0", "off")
    if(params["topic"] == "SimpleIo/Variable/hofbel"):
        connector.LogEntry("DEBUG", params["value"])
        connector.ChangeValue("hofbel", params["value"])
    if(params["topic"] == "SimpleIo/Variable/notify"):
        connector.LogEntry("DEBUG", params["value"])
        if(params["value"] == "ON"):
            connector.Action("Notification", params["value"])
            connector.WriteMqtt("SimpleIo/Variable/notify", "OFF")
    return

def callbackValueChanged(text):
    print("callbackValueChanged called " + text)
    connector.LogEntry("DEBUG", "callbackValueChanged")
    params = json.loads(text)
    if(params["name"] == "RelaisBoard0"):
        connector.LogEntry("DEBUG", params["value"])
        if(params["value"] == "off" ):
            connector.WriteMqtt("herbiOs/lights/Licht1/state", "{ \"state\" : \"OFF\" , \"brightness\" : 0 }" )
        if(params["value"] == "on"):
            connector.WriteMqtt("herbiOs/lights/Licht1/state", "{ \"state\" : \"ON\" , \"brightness\" :255 }" )
    if(params["name"] == "SETHOFLIGHT"):
        connector.LogEntry("DEBUG", params["value"])
        if(params["value"] == "ON" ):
            connector.ChangeValue("HOFLIGHT", "ON")
        if(params["value"] == "OFF" ):
            connector.ChangeValue("HOFLIGHT", "OFF")
    return


print("finally")