#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE

#include <cglm/struct.h> // vec2s
#include <glad.h>        // gl*
#include <GLFW/glfw3.h>  // glfw*, GLFW*
#include <stdarg.h>      // va_list, va_start, va_end
#include <stdio.h>       // fprintf, vfprintf
#include <stdlib.h>      // exit, atexit, EXIT_SUCCESS, EXIT_FAILURE

#include "game.h"
#include "gfx.h"
#include "main.h"

// Function declarations
static void errorCallback(int err, const char* desc);
static void init(void);
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
static void resizeCallback(GLFWwindow* window, int width, int height);
static void createWindow(void);

// Constants
static const char     TITLE[]        = "Break Bricks";
const int             SCR_WIDTH      = 1920;
const int             SCR_HEIGHT     = 1080;
static const unsigned SCR_RED_BITS   = 8;
static const unsigned SCR_GREEN_BITS = 8;
static const unsigned SCR_BLUE_BITS  = 8;
static const unsigned OPENGL_MAJOR   = 3;
static const unsigned OPENGL_MINOR   = 3;

// Variables
static GLFWwindow* window;
static bool isMinimised = false;

// Function implementations

void errorCallback(int err, const char* desc)
{
    fprintf(stderr, "%i: %s\n", err, desc);
}

void init(void)
{
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) exit(EXIT_FAILURE);
}

void main_term(int status, const char* fmt, ...)
{
    if (window) glfwDestroyWindow(window);

    glfwTerminate();

    if (fmt)
    {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
    }

    exit(status);
}

void main_quit(void)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void keyCallback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] int key,
    [[maybe_unused]] int scancode,
    int action,
    [[maybe_unused]] int mods
) {
    if (action == GLFW_PRESS)
    {
	game_keyDown(key);
    }
    else if (action == GLFW_RELEASE)
    {
	game_keyUp(key);
    }
}

vec2s main_get_mouse_pos(void)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return (vec2s) {{ x, y }};
}

void main_setMousePos(vec2s pos)
{
    glfwSetCursorPos(window, pos.x, pos.y);
}

void mouseCallback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] int button,
    int action,
    [[maybe_unused]] int mods
) {
    if (action == GLFW_PRESS)
    {
	//game_button_down(button);
    } 
    else if (action == GLFW_RELEASE)
    {
	//game_button_up(button);
    }
}

void resizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    if (!width || !height)
    {
	isMinimised = true;
    }
    else
    {
	isMinimised = false;
	//gfx_resize(width, height);
    }
}

void createWindow(void)
{
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

    if (!(window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, TITLE, mon, nullptr)))
    {
	main_term(EXIT_FAILURE, nullptr);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
    {
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    else
    {
	main_term(EXIT_FAILURE, "Raw mouse motion not supported.\n");
    }

    glfwMakeContextCurrent(window);
    int ver = gladLoadGL(glfwGetProcAddress);
    if (!ver)
    {
        main_term(EXIT_FAILURE, "Failed to load OpenGL.\n");
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, resizeCallback);
    // Turn on Vsync
    glfwSwapInterval(1);
}

int main(void)
{
    init();
    createWindow();

    // Render one frame: the loading screen
    //game_loading();
    //game_render();
    //glfwSwapBuffers(window);
    // Render to both buffers to avoid flicker
    //game_render();
    //glfwSwapBuffers(window);

    game_load();

    // Ignore events that happened during loading
    //glfwPollEvents();
    //game_loaded();

    //double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
	//double cur_time = glfwGetTime();
	//double frame_time = cur_time - last_time;
	//last_time = cur_time;

	if (isMinimised)
	{
	    glfwWaitEvents();
	}
	else
	{
	    glfwPollEvents();
	    game_input();
	    //game_update(frame_time);
	    game_render();
	    glfwSwapBuffers(window);
	}
    }

    main_term(EXIT_SUCCESS, nullptr);
}
