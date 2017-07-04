CC := sdcc
LD := sdld
CFLAGS := -mstm8 --std-c99
TESTCC := gcc
TESTLD := gcc

DRIVERS := uart_driver.rel solenoid_driver.rel circular_buffer.rel pl9823_driver.rel led_driver.rel controller.rel configuration.rel status_driver.rel

all: board_bring_up.ihx

%.rel : %.c
	$(CC) $(CFLAGS) -c $<

board_bring_up.ihx: board_bring_up.rel $(DRIVERS)
	$(CC) $(CFLAGS) board_bring_up.rel $(DRIVERS) --out-fmt-ihx -o board_bring_up.ihx

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

unitTest: circular_buffer.o circular_buffer_test.o led_driver.o led_driver_test.o pl9823_driver_stub.o solenoid_driver_stub.o controller.o controller_test.o uart_driver_stub.o configuration_stub.o
	$(TESTLD) -g -o circular_buffer_test circular_buffer.o circular_buffer_test.o
	./circular_buffer_test
	$(TESTLD) -g -o led_driver_test led_driver.o led_driver_test.o pl9823_driver_stub.o
	./led_driver_test
	$(TESTLD) -g -o controller_test controller_test.o controller.o led_driver.o pl9823_driver_stub.o solenoid_driver_stub.o circular_buffer.o uart_driver_stub.o configuration_stub.o status_driver_stub.o
	./controller_test
