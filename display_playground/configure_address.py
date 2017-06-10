import os, sys
import math
import serial
import time
import getopt, sys


# SERIAL = "/dev/tty.usbserial-00001014"
SERIAL = "/dev/ttyAMA0"

class PyManMain:
  """The Main PyMan Class – This class handles the main
  initialization and creating of the Game."""

  def __init__(self):
    try:
      SERIAL
      self.serial = serial.Serial(SERIAL, timeout=0)
    except NameError:
      self.serial = None

  def commandFor(address, command, data):
    result = bytearray()

    result.append(0x80 | address)

    result.extend(command)
    result.extend(data)

    return result

  def setAddress(self, currentAddress, newAddress):
    command = PyManMain.commandFor(currentAddress, 'A'.encode(), [newAddress])
    self.serial.write(command)

def usage():
  print("Usage: -a X -n Y or --address=X --newaddress=Y - Sets the node with addrexx X to have  address to Y")

if __name__ == "__main__":
  try:
    opts, args = getopt.getopt(sys.argv[1:], "a:n:", ["address=", "newaddress="])
    currentAddress = 1
    newAddress = 0
    for o, v in opts:
      if o in ("-a", "--address"):
        currentAddress = int(v)
      elif o in ("-n", "--newaddress"):
        newAddress = int(v)
    if newAddress == 0:
      usage()
      sys.exit(3)
    print("Setting node {} to be address {}".format(currentAddress, newAddress))
    print("Press enter to confirm")
    input()
    main = PyManMain()
    main.setAddress(currentAddress, newAddress)
    
  except getopt.GetoptError as err:
    print(err)
    usage()
    sys.exit(2)
