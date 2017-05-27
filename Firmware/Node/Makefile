CC := sdcc
LD := sdld
CFLAGS := -mstm8 --std-c99
TESTCC := gcc
TESTLD := gcc

DRIVERS := uart_driver.rel solenoid_driver.rel circular_buffer.rel pl9823_driver.rel

all: board_bring_up.ihx

%.rel : %.c
	$(CC) $(CFLAGS) -c $<

board_bring_up.ihx: board_bring_up.rel $(DRIVERS)
	$(CC) $(CFLAGS) board_bring_up.rel $(DRIVERS) --out-fmt-ihx -o board_bring_up.ihx

flash: board_bring_up.ihx
	sudo stm8flash -cstlinkv2 -pstm8s103f3 -w board_bring_up.ihx

clean:
	rm -f *.asm *.ihx *.cdb *.lst *.map *.lk *.rel *.rst *.sym *.o

%.o: %.c
	$(TESTCC) -c -DDEBUG=1 $<

unitTest: circular_buffer.o circular_buffer_test.o
	$(TESTLD) -o circular_buffer_test circular_buffer.o circular_buffer_test.o
	./circular_buffer_test
