
import platform

class Settings:
    if platform.system() == "Darwin":
        SERIAL_DEVICE = "/dev/tty.usbserial-0001"
    else:
        SERIAL_DEVICE = "/dev/ttyUSB0"

    BAUD_RATE = 38400
