#pragma once

namespace j_osk {

void emit_key_press(int fd, int key_code);
void emit_key_down(int fd, int key_code);
void emit_key_up(int fd, int key_code);

} // Namespace j_osk
