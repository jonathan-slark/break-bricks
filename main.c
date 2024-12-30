/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "main.h"
#include "util.h"

/* Function declarations */
static void errorcallback(int err, const char* desc);
static void init(void);
static void keycallback(GLFWwindow* window, int key, int scancode, int action,
			int mods);
static void resizecallback(GLFWwindow* window, int width, int height);
#ifndef NDEBUG
static bool ismember(const unsigned array[], size_t size, unsigned value);
static void GLAPIENTRY gldebugoutput(GLenum source, GLenum type, GLuint id,
				     GLenum severity, GLsizei length, const GLchar* message, const void* userparam);
#endif /* !NDEBUG */
static void createwindow(void);

/* Variables */
#ifndef NDEBUG
static const unsigned ignorelog[] = {
    131185, /* Buffer info */
    131204, /* Texture mapping warning */
    131218  /* Recompilation warning */
};
#endif /* !NDEBUG */
static GLFWwindow* window = NULL;
static bool minimised = false;

/* Function implementations */

void errorcallback(int err, const char* desc) {
    fprintf(stderr, "%i: %s\n", err, desc);
}

void init(void) {
    glfwSetErrorCallback(errorcallback);

    if (!glfwInit())
	exit(EXIT_FAILURE);
}

void term(int status, const char* fmt, ...) {
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

void keycallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode,
		 int action, [[maybe_unused]] int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_PRESS)
	game_keydown(key);
    else if (action == GLFW_RELEASE)
	game_keyup(key);
}

void resizecallback([[maybe_unused]] GLFWwindow* window, int width,
		    int height) {
    if (!width || !height) {
	minimised = true;
    } else {
	minimised = false;
	glViewport(0, 0, width, height);
    }
}

#ifndef NDEBUG

bool ismember(const unsigned array[], size_t size, unsigned value) {
    for (size_t i = 0; i < size; i++)
	if (array[i] == value)
	    return true;

    return false;
}

void GLAPIENTRY gldebugoutput([[maybe_unused]] GLenum source, [[maybe_unused]]
			      GLenum type, GLuint id, [[maybe_unused]] GLenum severity, [[maybe_unused]]
			      GLsizei length, const GLchar* message, [[maybe_unused]] const void*
			      userparam) {
    if (ismember(ignorelog, sizeof(ignorelog), id))
	return;

    fprintf(stderr, "%u: %s\n", id, (const char*)message);
}

#endif /* !NDEBUG */

void createwindow(void) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglmajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglminor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif /* __APPLE__ */
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif /* !NDEBUG */

    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);

    /* Request 8-bit full screen but keep the refresh rate the same. If this
     * matches the current mode then it will use borderless window. */
    glfwWindowHint(GLFW_RED_BITS, scrredbits);
    glfwWindowHint(GLFW_GREEN_BITS, scrgreenbits);
    glfwWindowHint(GLFW_BLUE_BITS, scrbluebits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    if (!(window = glfwCreateWindow(scrwidth, scrheight, title, mon, NULL)))
	term(EXIT_FAILURE, NULL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwMakeContextCurrent(window);
    int ver = gladLoadGL(glfwGetProcAddress);
    if (!ver)
	term(EXIT_FAILURE, "Failed to load OpenGL.\n");

    glfwSetKeyCallback(window, keycallback);
    glfwSetFramebufferSizeCallback(window, resizecallback);
    glfwSwapInterval(1);

#ifndef NDEBUG
    if (GLAD_GL_ARB_debug_output) {
	GLint flags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
	    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	    glDebugMessageCallbackARB(gldebugoutput, NULL);
	    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
				     GL_TRUE);
	}
    }
#endif /* !NDEBUG */

    resizecallback(window, scrwidth, scrheight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	glfwPollEvents();

	if (!minimised) {
	    game_input(frametime);
	    game_update(frametime);
	    game_render();

	    glfwSwapBuffers(window);
	}
    }

    term(EXIT_SUCCESS, NULL);
}
