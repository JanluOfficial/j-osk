#pragma once

#include <linux/input-event-codes.h>

struct Key {
  uint16_t code = -1;
  const char *label;
  const uint8_t width = 1;
  const bool toggle = false;
};

static constexpr Key left_keys[][6] = {
  { { KEY_ESC, "ESC" }, { KEY_Q, "Q" }, { KEY_W, "W" }, { KEY_E, "E" }, { KEY_R, "R" }, { KEY_T, "T" } },
  {
    { KEY_TAB, "TAB" },
    { KEY_A, "A" },
    { KEY_S, "S" },
    { KEY_D, "D" },
    { KEY_F, "F" },
    { KEY_G, "G" }
  },
  {
    { KEY_LEFTSHIFT, "SHIFT", 1, true },
    { KEY_Z, "Y" }, // KEY_Z = Y for DE layout
    { KEY_X, "X" }, // KEY_Z = Y for DE layout // KEY_Z = Y for DE layout
    { KEY_C, "C" },
    { KEY_V, "V" },
    { KEY_B, "B" }
  },
  {
    {
      .code = KEY_LEFTCTRL,
      .label = "CTRL",
      .toggle = true
    },
    {
      .code = KEY_LEFTMETA,
      .label = "SUP",
      .toggle = true },
    { KEY_SPACE, "", 3 }
  }
};

static constexpr Key right_keys[][6] = {
  {
    { KEY_Y, "Z" }, // For DE layout
    { KEY_U, "U" },
    { KEY_I, "I" },
    { KEY_O, "O" },
    { KEY_P, "P" },
    { KEY_BACKSPACE, "<--" }
  },
  {
    { KEY_H, "H" }, { KEY_J, "J" }, { KEY_K, "K" }, { KEY_L, "L" }, {
      .code = KEY_ENTER,
      .label = "ENT",
      .width = 2

    }
  },
  { { KEY_N, "N" }, { KEY_M, "M" }, { KEY_COMMA, "," }, { KEY_DOT, "." }, { KEY_MINUS, "-" } },
  {
    { KEY_SPACE, "", 3},
    {
      .code = KEY_LEFTALT,
      .label = "ALT",
      .toggle = true
    },
    {
      .code = KEY_RIGHTCTRL,
      .label = "CTRL",
      .toggle = true
    }
  },
};
