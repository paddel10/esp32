ESP32 transmits status of a doorsensor to a MQTT broker. MQTT client on MacOS subscribes to the specific status topic and sets an icon in the menu bar.

# ESP32
ESP32 with a sensor. Status is transmitted to a MQTT broker whenever the door is locked or unlocked. Code taken from https://github.com/merlinschumacher/Basecamp/blob/master/examples/doorsensor/doorsensor.ino).

# MacOS
- Background tasks in OS X: [Creating Launch Daemons and Agents](https://developer.apple.com/library/content/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingLaunchdJobs.html#//apple_ref/doc/uid/10000172i-SW7-BCIEDDBJ)

## MQTT Client - Python
Simple MQTT client connecting to a MQTT broker awaiting status messages and changing an icon in the menu bar.

## Notification in the menu bar (red dot)
- [tonsky/AnyBar](https://github.com/tonsky/AnyBar)
- Python client: https://github.com/philipbl/pyAnyBar
