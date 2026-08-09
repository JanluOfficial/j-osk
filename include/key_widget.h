#pragma once

#include <functional>
#include <memory>
#include <optional>

#include <peel/Gtk/Gtk.h>

#include "layout_parser.h"
#include "modifier.h"

using namespace peel;

// Defined in main.cpp, handles "text:"
void handle_layout_action(int uinput_fd, const std::string &action,
                           const std::shared_ptr<j_osk::ModifierState> &modifier_state);

namespace j_osk {
  struct KeyButtonEntry {
    Gtk::Button *button;
    std::optional<Modifier> modifier; // nullopt = regular key
  };
  
  Gtk::Button *create_key_button(const LayoutKey &key);

  void wire_key_action(Gtk::Button *button, const LayoutKey &key,
                        int uinput_fd,
                        const std::shared_ptr<ModifierState> &modifier_state,
                        std::function<void()> on_modifier_changed);
}