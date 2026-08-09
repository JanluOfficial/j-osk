#include <peel/Gtk/Application.h>
#include <peel/Gtk/Window.h>
#include <peel/Gtk/Box.h>
#include <peel/Gtk/Button.h>
#include <peel/Gtk/ToggleButton.h>
#include <peel/Gtk/Orientation.h>
#include <peel/Gtk/CssProvider.h>
#include <peel/Gtk/StyleContext.h>
#include <peel/Gdk/Display.h>
#include <peel/Gtk4LayerShell/Gtk4LayerShell.h>
#include <peel/Gio/ApplicationFlags.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>

#include <gio/gio.h>
#include <nlohmann/json.hpp>

#include "josk-resources.h"
#include "key_mapping.h"
#include "layout_parser.h"
#include "sysinput.h"
#include "key_widget.h"

using namespace peel;
using namespace nlohmann::json_abi_v3_12_0;

static j_osk::Layout load_layout_from_resource() {
  GResource *resource = josk_get_resource();
  if (!resource) {
    throw std::runtime_error("Unable to load embedded GResource");
  }

  g_resources_register(resource);

  GError *error = nullptr;
  GBytes *bytes = g_resource_lookup_data(resource,
      "/org/josk/assets/layouts/en-US.default.json",
      G_RESOURCE_LOOKUP_FLAGS_NONE,
      &error);

  if (!bytes) {
    std::string message = "Unable to load layout from resource.";
    if (error) {
      message += " ";
      message += error->message;
      g_error_free(error);
    }
    throw std::runtime_error(message);
  }

  gsize size;
  const char *data = reinterpret_cast<const char *>(g_bytes_get_data(bytes, &size));
  std::string json_text(data, size);
  g_bytes_unref(bytes);

  return j_osk::LayoutParser::parseJson(nlohmann::json::parse(json_text));
}

void handle_layout_action(int uinput_fd, const std::string &action,
  const std::shared_ptr<j_osk::ModifierState> &modifier_state) {
  constexpr const char prefix_text[] = "text:";
  constexpr const char prefix_key[]  = "key:";

  if (action.rfind(prefix_text, 0) == 0) {
    // Text
    std::string text = action.substr(sizeof(prefix_text) - 1);
    for (char c : text) {
      auto mapped = map_char_to_key(c);
      if (!mapped)
        continue;

      emit_key_press_with_modifiers(uinput_fd, mapped->key_code, mapped->shift, *modifier_state);
    }
    return;
  }

  if (action.rfind(prefix_key, 0) == 0) {
    if (auto code = map_special_key(action.substr(sizeof(prefix_key) - 1)))
      emit_key_press_with_modifiers(uinput_fd, *code, false, *modifier_state);
    return;
  }

  std::cout << "Unhandled layout action: " << action << std::endl;
}

