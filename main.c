/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <cglm/struct.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "game.h"
#include "gfx.h"

// Function declarations
static void errorcallback(int err, const char* desc);
static void init(void);
static void keycallback(GLFWwindow* window, int key, int scancode, int action,
			int mods);
static void mousecallback(GLFWwindow* window, int button, int action,
	int mods);
static void resizecallback(GLFWwindow* window, int width, int height);
static void createwindow(void);

// Constants
static const char     TITLE[]        = "Break Bricks";
const unsigned        SCR_WIDTH      = 1920;
const unsigned        SCR_HEIGHT     = 1080;
static const unsigned SCR_RED_BITS   = 8;
static const unsigned SCR_GREEN_BITS = 8;
static const unsigned SCR_BLUE_BITS  = 8;
static const unsigned OPENGL_MAJOR   = 4;
static const unsigned OPENGL_MINOR   = 6;

// Variables
static GLFWwindow* window = NULL;
static bool minimised = false;

// Function implementations

void errorcallback(int err, const char* desc) {
    fprintf(stderr, "%i: %s\n", err, desc);
}

void init(void) {
    glfwSetErrorCallback(errorcallback);

    if (!glfwInit())
	exit(EXIT_FAILURE);
}

void main_term(int status, const char* fmt, ...) {
    game_unload();

    if (window)
	glfwDestroyWindow(window);

    glfwTerminate();

    if (fmt) {
	va_list ap = NULL;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
    }

    exit(status);
}

void main_quit(void) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void keycallback([[maybe_unused]] GLFWwindow* window, int key,
	[[maybe_unused]] int scancode, int action,
	[[maybe_unused]] int mods) {
    if (action == GLFW_PRESS) {
	game_keydown(key);
    } else if (action == GLFW_RELEASE) {
	game_keyup(key);
    }
}

vec2s main_getmousepos(void) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return (vec2s) {{ x, y }};
}

void main_setmousepos(vec2s mousepos) {
    glfwSetCursorPos(window, mousepos.x, mousepos.y);
}

void mousecallback([[maybe_unused]] GLFWwindow* window, int button, int action,
	[[maybe_unused]] int mods) {
    if (action == GLFW_PRESS) {
	game_buttondown(button);
    } else if (action == GLFW_RELEASE) {
	game_buttonup(button);
    }
}

void resizecallback([[maybe_unused]] GLFWwindow* window, int width,
		    int height) {
    if (!width || !height) {
	minimised = true;
    } else {
	minimised = false;
	gfx_resize(width, height);
    }
}

void createwindow(void) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // !NDEBUG

    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);

    /* Request 8-bit full screen but keep the refresh rate the same. If this
     * matches the current mode then it will use borderless window. */
    glfwWindowHint(GLFW_RED_BITS, SCR_RED_BITS);
    glfwWindowHint(GLFW_GREEN_BITS, SCR_GREEN_BITS);
    glfwWindowHint(GLFW_BLUE_BITS, SCR_BLUE_BITS);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    if (!(window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, TITLE, mon, NULL)))
	main_term(EXIT_FAILURE, NULL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    } else {
	main_term(EXIT_FAILURE, "Raw mouse motion not supported.\n");
    }

    glfwMakeContextCurrent(window);
    int ver = gladLoadGL(glfwGetProcAddress);
    if (!ver)
        main_term(EXIT_FAILURE, "Failed to load OpenGL.\n");

    glfwSetKeyCallback(window, keycallback);
    glfwSetMouseButtonCallback(window, mousecallback);
    glfwSetFramebufferSizeCallback(window, resizecallback);
    glfwSwapInterval(1);
}

int main(void) {
    init();
    createwindow();
    game_load();

    double lasttime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
	double curtime = glfwGetTime();
	double frametime = curtime - lasttime;
	lasttime = curtime;

	if (minimised) {
	    glfwWaitEvents();
	} else {
	    glfwPollEvents();
	    game_input(frametime);
	    game_update(frametime);
	    game_render();
	    glfwSwapBuffers(window);
	}
    }

    main_term(EXIT_SUCCESS, NULL);
}
