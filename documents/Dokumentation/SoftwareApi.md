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
            "Parameters": {
                "Script" : "sample",
                "Function" : "callbackMqtt"
            }
        }
    ],
    "MqttServer": "192.168.2.49",
    "Resources": [
        {
            "Address": 32,
            "EnablePin": 6,
            "OutputMap": "1111111100000000",
            "Type": "MCP23017",
            "UseEnable": true,
            "VarName": [
                "RelaisBoard0",
                "Automatik",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
            ]
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

ApiKey -> The Key to Call the Rest Api Functions
EventRoot -> Der Interne Eent Manager Im Oberene Beispiel Wenn das Event MqttValue ein Tritt dann bitt die Funktion callbackMqtt im Sample.py auf rufen.

## System Events

MqttValue -> Ein Wert im Mqtt wurde geändert Überwachte Mqtt bereiche werden im WatchTopics Konfiguriert
ChangeValue -> Ändere eine eine Interne Variable
ValueChanged -> Interne / Gobale Variable wurde geändert
ChatMessage -> Eine Text Nachricht die durchs System Läuft

siehe GlobalTypes.hpp
