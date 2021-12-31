# The Api

## General

All http Call need an Api Key then Key is definded in the Json Konfig File.
The http Header is X-API-KEY

## Config File

```Json
{
    "ApiKey": "12345678",
    "EventRoot": [
        {
            "Function": "CallPython",
            "Name": "MqttValue",
            "Parameters": {}
        }
    ],
    "MqttServer": "192.168.2.50",
    "Resources": [
        {
            "Address": 32,
            "EnablePin": 6,
            "VarBaseName": "RelaisBoard",
            "OutputMap": "1111111100000000",
            "Type": "MCP23017",
            "UseEnable": true
        },
        {
            "ComPort": "None",
            "Type": "GPSMouse"
        }
    ],
    "ServerPort": 8080,
    "WatchTopics": [
        "devices/#",
        "SimpleIo/#"
    ]
}
```
