#!/bin/sh

RCol=$(tput sgr 0);    # Text Reset
BGre=$(tput setaf 2);
BYel=$(tput setaf 3);

PIP_URL=https://bootstrap.pypa.io/get-pip.py

# cleanup from previous installation attempts
rm -f AnyBar-0.1.4.zip
rm -f AnyBar.app

echo "Checking installation ..."

# check for Python
PYTHON=$(which python)
if [ "$PYTHON" == "" ]; then
    echo "python ... ${BYel}not found${RCol} - please download and install python"
    exit 0;
fi
echo "- python ... ${BGre}installed${RCol}"

# check for brew
BREW=$(which brew)
if [ "$BREW" == "" ]; then
    echo "- brew ... ${BYel}not found${RCol} - please download and install brew"
    exit 0;
fi
echo "- brew ... ${BGre}installed${RCol}"

# check for pip
PIP=$(which pip)
if [ "$PIP" == "" ]; then
    echo "- pip ... ${BYel}not found${RCol} - please download and install pip"
    exit 0;
fi
echo "- pip ... ${BGre}installed${RCol}"

echo "Installing paho-mqtt ..."
#pip install paho-mqtt

echo "Installing tonsky/AnyBar"
curl -O https://github.com/tonsky/AnyBar/releases/download/0.1.4/AnyBar-0.1.4.zip
unzip AnyBar-0.1.4.zip
#mv AnyBar.app /Applications/.

echo "Installing philipbl/pyAnyBar"
#pip install pyanybar

#- Install [PIP](https://bootstrap.pypa.io/get-pip.py): `python get-pip.py`
#- Install [Paho MQTT-library]([https://www.eclipse.org/paho/]): `pip install paho-mqtt`
#- Install [Cask](http://caskroom.io/): `brew tap caskroom/cask`
#- Install [tonsky/AnyBar](https://github.com/tonsky/AnyBar): `brew cask install anybar`
#- Install [philipbl/pyAnyBar](https://github.com/philipbl/pyAnyBar): `pip install pyanybar`
