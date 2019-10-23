ESP32 transmits status of a doorsensor to a MQTT broker. MQTT client on MacOS subscribes to the specific status topic and sets an icon in the menu bar.

# ESP32
ESP32 with a sensor. Status is transmitted to a MQTT broker whenever the door is locked or unlocked. Code taken from https://github.com/merlinschumacher/Basecamp/blob/master/examples/doorsensor/doorsensor.ino).

# MacOS
## Installation
- Background tasks in OS X: [Creating Launch Daemons and Agents](https://developer.apple.com/library/content/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingLaunchdJobs.html#//apple_ref/doc/uid/10000172i-SW7-BCIEDDBJ)
- Example [launchd.info](http://www.launchd.info/)
- launchd plist example files: /System/Library/LaunchDaemons
- [Distributing your application](https://developer.apple.com/library/content/documentation/Porting/Conceptual/PortingUnix/distributing/distibuting.html)

## MQTT Client - Python
Simple MQTT client connecting to a MQTT broker awaiting status messages and changing an icon in the menu bar.

## Notification in the menu bar (red dot)
- [tonsky/AnyBar](https://github.com/tonsky/AnyBar)
- [philipbl/pyAnyBar](https://github.com/philipbl/pyAnyBar) (Python client)

# Installation - MacOS
- Download and install [Python 2.7](https://www.python.org/downloads/)
- Install [PIP](https://bootstrap.pypa.io/get-pip.py): `python get-pip.py`
- Install [Paho MQTT-library]([https://www.eclipse.org/paho/]): `pip install paho-mqtt`
- Install [tonsky/AnyBar](https://github.com/tonsky/AnyBar/releases/download/0.1.4/AnyBar-0.1.4.zip): `AnyBar-0.1.4.zip`
  - Start AnyBar during startup (System Preferences - Accounts or the User & Groups)

  ![AnyBar][logo]
  [logo]: https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "AnyBar"
- Install [philipbl/pyAnyBar](https://github.com/philipbl/pyAnyBar): `sudo pip install pyanybar`
- Install python module requests: `sudo pip install requests`

sudo pip uninstall pip
sudo python -m ensurepip



launchctl load ~/Library/LaunchAgents/ch.i-loo.status.plist

# Installation
- https://github.com/tonsky/AnyBar/releases/download/0.1.4/AnyBar-0.1.4.zip
- sudo pip install pyanybar
- sudo pip install requests

https://stackoverflow.com/questions/29338066/mac-osx-execute-a-python-script-at-startup