static void build_window(Gtk::Application *app, int uinput_fd,
  const j_osk::Layout &layout,
  const std::shared_ptr<j_osk::ModifierState> &modifier_state) {
  namespace LS = Gtk4LayerShell;

  auto css_provider = Gtk::CssProvider::create();
  css_provider->load_from_resource("/org/josk/assets/style.css");
  auto style_provider = std::move(css_provider).cast<Gtk::StyleProvider>();
  auto display = Gdk::Display::get_default();
  if (display) {
    Gtk::StyleContext::add_provider_for_display(
      display,
      style_provider,
      GTK_STYLE_PROVIDER_PRIORITY_USER);
  }

  auto *window = Gtk::Window::create();
  window->set_application(app);
  window->set_title("J-OSK");

  LS::init_for_window(window);
  LS::set_layer(window, LS::Layer::TOP);
  LS::set_keyboard_mode(window, LS::KeyboardMode::NONE);

  // Pin to bottom, stretch full width
  LS::set_anchor(window, LS::Edge::BOTTOM, true);
  LS::set_anchor(window, LS::Edge::LEFT,   true);
  LS::set_anchor(window, LS::Edge::RIGHT,  true);
  LS::auto_exclusive_zone_enable(window);

  auto main_box = Gtk::Box::create(Gtk::Orientation::VERTICAL, 6); // 6px gap between rows

  auto modifier_buttons = std::make_shared<std::vector<j_osk::KeyButtonEntry>>();

  auto sync_modifier_buttons = [modifier_buttons, modifier_state] {
    for (const auto &entry : *modifier_buttons) {
      auto style = entry.button->get_style_context();
      style->remove_class("active");
      style->remove_class("inactive");
      style->add_class(modifier_state->is_active(*entry.modifier) ? "active" : "inactive");
    }
  };

  for (const auto &row : layout.rows) {
    auto row_box = Gtk::Box::create(Gtk::Orientation::HORIZONTAL, 6);
    //row_box->set_homogeneous(true);

    for (const auto &key : row.keys) {
      if (key.label.empty())
        continue;

      auto *button = j_osk::create_key_button(key);

      if (key.action.rfind("mod:", 0) == 0) {
        if (auto mod = j_osk::parse_modifier(key.action.substr(sizeof("mod:") - 1)))
          modifier_buttons->push_back({button, *mod});
      }

      j_osk::wire_key_action(button, key, uinput_fd, modifier_state, sync_modifier_buttons);
      row_box->append(button);
    }

    main_box->append(std::move(row_box));
  }

  sync_modifier_buttons();
  window->set_child(std::move(main_box));
  window->present();
}

int main(int argc, char *argv[]) {
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK); // uinput virtual device setup

  ioctl(fd, UI_SET_EVBIT, EV_KEY);

  auto collect_keycodes = [&](const j_osk::Layout &layout) {
    std::set<int> codes;
    for (auto mod : {j_osk::Modifier::Shift, j_osk::Modifier::Control,
                      j_osk::Modifier::Alt, j_osk::Modifier::Super}) {
      if (auto code = j_osk::get_modifier_keycode(mod))
        codes.insert(*code);
    }

    for (const auto &row : layout.rows) {
      for (const auto &key : row.keys) {
        if (key.action.rfind("text:", 0) == 0) {
          std::string text = key.action.substr(sizeof("text:") - 1);
          for (char c : text) {
            auto mapped = map_char_to_key(c);
            if (!mapped)
              continue;
            codes.insert(mapped->key_code);
            if (mapped->shift)
              codes.insert(KEY_LEFTSHIFT);
          }
          continue;
        }
 
        if (key.action.rfind("key:", 0) == 0) {
          if (auto code = map_special_key(key.action.substr(sizeof("key:") - 1)))
            codes.insert(*code);
          continue;
        }

      }
    }

    return codes;
  };

  auto layout = load_layout_from_resource();
  auto keycodes = collect_keycodes(layout);
  for (int code : keycodes)
    ioctl(fd, UI_SET_KEYBIT, code);

  auto modifier_state = std::make_shared<j_osk::ModifierState>();

  //std::cout << "KEY_Q is " << KEY_Q << std::endl; // This was for debug

  struct uinput_setup us;
  memset(&us, 0, sizeof(us));
  us.id.bustype = BUS_USB;
  us.id.vendor  = 0x1234;
  us.id.product = 0x5678;
  strcpy(us.name, "J-OSK Virtual Input Device");

  ioctl(fd, UI_DEV_SETUP, &us);
  ioctl(fd, UI_DEV_CREATE);

  auto app = Gtk::Application::create (
    "org.janlu.j-osk",
    Gio::Application::Flags::DEFAULT_FLAGS
  );

  app->connect_activate ([fd, layout = std::move(layout), modifier_state] (Gio::Application *gio_app) {
    auto *gtk_app = reinterpret_cast<Gtk::Application *>(gio_app);
    build_window(gtk_app, fd, layout, modifier_state);
  });

  return app->run(argc, argv);
}