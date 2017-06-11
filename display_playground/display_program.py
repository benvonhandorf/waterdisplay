import math
from colors import COLORS

class DisplayProgram:

  def commandFor(address, command, data):
    result = bytearray()

    result.append(0x80 | address)

    result.extend(command)
    result.extend(data)

    return result

  def addressFor(self, startingNode, offset):
    return (( startingNode + offset ) % self.nodeCount ) + 1

  def update(self, msDelta):
    pass
    