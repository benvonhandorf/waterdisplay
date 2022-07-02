
#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "configuration.h"


uint8_t configuration_set(uint8_t config_location, uint8_t config_value) {
  return configuration_get(config_location);
}

uint8_t configuration_get(uint8_t config_location) {
  return 2;
}