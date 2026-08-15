#include "key_widget.h"

#include <iostream>

#include <peel/Gtk/Gtk.h>

#include "key_mapping.h"
#include "modifier.h"

using namespace peel;

namespace j_osk {

namespace {
  constexpr float MIN_SIZED_WIDTH = 24.0f;
  constexpr const char MOD_PREFIX[] = "mod:";

  void update_modifier_button_style(Gtk::Button *button, bool active) {
    auto style = button->get_style_context();
    style->remove_class("active");
    style->remove_class("inactive");
    style->add_class(active ? "active" : "inactive");
  }
}

Gtk::Button *create_key_button(const LayoutKey &key) {
  auto button = Gtk::Button::create_with_label(key.label.c_str());
  button->get_style_context()->add_class("key");

  if (key.width && *key.width >= MIN_SIZED_WIDTH) {
    button->set_size_request(static_cast<int>(*key.width),
                              static_cast<int>(MIN_SIZED_WIDTH));
    button->set_hexpand(false);
  } else {
    button->set_hexpand(true);
  }

  return std::move(button).release_floating_ptr();
}

void wire_key_action(Gtk::Button *button, const LayoutKey &key,
                      int uinput_fd,
                      const std::shared_ptr<ModifierState> &modifier_state,
                      std::function<void()> on_modifier_changed) {
  const bool is_modifier_key = key.action.rfind(MOD_PREFIX, 0) == 0;

  if (!is_modifier_key) {
    button->connect_clicked([uinput_fd, action = key.action, modifier_state]
                             (Gtk::Button * /*btn*/) {
      handle_layout_action(uinput_fd, action, modifier_state);
    });
    return;
  }

  button->get_style_context()->add_class("modifier");

  auto mod = parse_modifier(key.action.substr(sizeof(MOD_PREFIX) - 1));
  if (!mod) {
    std::cout << "[WARNING] Unrecognized modifier action: " << key.action << std::endl;
    return;
  }

  update_modifier_button_style(button, modifier_state->is_active(*mod));

  button->connect_clicked([modifier_state, mod = *mod, on_modifier_changed]
                           (Gtk::Button * /*btn*/) {
    modifier_state->toggle(mod);
    if (on_modifier_changed) on_modifier_changed();
  });
}

}