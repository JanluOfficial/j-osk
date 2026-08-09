#include <modifier.h>

#include <iostream>
#include <optional>
#include <string>
#include <bits/stdc++.h>

#include <linux/input-event-codes.h>

namespace j_osk {

bool ModifierState::is_active(Modifier mod) {
  switch (mod) {
    case Modifier::Shift: return shift_;
    case Modifier::Control: return control_;
    case Modifier::Alt: return alt_;
    case Modifier::Super: return super_;
    default:
      std::cout << "[WARNING] Could not get active state of unknown modifier. Returning false.";
      return false;
  }
}

void ModifierState::set_active(Modifier mod, bool active) {
  switch (mod) {
    case Modifier::Shift:
      shift_ = active;
      break;
    case Modifier::Control:
      control_ = active;
      break;
    case Modifier::Alt:
      alt_ = active;
      break;
    case Modifier::Super:
      super_ = active;
      break;
    default:
      std::cout << "[WARNING] Could not set unknown modifier. Ignoring.";
      break;
  }
}

void ModifierState::toggle(Modifier mod) {
  set_active(mod, !is_active(mod));
}

std::optional<Modifier> parse_modifier(std::string name) {
  std::transform(name.begin(), name.end(), name.begin() ,::tolower);

  if (name == "shift")
    return Modifier::Shift;
  if (name == "control")
    return Modifier::Control;
  if (name == "alt")
    return Modifier::Alt;
  if (name == "super" || name == "meta")
    return Modifier::Super;
  std::cout << "[WARNING] Cannot parse modifier with name \"" << name << "\".";
  return;
}

std::optional<int> get_modifier_keycode(Modifier mod) {
  switch (mod) {
    case Modifier::Shift: return KEY_LEFTSHIFT;
    case Modifier::Control: return KEY_LEFTCTRL;
    case Modifier::Alt: return KEY_LEFTALT;
    case Modifier::Super: return KEY_LEFTMETA;
    default:
      std::cout << "[WARNING] Could not get Keycode for modifier.";
      return;
  }
}

}