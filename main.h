/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#include <cglm/struct.h>

extern const unsigned SCR_WIDTH;
extern const unsigned SCR_HEIGHT;

void  main_term(int status, const char* fmt, ...);
void  main_quit(void);
vec2s main_get_mouse_pos(void);
void  main_set_mouse_pos(vec2s pos);
