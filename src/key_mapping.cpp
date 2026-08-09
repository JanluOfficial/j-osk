#include "key_mapping.h"

#include "sysinput.h"
#include "modifier.h"

#include <linux/input-event-codes.h>

std::optional<KeyAction> map_char_to_key(char c) {
  switch (c) {
    case 'a': return KeyAction{KEY_A, false};
    case 'b': return KeyAction{KEY_B, false};
    case 'c': return KeyAction{KEY_C, false};
    case 'd': return KeyAction{KEY_D, false};
    case 'e': return KeyAction{KEY_E, false};
    case 'f': return KeyAction{KEY_F, false};
    case 'g': return KeyAction{KEY_G, false};
    case 'h': return KeyAction{KEY_H, false};
    case 'i': return KeyAction{KEY_I, false};
    case 'j': return KeyAction{KEY_J, false};
    case 'k': return KeyAction{KEY_K, false};
    case 'l': return KeyAction{KEY_L, false};
    case 'm': return KeyAction{KEY_M, false};
    case 'n': return KeyAction{KEY_N, false};
    case 'o': return KeyAction{KEY_O, false};
    case 'p': return KeyAction{KEY_P, false};
    case 'q': return KeyAction{KEY_Q, false};
    case 'r': return KeyAction{KEY_R, false};
    case 's': return KeyAction{KEY_S, false};
    case 't': return KeyAction{KEY_T, false};
    case 'u': return KeyAction{KEY_U, false};
    case 'v': return KeyAction{KEY_V, false};
    case 'w': return KeyAction{KEY_W, false};
    case 'x': return KeyAction{KEY_X, false};
    case 'y': return KeyAction{KEY_Y, false};
    case 'z': return KeyAction{KEY_Z, false};

    case 'A': return KeyAction{KEY_A, true};
    case 'B': return KeyAction{KEY_B, true};
    case 'C': return KeyAction{KEY_C, true};
    case 'D': return KeyAction{KEY_D, true};
    case 'E': return KeyAction{KEY_E, true};
    case 'F': return KeyAction{KEY_F, true};
    case 'G': return KeyAction{KEY_G, true};
    case 'H': return KeyAction{KEY_H, true};
    case 'I': return KeyAction{KEY_I, true};
    case 'J': return KeyAction{KEY_J, true};
    case 'K': return KeyAction{KEY_K, true};
    case 'L': return KeyAction{KEY_L, true};
    case 'M': return KeyAction{KEY_M, true};
    case 'N': return KeyAction{KEY_N, true};
    case 'O': return KeyAction{KEY_O, true};
    case 'P': return KeyAction{KEY_P, true};
    case 'Q': return KeyAction{KEY_Q, true};
    case 'R': return KeyAction{KEY_R, true};
    case 'S': return KeyAction{KEY_S, true};
    case 'T': return KeyAction{KEY_T, true};
    case 'U': return KeyAction{KEY_U, true};
    case 'V': return KeyAction{KEY_V, true};
    case 'W': return KeyAction{KEY_W, true};
    case 'X': return KeyAction{KEY_X, true};
    case 'Y': return KeyAction{KEY_Y, true};
    case 'Z': return KeyAction{KEY_Z, true};

    case '1': return KeyAction{KEY_1, false};
    case '2': return KeyAction{KEY_2, false};
    case '3': return KeyAction{KEY_3, false};
    case '4': return KeyAction{KEY_4, false};
    case '5': return KeyAction{KEY_5, false};
    case '6': return KeyAction{KEY_6, false};
    case '7': return KeyAction{KEY_7, false};
    case '8': return KeyAction{KEY_8, false};
    case '9': return KeyAction{KEY_9, false};
    case '0': return KeyAction{KEY_0, false};

    case ' ': return KeyAction{KEY_SPACE, false};
    case ',': return KeyAction{KEY_COMMA, false};
    case '.': return KeyAction{KEY_DOT, false};
    case '-': return KeyAction{KEY_MINUS, false};
    case ':': return KeyAction{KEY_SEMICOLON, true};
    case ';': return KeyAction{KEY_SEMICOLON, false};
    case '/': return KeyAction{KEY_SLASH, false};
    case '?': return KeyAction{KEY_SLASH, true};
    case '!': return KeyAction{KEY_1, true};
    case '@': return KeyAction{KEY_2, true};
    case '#': return KeyAction{KEY_3, true};
    case '$': return KeyAction{KEY_4, true};
    case '%': return KeyAction{KEY_5, true};
    case '^': return KeyAction{KEY_6, true};
    case '&': return KeyAction{KEY_7, true};
    case '*': return KeyAction{KEY_8, true};
    case '(': return KeyAction{KEY_9, true};
    case ')': return KeyAction{KEY_0, true};
    case '_': return KeyAction{KEY_MINUS, true};
    case '+': return KeyAction{KEY_EQUAL, true};
    case '=': return KeyAction{KEY_EQUAL, false};
    default: return std::nullopt;
  }
}

std::optional<int> map_special_key(const std::string &name) {
  if (name == "backspace") return KEY_BACKSPACE;
  if (name == "enter")     return KEY_ENTER;
  if (name == "tab")       return KEY_TAB;
  if (name == "left")      return KEY_LEFT;
  if (name == "right")     return KEY_RIGHT;
  if (name == "up")        return KEY_UP;
  if (name == "down")      return KEY_DOWN;
  return std::nullopt;
}

void emit_key_press_with_modifiers(int fd, int key_code, bool shift, const j_osk::ModifierState &state) {
  bool temp_shift = shift && !state.is_active(j_osk::Modifier::Shift);

  if (state.is_active(j_osk::Modifier::Control))
    j_osk::emit_key_down(fd, KEY_LEFTCTRL);
  if (state.is_active(j_osk::Modifier::Alt))
    j_osk::emit_key_down(fd, KEY_LEFTALT);
  if (state.is_active(j_osk::Modifier::Super))
    j_osk::emit_key_down(fd, KEY_LEFTMETA);
  if (temp_shift)
    j_osk::emit_key_down(fd, KEY_LEFTSHIFT);

  j_osk::emit_key_press(fd, key_code);

  if (temp_shift)
    j_osk::emit_key_up(fd, KEY_LEFTSHIFT);
  if (state.is_active(j_osk::Modifier::Super))
    j_osk::emit_key_up(fd, KEY_LEFTMETA);
  if (state.is_active(j_osk::Modifier::Alt))
    j_osk::emit_key_up(fd, KEY_LEFTALT);
  if (state.is_active(j_osk::Modifier::Control))
    j_osk::emit_key_up(fd, KEY_LEFTCTRL);
}
