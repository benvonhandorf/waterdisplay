import os, sys
import math
import serial


# SERIAL = "/dev/tty.usbserial-00001014"
SERIAL = "/dev/ttyAMA0"

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
		if(self.spraying) {
			print(self.address + " - Sprayer On")
		}

	def offCommand(self, command):
		print(self.address + " - OFF")

	def ledCommand(self, command):
		# command[2] = number of LEDs being addressed.  Assume 1
		self.ledColor = COLORS.tupleFromList(command[-4:])

		print(self.address + " - LED Color : " + self.ledColor)

	def fadeLedCommand(self, command):
		# command[3] = number of LEDs being addressed.  Assume 1
		self.fadeDurationMs = command[2] * 50
		self.fadeDurationMsComplete = 0
		self.targetLedColor = COLORS.tupleFromList(command[-4:])
		self.sourceLedColor = self.ledColor

		print(self.address + " - Fade to : " + self.ledColor + " over " + self.fadeDurationMs + "ms")

	def __init__(self, address, screen, location, angle):
		self.address = address
		self.screen = screen
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
		self.nodeCount = 1
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
			self.timeToNextCommand = 10000

			self.nodeSpraying = self.nodeSpraying + 1
			if self.nodeSpraying >= self.nodeCount:
				self.nodeSpraying = 0

			for offset in range(0, 2) :
				address = self.addressFor(self.nodeSpraying, offset)
				result.append(DisplayProgram.commandFor(address, 'l'.encode(), [0x7F] + [0x01] + COLORS.listFromTuple(self.colorSet[self.colorPosition])))
				result.append(DisplayProgram.commandFor(address, 'S'.encode(), [0x01]))
				self.colorPosition = (self.colorPosition + 1) % len(self.colorSet)

		return result

class PyManMain:
	"""The Main PyMan Class – This class handles the main
	initialization and creating of the Game."""

	def __init__(self):
		self.clock = pygame.time.Clock()

		self.program = DisplayProgram()

		self.nodes = [SprayerNode(0x01, self.screen, (320+100, 75), 180-45)]

		try:
		  SERIAL
		  self.serial = serial.Serial(SERIAL, timeout=0)
		except NameError:
		  self.serial = None

		self.clock.tick()

	def readSerial(self):
		if self.serial is not None:
			serialData = self.serial.read(100)
			print("Serial:" + str(serialData))


	def MainLoop(self):
		while 1:
			self.clock.tick()

			commands = self.program.update(self.clock.get_time())

			for command in commands:
				print(command)
				if self.serial is not None:
					self.serial.write(command)

			self.readSerial()

			for node in self.nodes:
				node.update(commands, self.clock.get_time())

if __name__ == "__main__":
	MainWindow = PyManMain()
	MainWindow.MainLoop()