import platform

class Settings:
    if platform.system() == "Darwin":
        SERIAL_DEVICE = "/dev/tty.usbmodem112103"
    else:
        SERIAL_DEVICE = "/dev/ttyACM0"

    BAUD_RATE = 9600

