//#pragma once
#ifndef SYSINPUT_H
#define SYSINPUT_H

void emit_key_press(int fd, int key_code);
void emit_key_down(int fd, int key_code);
void emit_key_up(int fd, int key_code);

#endif // SYSINPUT_H
