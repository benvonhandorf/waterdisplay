import os, sys
import pygame
import math
import serial

from pygame.locals import *

if not pygame.font: print("Warning, fonts disabled")
if not pygame.mixer: print("Warning, sound disabled")

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

	def offCommand(self, command):
		self.spraying = False
		self.targetLedColor = None
		self.ledColor = COLORS.BLACK

	def ledCommand(self, command):
		# command[2] = number of LEDs being addressed.  Assume 1
		self.ledColor = COLORS.tupleFromList(command[-4:])

	def fadeLedCommand(self, command):
		# command[3] = number of LEDs being addressed.  Assume 1
		self.fadeDurationMs = command[2] * 50
		self.fadeDurationMsComplete = 0
		self.targetLedColor = COLORS.tupleFromList(command[-4:])
		self.sourceLedColor = self.ledColor

	def fadeLed(self, msDelta):
		if self.targetLedColor is None:
			return

		self.fadeDurationMsComplete = self.fadeDurationMsComplete + msDelta
		fraction = min(1.0, self.fadeDurationMsComplete / self.fadeDurationMs)

		if fraction >= 1.0:
			self.ledColor = self.targetLedColor
			self.targetLedColor = None
			self.sourceLedColor = None
			return

		self.ledColor = (COLORS.fadeElement(self.sourceLedColor[0], self.targetLedColor[0], fraction),
			COLORS.fadeElement(self.sourceLedColor[1], self.targetLedColor[1], fraction),
			COLORS.fadeElement(self.sourceLedColor[2], self.targetLedColor[2], fraction),
			COLORS.fadeElement(self.sourceLedColor[3], self.targetLedColor[3], fraction))


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

		self.fadeLed(msDelta)

		pygame.draw.circle(self.screen, self.ledColor, self.location, self.RADIUS, 0)
		pygame.draw.circle(self.screen, COLORS.BLACK, self.location, self.RADIUS, 2)
		targetLocation = (self.location[0] + math.cos(math.radians(self.angle)) * self.RADIUS, 
			self.location[1] + math.sin(math.radians(self.angle)) * self.RADIUS)
		pygame.draw.line(self.screen, COLORS.BLACK, self.location, targetLocation, 2)

		if self.spraying:
			targetLocation = (self.location[0] + math.cos(math.radians(self.angle)) * self.SPRAYRADIUS, 
			self.location[1] + math.sin(math.radians(self.angle)) * self.SPRAYRADIUS)
			pygame.draw.line(self.screen, self.ledColor, self.location, targetLocation, 4)

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
			self.timeToNextCommand = 1000
			result.append(DisplayProgram.commandFor(self.addressFor(self.nodeSpraying, 0), 'F'.encode(), []))
			self.nodeSpraying = self.nodeSpraying + 1
			if self.nodeSpraying >= self.nodeCount:
				self.nodeSpraying = 0

			for offset in range(0, 2) :
				address = self.addressFor(self.nodeSpraying, offset)
				result.append(DisplayProgram.commandFor(address, 'l'.encode(), [0x10] + [0x01] + COLORS.listFromTuple(self.colorSet[self.colorPosition])))
				result.append(DisplayProgram.commandFor(address, 'S'.encode(), [0x01]))
				self.colorPosition = (self.colorPosition + 1) % len(self.colorSet)

		return result


class PyManMain:
	"""The Main PyMan Class – This class handles the main
	initialization and creating of the Game."""

	def __init__(self, width=640,height=480):
		"""Initialize"""
		"""Initialize PyGame"""
		pygame.init()
		"""Set the window Size"""
		self.width = width
		self.height = height
		"""Create the Screen"""
		self.screen = pygame.display.set_mode((self.width
		, self.height))

		self.clock = pygame.time.Clock()

		self.program = DisplayProgram()

		self.nodes = [SprayerNode(0x01, self.screen, (320+100, 75), 180-45),
			SprayerNode(0x02, self.screen, (320, 425), 270),
			SprayerNode(0x03, self.screen, (320-100, 75), 45),			
			]

		pygame.display.set_caption('Display Development')

		self.clock.tick()

	def drawPond(self):
		pygame.draw.ellipse(self.screen, COLORS.BLUE, (200, 75, 250, 200), 0)
		pygame.draw.ellipse(self.screen, COLORS.BLUE, (200, 200, 250, 200), 0)


	def MainLoop(self):
		"""This is the Main Loop of the Game"""
		while 1:
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					sys.exit()

			self.clock.tick()

			self.screen.fill(COLORS.WHITE)

			self.drawPond()

			commands = self.program.update(self.clock.get_time())

			for command in commands:
				print(command)

			for node in self.nodes:
				node.update(commands, self.clock.get_time())

			pygame.display.update()


if __name__ == "__main__":
	MainWindow = PyManMain()
	MainWindow.MainLoop()
