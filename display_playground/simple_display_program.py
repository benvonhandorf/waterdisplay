from display_program import DisplayProgram
from colors import COLORS

class SimpleDisplayProgram( DisplayProgram ):
  def __init__(self):
    self.timeToNextCommand = 0
    self.nodeCount = 3
    self.nodeSpraying = 0
    self.colorSet = [COLORS.RED, COLORS.GREEN, COLORS.BLUE, COLORS.PURPLE, COLORS.YELLOW, COLORS.BLUEGREEN, COLORS.ORANGE]
    self.colorPosition = 0

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