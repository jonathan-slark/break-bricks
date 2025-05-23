#undef  GLAD_GL_IMPLEMENTATION
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#ifndef NDEBUG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include <cglm/struct.h> // mat4s, glms_ortho, vec3s
#include <glad.h>        // gl*, GL*
#ifndef NDEBUG
#include <stb/stb_image_write.h>
#include <stdlib.h>      // malloc, free
#endif

#include "../main.h"
#include "gfx.h"
#include "shader.h"

// Function prototypes
#ifndef NDEBUG
static bool isMember(const unsigned array[], size_t size, unsigned value);
static void GLAPIENTRY debugOutput(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message, const void* userparam);
#endif // !NDEBUG

// Constants
#ifndef NDEBUG
static const unsigned LOG_IGNORE[] =
{
    131185, // Buffer info
};
static const char SCREENSHOT[] = "screenshot.png";
static const int NUM_CHANNELS  = 3;
#endif // !NDEBUG
static const char SHADER_VERT[] = "shader/vert.glsl";
static const char SHADER_FRAG[] = "shader/frag.glsl";

// Variables
static Shader shader;

// Function definitions

#ifndef NDEBUG

bool isMember(const unsigned array[], size_t size, unsigned value)
{
    for (size_t i = 0; i < size; i++) {
        if (array[i] == value) return true;
    }

    return false;
}

void GLAPIENTRY debugOutput(
    [[maybe_unused]] GLenum source,
    [[maybe_unused]] GLenum type,
    GLuint id,
    [[maybe_unused]] GLenum severity,
    [[maybe_unused]] GLsizei length,
    const GLchar* message,
    [[maybe_unused]] const void* userparam
) {
    if (isMember(LOG_IGNORE, sizeof(LOG_IGNORE), id)) return;

    fprintf(stderr, "%u: %s\n", id, (const char*) message);
}

void gfx_screenshot(void)
{
    size_t dataSize = NUM_CHANNELS * SCR_WIDTH * SCR_HEIGHT;
    GLubyte* data = malloc(dataSize);

    if (data) {
	glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_flip_vertically_on_write(true);
	stbi_write_png(
		SCREENSHOT,
		SCR_WIDTH,
		SCR_HEIGHT,
		NUM_CHANNELS,
		data,
		SCR_WIDTH * NUM_CHANNELS
		);

	free(data);
    }
}

#endif // !NDEBUG

void gfx_init(void)
{
#ifndef NDEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB(debugOutput, NULL);
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }
#endif // !NDEBUG

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader = shader_load(SHADER_VERT, SHADER_FRAG);
    shader_use(shader);
    gfx_resize(SCR_WIDTH, SCR_HEIGHT);
}

void gfx_term(void)
{
    shader_unload(shader);
}

void gfx_resize(int width, int height)
{
    glViewport(0, 0, width, height);

    // Using origin top left to match coords typically used with images
    mat4s proj = glms_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    shader_setProj(shader, proj);
}

void gfx_clear(vec3s col)
{
    glClearColor(col.r, col.g, col.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

Shader gfx_getShader(void)
{
    return shader;
}
