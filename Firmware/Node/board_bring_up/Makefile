CC := sdcc
LD := sdld
CFLAGS := -mstm8 --std-c99
TESTCC := gcc
TESTLD := gcc

DRIVERS := 

all: board_bring_up.ihx

%.rel : %.c
	$(CC) $(CFLAGS) -c $<

board_bring_up.ihx: board_bring_up.rel $(DRIVERS)
	$(CC) $(CFLAGS) board_bring_up.rel $(DRIVERS) --out-fmt-ihx -o board_bring_up.ihx

flash_opt:
	sudo stm8flash -cstlinkv2 -pstm8s103f3 -s 0x4803 -b 2 -w pin15_option_bit

flash: board_bring_up.ihx
	sudo stm8flash -cstlinkv2 -pstm8s103f3 -w board_bring_up.ihx

pwm_test.ihx: pwm_test.rel
	$(CC) $(CFLAGS) pwm_test.rel --out-fmt-ihx -o pwm_test.ihx

flash_pwm: pwm_test.ihx
	sudo stm8flash -cstlinkv2 -pstm8s103f3 -w pwm_test.ihx

clean:
	rm -f *.asm *.ihx *.cdb *.lst *.map *.lk *.rel *.rst *.sym *.o

%.o: %.c
	$(TESTCC) -c -DDEBUG=1 -g $<
