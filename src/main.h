#pragma once

#include <cglm/struct.h> // vec2s

// Constants
extern const int SCR_WIDTH;
extern const int SCR_HEIGHT;

// Function prototypes
void   main_term(int status, const char* fmt, ...);
void   main_quit(void);
double main_getMouseDx(void);
vec2s  main_getMousePos(void);
void   main_setMousePos(vec2s pos);
