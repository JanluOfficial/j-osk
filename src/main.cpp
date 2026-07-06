#include <peel/Gtk/Application.h>
#include <peel/Gtk/Window.h>
#include <peel/Gtk/Box.h>
#include <peel/Gtk/Button.h>
#include <peel/Gtk/ToggleButton.h>
#include <peel/Gtk/Orientation.h>
#include <peel/Gtk4LayerShell/Gtk4LayerShell.h>
#include <peel/Gio/ApplicationFlags.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <cstring>
#include <iostream>

#include <nlohmann/json.hpp>

#include "key.h"
#include "sysinput.h"

using namespace peel;
using namespace nlohmann::json_abi_v3_12_0;

// Window building function (duh)
static void build_window(Gtk::Application *app, int uinput_fd) {
  namespace LS = Gtk4LayerShell;

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

  for (auto& row : left_keys) {
    auto row_box = Gtk::Box::create(Gtk::Orientation::HORIZONTAL, 6);
    row_box->set_homogeneous(true);

    for (Key key : row) {
      if (!key.label) continue;

      auto button = Gtk::Button::create_with_label(key.label);
      

      if (key.width >= 0.5) {
         button->set_size_request(key.width * 40, 40);
      }

      int code = key.code;
      button->connect_clicked([uinput_fd, code](Gtk::Button* /*btn*/) {

        emit_key_press(uinput_fd, code);
      });

      row_box->append(std::move(button));
    }

    // Add the completed horizontal row container into our vertical layout stack
    main_box->append(std::move(row_box));
  }

  window->set_child(std::move(main_box));
  window->present();
}

int main(int argc, char *argv[]) {
  // Set up uinput virtual device (makes it worky :D )
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  ioctl(fd, UI_SET_EVBIT, EV_KEY);

  for (auto &row : left_keys)
    for (auto &key : row)
      ioctl(fd, UI_SET_KEYBIT, key.code);

  for (auto &row : right_keys)
    for (auto &key : row)
      ioctl(fd, UI_SET_KEYBIT, key.code);

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

  app->connect_activate ([fd] (Gio::Application *gio_app) {
    auto *gtk_app = reinterpret_cast<Gtk::Application *>(gio_app);
    build_window(gtk_app, fd);
  });

  return app->run(argc, argv);
}
