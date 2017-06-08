import os, sys
import math
import serial
import time


# SERIAL = "/dev/tty.usbserial-00001014"
# SERIAL = "/dev/ttyAMA0"

class COLORS:
  BLACK = (  0,   0,   0, 0)
  WHITE = (0x7F, 0x7F, 0x7F, 0)
  RED   = (0x7F,   0,   0, 0)
  PURPLE   = (0x7F,   0,   0x7F, 0)
  GREEN = (  0, 0x7F,   0, 0)
  BLUE  = (  0,   0, 0x7F, 0)
  YELLOW = (0x7F, 0x7F, 0, 0)
  BLUEGREEN = (0x00, 0x66, 0x7f, 0)
  ORANGE = (0x7F, 0x79, 0x33, 0)

  def listFromTuple(tuple):
    return [tuple[0], tuple[1], tuple[2], tuple[3]]

  def tupleFromList(list):
    return (list[0], list[1], list[2], list[3])

  def fadeElement(sourceElement, targetElement, fraction):
    delta = targetElement - sourceElement

    return sourceElement + (fraction * delta)

class SprayerNode:
  RADIUS = 20
  SPRAYRADIUS = 150

  def solenoidCommand(self, command):
    self.spraying = command[2] > 0
    if self.spraying :
      print(str(self.address) + " - Sprayer On")
    else:
      print(str(self.address) + " - Sprayer Off")
    

  def offCommand(self, command):
    print(str(self.address) + " - OFF")

  def ledCommand(self, command):
    # command[2] = number of LEDs being addressed.  Assume 1
    self.ledColor = COLORS.tupleFromList(command[-4:])

    print(str(self.address) + " - LED Color : " + str(self.ledColor))

  def fadeLedCommand(self, command):
    # command[3] = number of LEDs being addressed.  Assume 1
    self.fadeDurationMs = command[2] * 50
    self.fadeDurationMsComplete = 0
    self.targetLedColor = COLORS.tupleFromList(command[-4:])
    self.sourceLedColor = self.ledColor

    print(str(self.address) + " - Fade to : " + str(self.targetLedColor) + " over " + str(self.fadeDurationMs) + "ms")

  def __init__(self, address, location, angle):
    self.address = address
    self.location = location
    self.angle = angle
    self.spraying = False
    self.ledColor = COLORS.BLUE
    self.targetLedColor = None

    self.commands = {
      'S': self.solenoidCommand,
      'L': self.ledCommand,
      'l': self.fadeLedCommand,
      'F': self.offCommand
    }

  def processCommand(self, command):
    if command[0] & 0x0F == self.address:
      commandByte = chr(command[1])
      commandProcessor = self.commands[commandByte]

      commandProcessor(command)

  def update(self, commands, msDelta):
    for command in commands:
      self.processCommand(command)

class DisplayProgram:
  def __init__(self):
    self.timeToNextCommand = 0
    self.nodeCount = 3
    self.nodeSpraying = 0
    self.colorSet = [COLORS.RED, COLORS.GREEN, COLORS.BLUE, COLORS.PURPLE, COLORS.YELLOW, COLORS.BLUEGREEN, COLORS.ORANGE]
    self.colorPosition = 0

  def commandFor(address, command, data):
    result = bytearray()

    result.append(0x80 | address)

    result.extend(command)
    result.extend(data)

    return result

  def addressFor(self, startingNode, offset):
    return (( startingNode + offset ) % self.nodeCount ) + 1

  def update(self, msDelta):
    result = []

    self.timeToNextCommand = self.timeToNextCommand - msDelta

    if(self.timeToNextCommand <= 0):
      self.timeToNextCommand = 5000
      
      # result.append(DisplayProgram.commandFor(self.nodeSpraying, 'F'.encode(), []))
      result.append(DisplayProgram.commandFor(self.nodeSpraying, 'l'.encode(), [0x10] + [0x01] + COLORS.listFromTuple(COLORS.BLACK)))
      result.append(DisplayProgram.commandFor(self.nodeSpraying, 'S'.encode(), [0x00]))

      self.nodeSpraying = self.nodeSpraying + 1
      if self.nodeSpraying >= self.nodeCount:
        self.nodeSpraying = 0

      for offset in range(0, 1) :
        address = self.addressFor(self.nodeSpraying, offset)
        result.append(DisplayProgram.commandFor(address, 'l'.encode(), [0x10] + [0x01] + COLORS.listFromTuple(self.colorSet[self.colorPosition])))
        result.append(DisplayProgram.commandFor(address, 'S'.encode(), [0x01]))
        self.colorPosition = (self.colorPosition + 1) % len(self.colorSet)

    return result

class PyManMain:
  """The Main PyMan Class – This class handles the main
  initialization and creating of the Game."""

  def __init__(self):
    self.lastTime = time.clock()*1000

    self.program = DisplayProgram()

    self.nodes = [SprayerNode(0x01,(320+100, 75), 180-45)]

    try:
      SERIAL
      self.serial = serial.Serial(SERIAL, timeout=0)
      self.serial.nonblocking()
    except NameError:
      self.serial = None

  def readSerial(self):
    if self.serial is not None:
      serialData = self.serial.read(100)
      if serialData is not None and len(serialData) > 0:
        print("Serial:" + str(serialData))


  def MainLoop(self):
    while 1:
      newTime = time.clock() * 1000
      delta = newTime - self.lastTime
      self.lastTime = newTime

      commands = self.program.update(delta)

      for command in commands:
        print(command)
        if self.serial is not None:
          self.serial.write(command)

      self.readSerial()

      for node in self.nodes:
        node.update(commands, delta)

if __name__ == "__main__":
  MainWindow = PyManMain()
  MainWindow.MainLoop()
