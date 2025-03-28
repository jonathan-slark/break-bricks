#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // GLFW*

#include "../main.h"
#include "../util.h"
#include "input.h"

// Function prototypes
static void quit(void);

// Types
typedef struct
{
    int key;
    void (*func)(void);
} Key;

// Constants
static const Key KEYS[] =
{
    { GLFW_KEY_ESCAPE, quit }
};

// Function declarations

void input_keyDown(int key)
{
    for (size_t i = 0; i < COUNT(KEYS); i++)
    {
        if (KEYS[i].key == key)
        {
            (*KEYS[i].func)();
        }
    }
}

void input_keyUp([[maybe_unused]] int key)
{
    // VOID
}

void quit(void)
{
    main_quit();
}

void input_update(void)
{
}
