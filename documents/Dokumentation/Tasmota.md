# Tasmota

## Tasmota running on Debian

```bash
export QT_QPA_PLATFORM=wayland
python3 $HOME/.local/bin/tasmotizer.py
```

## ESP Tools

```bash
pip3 install esptool
esptool.py -p /dev/ttyUSB0 read_mac
esptool.py --port /dev/ttyUSB0 read_flash 0x00000 0x100000 relaisFWbackup.bin
```

Auf passen wenn das esptool nicht die selbe Version ist wie die im tasmotizer muss man zwischen dem Wechselt der Applikationen
einen Rest vom Chip machen sonst hängt sich was auf.

## Tasmota Config 4ch Relais

```text
Rule1
 on System#Boot do Backlog Baudrate 115200; SerialSend5 0 endon
 on Power1#State=1 do SerialSend5 A00101A2 endon
 on Power1#State=0 do SerialSend5 A00100A1 endon
 on Power2#State=1 do SerialSend5 A00201A3 endon
 on Power2#State=0 do SerialSend5 A00200A2 endon
 on Power3#State=1 do SerialSend5 A00301A4 endon
 on Power3#State=0 do SerialSend5 A00300A3 endon
 on Power4#State=1 do SerialSend5 A00401A5 endon
 on Power4#State=0 do SerialSend5 A00400A4 endon
```

```text
 {"NAME":"4CH Relay","GPIO":[1,1,1,1,1,1,1,1,1,1,1,1,1,1],"FLAG":0,"BASE":18}
 {"NAME":"4CH Relay","GPIO":[52,255,17,255,255,255,255,255,21,22,23,24,255],"FLAG":0,"BASE":18}
```

Rule1 1 zum Aktivieren der Rules

## Tasmota and Mqtt

tasmota/discovery/\<mac\>/config hier ist ein Json zur Konfig

tasmota/discovery/\<mac\>/sensory hier ist ein Json zur Sensoren drin
