#pragma once

#include <optional>
#include <string>
#include <utility>

#include <linux/input-event-codes.h>

#include "modifier.h"

struct KeyAction {
  int key_code;
  bool shift;
};

std::optional<KeyAction> map_char_to_key(char c);
void emit_key_press_with_modifiers(int fd, int key_code, bool shift, const j_osk::ModifierState &state);