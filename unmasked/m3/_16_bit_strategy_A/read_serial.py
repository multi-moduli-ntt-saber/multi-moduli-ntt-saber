#!/usr/bin/env python3
import platform
import serial
import sys

if platform.system() == "Darwin":
    dev = serial.Serial("/dev/tty.usbmodem12303", 9600)
else:
    dev = serial.Serial("/dev/ttyACM0", 9600)

print("> Returned data:", file=sys.stderr)

while True:
    x = dev.read()
    sys.stdout.buffer.write(x)
    sys.stdout.flush()

