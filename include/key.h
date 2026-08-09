#pragma once

#include <stdint-uintn.h>
#include <linux/input-event-codes.h>

struct Key {
  uint16_t code = -1;
  const char *label;
  const uint8_t width = 1;
  const bool toggle = false;
};