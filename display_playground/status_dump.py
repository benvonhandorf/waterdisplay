import os, sys
import math
import serial
import time
import getopt, sys


SERIAL = "/dev/tty.usbserial-A1043345"
#SERIAL = "/dev/ttyAMA0"

class PyManMain:
  """The Main PyMan Class – This class handles the main
  initialization and creating of the Game."""

  def __init__(self):
    try:
      SERIAL
      self.serial = serial.Serial(SERIAL, timeout=2)
    except NameError:
      self.serial = None

  def commandFor(address, command, data):
    result = bytearray()

    result.append(0x80 | address)

    result.extend(command)
    result.extend(data)

    return result

  def dumpStatus(self, currentAddress):
    command = PyManMain.commandFor(currentAddress, 'D'.encode(), [])
    self.serial.write(command)

  def readSerial(self):
    if self.serial is not None:
      serialData = self.serial.read(100)
      if serialData is not None and len(serialData) > 0:
        print("RX:" + str(serialData))

if __name__ == "__main__":
  try:
    main = PyManMain()
    for addr in range(1, 15):
      main.dumpStatus(addr)
      main.readSerial()

    
    
  except getopt.GetoptError as err:
    print(err)
    usage()
    sys.exit(2)
