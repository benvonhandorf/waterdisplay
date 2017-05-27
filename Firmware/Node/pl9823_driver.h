
void pl9823_init(uint8_t led_count);

void pl9823_fade();

typedef struct {
  unsigned char source[4];
  unsigned char target[4];
  unsigned char current[4];
} LED_DATA_T;

typedef struct {
  //Both time values are indicated in "steps", which are defined as the number of units to fade over.
  uint8_t fadeTotal;
  uint8_t fadeComplete;
} FADE_DATA_T;

void fadeLed(LED_DATA_T * led, uint8_t fadePortion);