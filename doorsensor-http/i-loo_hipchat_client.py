#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
import sys
import re
import time
import requests
from anybar import AnyBar

# The batteryLimit defines the point at which the battery is considered empty.
BATTERY_LIMIT = 3300

url = sys.argv[1]; # http://example.com/iloo

while True:
    try:
        r = requests.get(url)
        json = r.json()
        # {"status":"0","battery":"4000"}
        if "status" in json:
            if json["status"] == "0":
                AnyBar().change("red")
            else:
                AnyBar().change("green")
        if "battery" in json and int(json["battery"]) < BATTERY_LIMIT:
            AnyBar().change("yellow")

    except Exception:
        pass

    time.sleep(10)