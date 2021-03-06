import os, sys
import math
import serial
import time
from colors import COLORS
from simple_display_program import SimpleDisplayProgram
from image_parser_program import ImageParserProgram
from time import sleep


SERIAL = "/dev/tty.usbserial-A1043345"
#SERIAL = "/dev/ttyAMA0"

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



class PyManMain:
  """The Main PyMan Class – This class handles the main
  initialization and creating of the Game."""

  def __init__(self):
    self.lastTime = time.clock()*1000

    self.program = SimpleDisplayProgram(nodes=[1])

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
        print("RX:" + str(serialData))

  def readAck(self, command):
    if self.serial is not None:
      commandByte = chr(command[1])
      ackString = "ACK-{}".format(commandByte)
      serialData = bytearray()
      displayedLength = 0
      serialData.extend(self.serial.read(100))
      serialString = str(serialData)
      while ackString not in serialString:
        if serialString and len(serialData) > displayedLength:
          print("Partial RX:" + serialString + " looking for {}".format(ackString))
          displayedLength = len(serialData)
        serialData.extend(self.serial.read(100))
        serialString = str(serialData)
        sleep(0.1)

      if serialString is not None and len(serialString) > 0:
        print("RX:" + serialString)


  def MainLoop(self):
    while 1:
      newTime = time.clock() * 1000
      delta = newTime - self.lastTime
      self.lastTime = newTime

      commands = self.program.update(delta)

      appendedCommands = bytearray()

      for command in commands:
        appendedCommands.extend(command)
        print("TX:" + str(command))
        if self.serial is not None:
          self.serial.write(command)
          self.readAck(command)


      # if len(appendedCommands) > 0:
      #   print("TX:" + str(appendedCommands))
      #   if self.serial is not None:
      #     self.serial.write(appendedCommands)
      #     self.readAck(command)

      self.readSerial()

      # for node in self.nodes:
      #   node.update(commands, delta)

if __name__ == "__main__":
  MainWindow = PyManMain()
  MainWindow.MainLoop()
