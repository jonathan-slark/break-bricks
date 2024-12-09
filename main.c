#define GLAD_GL_IMPLEMENTATION
#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "util.h"

/* Variables */
static const unsigned int ignorelog[] = {
    131185 /* Buffer info */
};
static GLFWwindow *window;

/* Function implementations */

void
errorcallback(int err, const char *desc)
{
    fprintf(stderr, "%i: %s\n", err, desc);
}

void
init(void)
{
    glfwSetErrorCallback(errorcallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
}

void
term(int status, const char *fmt, ...)
{
    va_list ap;

    if (window)
        glfwDestroyWindow(window);

    glfwTerminate();

    if (fmt) {
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }

    exit(status);
}

void
keycallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    UNUSED(scancode);
    UNUSED(mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void
resizecallback(GLFWwindow* window, int width, int height)
{
    UNUSED(window);

    glViewport(0, 0, width, height);
}

#ifndef NDEBUG

int
ismember(const unsigned int array[], size_t size, unsigned int value)
{
    size_t i;

    for (i = 0; i < size; i++)
        if (array[i] == value)
            return 1;

    return 0;
}

void APIENTRY gldebugoutput(GLenum source, GLenum type, unsigned int id,
        GLenum severity, GLsizei length, const char *message,
        const void *userparam)
{
    UNUSED(source);
    UNUSED(type);
    UNUSED(severity);
    UNUSED(length);
    UNUSED(userparam);

    if (ismember(ignorelog, sizeof(ignorelog), id))
        return;

    fprintf(stderr, "%u: %s\n", id, message);
}

#endif /* !NDEBUG */

void
createwindow(void)
{
    int version, flags;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openglmajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openglminor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif /* !NDEBUG */
    if (!(window = glfwCreateWindow(scrwidth, scrheight, title, NULL, NULL)))
        term(EXIT_FAILURE, NULL);

    glfwMakeContextCurrent(window);

    if (!(version = gladLoadGL(glfwGetProcAddress)))
        term(EXIT_FAILURE, "Failed to load OpenGL.\n");

    glfwSetKeyCallback(window, keycallback);
    glfwSetFramebufferSizeCallback(window, resizecallback);
    glfwSwapInterval(1);

#ifndef NDEBUG
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gldebugoutput, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                GL_TRUE);
    }
#endif /* !NDEBUG */
}

int
main(void)
{
    init();
    createwindow();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    term(EXIT_SUCCESS, NULL);
}
