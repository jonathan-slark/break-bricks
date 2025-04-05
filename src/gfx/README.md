# Graphics Subsystem

This subsystem provides the core functionality for rendering in Break Bricks. It includes modules for textures, sprites, shaders, rendering, and font rendering.

## Features

- **Texture Management:** Loading and creation of textures (see tex.h / tex.c).
- **Sprite Management:** Creation and manipulation of sprites for 2D objects (see sprite.h / sprite.c).
- **Shader Handling:** Compiling, loading, and using shaders for rendering (see shader.h / shader.c).
- **Rendering Pipeline:** Batching and drawing of sprites using vertex buffers (see rend.h / rend.c).
- **Font Rendering:** Loading TrueType fonts and rendering text (see font.h / font.c).

## File Structure

- **src/gfx/tex.h, tex.c:** Texture creation, loading, and unloading.
- **src/gfx/sprite.h, sprite.c:** Sprite definitions and transformations.
- **src/gfx/shader.h, shader.c:** Shader program compilation and management.
- **src/gfx/rend.h, rend.c:** Renderer creation, batching, and drawing routines.
- **src/gfx/gfx.h, gfx.c:** Initialization and management of the graphics context.
- **src/gfx/font.h, font.c:** Font loading and text rendering using TrueType fonts.
- **src/gfx/screen.h, screen.c:** Handling of screen rendering using sprites and renderers.

## Dependencies

- OpenGL (via GLAD)
- cglm (for math, e.g., vec2s, mat4s)
- stb libraries (for image and font handling)

## License

This project is licensed under the MIT License. See the LICENSE file for details.
