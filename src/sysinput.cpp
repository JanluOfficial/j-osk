#include <sysinput.h>
#include <linux/uinput.h>
#include <unistd.h>

struct input_event ie;

namespace j_osk {

void init_uinput_device(int fd) {
  ioctl(fd, UI_SET_EVBIT, EV_KEY);
}

void emit_key_down(int fd, int key_code) {
  ie.type = EV_KEY; ie.code = key_code; ie.value = 1;
  write (fd, &ie, sizeof (ie));
  ie.type = EV_SYN; ie.code = SYN_REPORT; ie.value = 0;
  write (fd, &ie, sizeof (ie));
}

void emit_key_up(int fd, int key_code) {
  ie.type = EV_KEY; ie.code = key_code; ie.value = 0;
  write (fd, &ie, sizeof (ie));
  ie.type = EV_SYN; ie.code = SYN_REPORT; ie.value = 0;
  write (fd, &ie, sizeof (ie));
}

void emit_key_press(int fd, int key_code) {
  emit_key_down(fd, key_code);
  emit_key_up(fd, key_code);
}

} // Namespace j_osk