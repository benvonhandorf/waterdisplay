from display_program import DisplayProgram
from colors import COLORS
from skimage import data
from skimage import io
import random
import numpy

class ImageParserProgram( DisplayProgram ):
  def __init__(self, sourceImage = "source_image.jpg", nodes = [1]):
    self.image = io.imread(sourceImage)[0]
    self.imageSize = self.image.shape
    self.timeToNextCommand = 0
    self.sprayerThreshold = numpy.average(self.image) * 0.85
    random.seed()
    self.nodes = nodes
    self.positions = {}

    for node in self.nodes:
      self.positions[node] = (random.randint(0, self.imageSize[0] - 1), random.randint(1, self.imageSize[0] - 1))

  def movePosition(self, position):
    xDelta = random.choice(range(-5, 5))
    yDelta = random.choice(range(-5, 5))

    position = (position[0] + xDelta, position[1] + yDelta)

    if position[0] < 0:
      position = (self.imageSize[0] - 1, position[1])

    if position[0] >= self.imageSize[0]:
      position = (0, position[1])

    if position[1] < 0:
      position = (position[0], self.imageSize[1] - 1)

    if position[1] >= self.imageSize[1]:
      position = (position[0], 0)

    return position

  def update(self, msDelta):
    result = []

    self.timeToNextCommand = self.timeToNextCommand - msDelta

    if(self.timeToNextCommand <= 0):
      self.timeToNextCommand = 1000

      for node in self.nodes:
        position = self.positions[node]
        position = self.movePosition(position)
        self.positions[node] = position

        imageVal = self.image[int(position[0])][int(position[1])]
        imageVal = list(map( lambda x: x >> 1, imageVal.tolist()))

        result.append(DisplayProgram.commandFor(node, 'L'.encode(),  [0x01] + imageVal + [0x00]))
        if numpy.average(imageVal) > self.sprayerThreshold:
          result.append(DisplayProgram.commandFor(node, 'S'.encode(), [0x01]))
        else :
          result.append(DisplayProgram.commandFor(node, 'S'.encode(), [0x00]))

    return result
