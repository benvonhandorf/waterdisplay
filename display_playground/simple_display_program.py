from display_program import DisplayProgram
from colors import COLORS

class SimpleDisplayProgram( DisplayProgram ):
  def __init__(self, nodes=[1]):
    self.timeToNextCommand = 0
    self.nodes = nodes
    self.spraying = False
    self.colorSet = [COLORS.RED, COLORS.GREEN, COLORS.BLUE, COLORS.PURPLE, COLORS.YELLOW, COLORS.BLUEGREEN, COLORS.ORANGE]
    self.colorPosition = 0

  def update(self, msDelta):
    result = []

    self.timeToNextCommand = self.timeToNextCommand - msDelta

    if(self.timeToNextCommand <= 0):
      self.timeToNextCommand = 2000
      
      if self.spraying:
        for node in self.nodes :
          result.append(DisplayProgram.commandFor(node, 'l'.encode(), [0x08] + [0x01] + COLORS.listFromTuple(COLORS.BLACK)))
          result.append(DisplayProgram.commandFor(node, 'S'.encode(), [0x00]))
        self.spraying = False
      else:
        for node in self.nodes :
          result.append(DisplayProgram.commandFor(node, 'l'.encode(), [0x08] + [0x01] + COLORS.listFromTuple(self.colorSet[self.colorPosition])))
          result.append(DisplayProgram.commandFor(node, 'S'.encode(), [0x01]))
          self.colorPosition = (self.colorPosition + 1) % len(self.colorSet)

        self.spraying = True

    return result