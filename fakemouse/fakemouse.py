#!/usr/bin/python3

import dbus
import sys

usage = """
Usage: ./fakemouse.py mouse_device

    mouse_device    Your mouse's name, e.g. 002 004
"""

def main():
    if (len(sys.argv) != 2):
        print(usage)
    sys_bus = dbus.SystemBus()
    mouse = sys_bus.get_object(sys.argv[0])

if __name__=="__main__":
    main()
