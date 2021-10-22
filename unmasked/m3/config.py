import platform

class Settings:
    if platform.system() == "Linux":
        SERIAL_DEVICE = "/dev/ttyACM0"
    elif platform.system() == "Darwin":
        SERIAL_DEVICE = "/dev/tty.usbmodem12303"
    else:
        raise Exception("OS not supported")