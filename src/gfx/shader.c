#undef GLAD_GL_IMPLEMENTATION

#include <cglm/struct.h> // mat4s, vec3s
#include <glad.h>        // gl*, GL*

#include "../main.h"
#include "../util.h"
#include "shader.h"

// Function prototypes
static void showLog(GLuint object, PFNGLGETSHADERIVPROC proc_param, PFNGLGETSHADERINFOLOGPROC proc_log);

// Constants
static const GLchar UNIFORM_PROJ[]    = "proj";
static const GLchar UNIFORM_TEX[]     = "tex";
static const GLchar UNIFORM_COL[]     = "col";
static const GLchar UNIFORM_IS_FONT[] = "isFont";

// Function definitions

void showLog(GLuint object, PFNGLGETSHADERIVPROC proc_param, PFNGLGETSHADERINFOLOGPROC proc_log)
{
    GLint len;
    proc_param(object, GL_INFO_LOG_LENGTH, &len);
    if (len) {
        GLchar* log = (GLchar*) malloc(len * sizeof(GLchar));
        proc_log(object, len, &len, log);
        fprintf(stderr, (char*) log);
        free(log);
    }
}

GLint shader_compile(GLenum type, const GLchar* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    GLint isCompiled;
    glGetShaderiv(s, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled) {
        showLog(s, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(s);
        main_term(EXIT_FAILURE, "Could not load shader.\n");
    }
    return s;
}

Shader shader_load(const char* vert, const char* frag)
{
    GLchar* vSrc = (GLchar*) util_load(vert, READ_ONLY_TEXT);
    GLchar* fSrc = (GLchar*) util_load(frag, READ_ONLY_TEXT);
    GLuint v = shader_compile(GL_VERTEX_SHADER,   vSrc);
    GLuint f = shader_compile(GL_FRAGMENT_SHADER, fSrc);
    util_unload(vSrc);
    util_unload(fSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    glDeleteShader(v);
    glDeleteShader(f);

    GLint isLinked;
    glGetProgramiv(prog, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        showLog(prog, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(prog);
        main_term(EXIT_FAILURE, "Could not link shaders.\n");
    }

    return (Shader) {
        .prog       = prog,
        .loc_proj   = glGetUniformLocation(prog, UNIFORM_PROJ),
        .loc_tex    = glGetUniformLocation(prog, UNIFORM_TEX),
        .loc_col    = glGetUniformLocation(prog, UNIFORM_COL),
        .loc_isFont = glGetUniformLocation(prog, UNIFORM_IS_FONT)
    };
}

void shader_unload(Shader s)
{
    glDeleteProgram(s.prog);
}

void shader_use(Shader s)
{
    glUseProgram(s.prog);
}

void shader_setProj(Shader s, mat4s proj)
{
    glUniformMatrix4fv(s.loc_proj, 1, GL_FALSE, proj.raw[0]);
}

void shader_setTex(Shader s, GLint tex)
{
    glUniform1i(s.loc_tex, tex);
}

void shader_setCol(Shader s, vec3s col)
{
    glUniform3f(s.loc_col, col.r, col.g, col.b);
}

void shader_setIsFont(Shader s, bool isFont)
{
    glUniform1i(s.loc_isFont, (GLint) isFont);
}
