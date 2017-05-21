#include "stm8s.h"
#include "uart_driver.h"
#include "solenoid_driver.h"

void main() {
	int i = 0;
	int solenoid_active = 0;

	uart_init();
	solenoid_init();

	while(1) {
		uart_write("Hello\n", 6);
		solenoid_active = !solenoid_active;

		solenoid_write(solenoid_active);

		for(i = 0 ; i < 0x7FFF; i++) {
			//Sleep
		}
	}
}