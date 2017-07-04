#define CLRBIT(REG, VALUE) REG &= ~VALUE
#define SETBIT(REG, VALUE) REG |= VALUE

#define wfi()                                                                  \
  { __asm__("wfi"); } /* Wait For Interrupt */

#define rmi()                                                                  \
  { __asm__("rim"); } /* Enable interrupts */

#define smi()                                                                  \
  { __asm__("sim"); }

#define noop()			\
  { __asm__("noop"); }

  