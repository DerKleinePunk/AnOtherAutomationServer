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
Action -> Tu was Whath 2 Parameter zb Notification alle UI Spielen einen Sound

siehe GlobalTypes.hpp

## http Api Calls

### Get Wlans in der Nähe

Request

GET http://localhost:8080/api/system/wlan

reponse

```Json
[{
  "AccessPoints": [{
    "Bitrate": "270 Mbit/s",
    "Bssid": "DE:07:B6:11:28:F0",
    "Freg": "5180 MHz",
    "Mode": "Infrastructure",
    "Ssid": "--",
    "Strength": 49
  }, {
    "Bitrate": "270 Mbit/s",
    "Bssid": "D8:07:B6:11:28:F0",
    "Freg": "5180 MHz",
    "Mode": "Infrastructure",
    "Ssid": "ROBONETZ",
    "Strength": 47
  }, {
    "Bitrate": "405 Mbit/s",
    "Bssid": "60:E3:27:6F:D1:FA",
    "Freg": "2447 MHz",
    "Mode": "Infrastructure",
    "Ssid": "ROBONETZ_2",
    "Strength": 82
  }, {
    "Bitrate": "195 Mbit/s",
    "Bssid": "DE:07:B6:11:28:F1",
    "Freg": "2432 MHz",
    "Mode": "Infrastructure",
    "Ssid": "--",
    "Strength": 50
  }, {
    "Bitrate": "195 Mbit/s",
    "Bssid": "D8:07:B6:11:28:F1",
    "Freg": "2432 MHz",
    "Mode": "Infrastructure",
    "Ssid": "ROBONETZ",
    "Strength": 44
  }],
  "ActiveSsid": "none",
  "Iface": "wlx0013eff20372",
  "Speed": 0
}]
```

PUT http://localhost:8080/api/system/wlan
Body { "connectionName" : "WLANNAE", "passwort" : "5"}

