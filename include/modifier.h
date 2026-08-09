#pragma once

#include <optional>
#include <string>

namespace j_osk {
  // TODO: right variant modifiers when eventually needed (e.g. vbox host key)
  enum class Modifier { Shift, Control, Alt, Super };

  class ModifierState {
    private:
      bool shift_ = false, control_ = false, alt_ = false, super_ = false;

    public:
      bool is_active(Modifier mod);
      void set_active(Modifier mod, bool active);
      void toggle(Modifier mod);
  };

  std::optional<Modifier> parse_modifier(std::string name);
  std::optional<int> get_modifier_keycode(Modifier mod);
}