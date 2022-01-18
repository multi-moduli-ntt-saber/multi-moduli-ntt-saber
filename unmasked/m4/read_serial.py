#!/usr/bin/env python3
import platform
import serial
import sys
from config import Settings

dev = serial.Serial(Settings.SERIAL_DEVICE, Settings.BAUD_RATE)

print("> Returned data:", file=sys.stderr)

while True:
    x = dev.read()
    sys.stdout.buffer.write(x)
    sys.stdout.flush()