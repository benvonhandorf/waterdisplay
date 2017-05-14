import os, sys
import pygame
import math
from pygame.locals import *

if not pygame.font: print("Warning, fonts disabled")
if not pygame.mixer: print("Warning, sound disabled")

class COLORS:
	BLACK = (  0,   0,   0)
	WHITE = (255, 255, 255)
	RED   = (255,   0,   0)
	PURPLE   = (255,   0,   255)
	GREEN = (  0, 255,   0)
	BLUE  = (  0,   0, 255)

	def listFromTuple(tuple):
		return [tuple[0], tuple[1], tuple[2]]

	def tupleFromList(list):
		return (list[0], list[1], list[2])

class SprayerNode:
	RADIUS = 20
	SPRAYRADIUS = 150

	def solenoidCommand(self, command):
		self.spraying = command[2] > 0

	def ledCommand(self, command):
		# command[2] = number of LEDs being addressed.  Assume 1
		self.ledColor = COLORS.tupleFromList(command[-3:])


	def __init__(self, address, screen, location, angle):
		self.address = address
		self.screen = screen
		self.location = location
		self.angle = angle
		self.spraying = False
		self.ledColor = COLORS.BLUE

		self.commands = {
			'S': self.solenoidCommand,
			'L': self.ledCommand
		}

	def processCommand(self, command):
		if command[0] & 0x0F == self.address:
			commandByte = command[1]
			commandProcessor = self.commands[commandByte]

			commandProcessor(command)

	def update(self, commands):
		for command in commands:
			self.processCommand(command)

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
		self.colorSet = [COLORS.RED, COLORS.GREEN, COLORS.BLUE, COLORS.PURPLE]
		self.colorPosition = 0

	def commandFor(address, command, data):
		result = [0x80 | address]

		result.append(command)
		for byte in data:
			result.append(byte)

		return result

	def update(self, msDelta):
		result = []

		self.timeToNextCommand = self.timeToNextCommand - msDelta

		if(self.timeToNextCommand <= 0):
			self.timeToNextCommand = 1000
			result.append(DisplayProgram.commandFor(self.nodeSpraying, 'S', [0x00]))
			self.nodeSpraying = self.nodeSpraying + 1
			if self.nodeSpraying > self.nodeCount:
				self.nodeSpraying = 1

			result.append(DisplayProgram.commandFor(self.nodeSpraying, 'L', [0x01] + COLORS.listFromTuple(self.colorSet[self.colorPosition])))
			result.append(DisplayProgram.commandFor(self.nodeSpraying, 'S', [0x01]))
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

		self.nodes = [SprayerNode(1, self.screen, (320+100, 240-100), 180-45),
			SprayerNode(2, self.screen, (320, 240+100), 270),
			SprayerNode(3, self.screen, (320-100, 240-100), 45),			
			]

		pygame.display.set_caption('Display Development')

		self.clock.tick()


	def MainLoop(self):
		"""This is the Main Loop of the Game"""
		while 1:
			for event in pygame.event.get():
				if event.type == pygame.QUIT:
					sys.exit()

			self.clock.tick()

			self.screen.fill(COLORS.WHITE)

			commands = self.program.update(self.clock.get_time())

			for node in self.nodes:
				node.update(commands)

			pygame.display.update()


if __name__ == "__main__":
	MainWindow = PyManMain()
	MainWindow.MainLoop()