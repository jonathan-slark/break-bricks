# Break Bricks

## 🧱 Overview

**Break Bricks** is a modern reinterpretation of the classic brick-breaking arcade game. It features high-resolution graphics, smooth controls, and a custom OpenGL renderer, making it a sleek and enjoyable retro-style experience.

## ▶️ Running the Game

1. Extract the ZIP archive.
2. Run `break-bricks.exe`.

*Note: The game runs in a 1920x1080 borderless window. Black borders may appear if your desktop resolution is higher.*

## ⚙️ Technical Details

- **Resolution**: 1920x1080, borderless window mode.
- **Frame Rate**: Syncs with the desktop refresh rate for smooth ball movement on all monitors.
- **Controls**: Precision mouse input for paddle movement.
- **Language**: Written in modern C (C23) with minimal dependencies.
- **Graphics**: OpenGL 3.3 batched renderer, spritesheets, and TTF font rendering.
- **Audio**: Each level features its own background music and NASA imagery.
- **Build System**: [MSYS2](https://www.msys2.org/) + GCC toolchain.

## 🎯 Project Goals

Inspired by the [Breakout guide](https://learnopengl.com/In-Practice/2D-Game/Breakout) on LearnOpenGL, this project began as a deeper dive into OpenGL after the "Hello Triangle" example. The aim was to build a full-featured, modern desktop game while avoiding the complexity of Vulkan.

## 📝 Development Notes

This project turned out to be far more complex than expected. I wanted to go beyond the original arcade simplicity, which led to unexpected challenges:

- The original game used rudimentary logic with minimal collision resolution.
- I initially implemented custom collision logic, which had inconsistencies.
- AI-assisted research introduced me to [swept AABB collision](https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/), which proved much more robust.

### Renderer Journey

- The LearnOpenGL guide uses a simple renderer for teaching, but I pushed for a modern batched renderer to support sprite batching and particles.
- This took significant effort and research. At one point, I rewrote it from scratch — only to end up with a black screen and no clear cause.

### Lessons Learned

- Over-engineering for a simple game can eat up your momentum.
- Rewriting subsystems (logging, audio, rendering) helped me learn but stalled the project.
- Sometimes it’s better to release a working version than chase perfection.
- For future projects, I’ll explore engines like Raylib to stay focused on game design, not infrastructure.

## 📜 Credits & Licenses

### Libraries

- [GLFW](https://www.glfw.org/) – zlib/libpng License
- [GLAD](https://github.com/Dav1dde/glad) – CC0 License
- [miniaudio](https://miniaud.io/) – MIT No Attribution
- [stb](https://github.com/nothings/stb) – Public Domain

### Assets

- [Space Skybox by Westbeam](https://opengameart.org/content/space-skyboxes-1) – CC0
- [Free SFX by Kronbits](https://kronbits.itch.io/freesfx) – CC0
- [Astroids by HoliznaCC0](https://freemusicarchive.org/music/holiznacc0/tiny-plastic-video-games-for-long-anxious-space-travel/astroids/) – CC0
- [Jupiteroid Font](https://ggbot.itch.io/jupiteroid-font) – CC0
- [Stars by Bonsaiheldin](https://opengameart.org/content/stars-parallax-backgrounds) – CC0

### NASA Imagery

All NASA and ESA images are public domain or under [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

## 🔧 Build Dependencies (MSYS2)

- `mingw-w64-ucrt-x86_64-cglm`
- `mingw-w64-ucrt-x86_64-gcc`
- `mingw-w64-ucrt-x86_64-glfw`
- `mingw-w64-ucrt-x86_64-stb`
