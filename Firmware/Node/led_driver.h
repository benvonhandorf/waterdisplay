typedef struct {
  uint8_t source[4];
  uint8_t target[4];
  uint8_t current[4];
} LED_DATA_T;

typedef struct {
  //Both time values are indicated in "steps", which are defined as the number of units to fade over.
  uint8_t fadeTotal;
  uint8_t fadeComplete;
} FADE_DATA_T;

void led_init(uint8_t led_count);

void led_fade();

void led_fade_single(LED_DATA_T * led, uint8_t fadePortion);