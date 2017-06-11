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