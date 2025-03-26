#pragma once
#undef GLAD_GL_IMPLEMENTATION

#include <cglm/struct.h> // mat4s, vec3s
#include <glad.h>        // gl*, GL*

// Types
typedef struct {
    GLuint prog;
    GLint  loc_proj;
    GLint  loc_tex;
    GLint  loc_col;
    GLbool loc_isFont;
} Shader;

// Function prototypes
GLint  shader_compile(GLenum type, const GLchar* src);
Shader shader_load(const char* vert, const char* frag);
void   shader_unload(Shader s);
void   shader_use(Shader s);
void   shader_setProj(Shader s, mat4s proj);
void   shader_setTex(Shader s, GLint tex);
void   shader_setCol(Shader s, vec3s col);
void   shader_setIsFont(Shader s, bool isFont);
