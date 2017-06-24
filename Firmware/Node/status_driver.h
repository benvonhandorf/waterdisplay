

void status_init();

#define STATUS_ONE 0x01
#define STATUS_TWO 0x02

void status_set(uint8_t status_bit);
void status_clear(uint8_t status_bit);
void status_toggle(uint8_t status_bit);