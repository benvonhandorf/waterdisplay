
#include <stdint.h>
#include "stm8s.h"
#include "stm_util.h"
#include "configuration.h"

uint8_t *config_base_address = 0x4000;

#define FLASH_WRITABLE 0x08

uint8_t configuration_set(uint8_t config_location, uint8_t config_value) {
  if ((FLASH_IAPSR & FLASH_WRITABLE) == 0) {
    FLASH_DUKR = 0xae;
    FLASH_DUKR = 0x56;
  }

  *(config_base_address + config_location) = config_value;

  CLRBIT(FLASH_IAPSR, FLASH_WRITABLE);

  return configuration_get(config_location);
}

uint8_t configuration_get(uint8_t config_location) {
  uint8_t value = *(config_base_address + config_location);

  return value;
}