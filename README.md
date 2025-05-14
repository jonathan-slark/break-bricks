# 🎮 Break Bricks

> A modern, space-themed reimagining of the arcade classic — built in C and OpenGL from the ground up.

## 🌟 Project Overview

**Break Bricks** is a desktop breakout-style game that blends old-school mechanics with modern design. Developed entirely in C23 using OpenGL 3.3, it features responsive controls, smooth frame pacing, and custom visual/audio presentation across a series of handcrafted levels.

This project was a personal milestone — my second OpenGL game and first fully completed engine-based project. It showcases my ability to design, build, and polish a desktop game from scratch.

---

## 🧩 Features

- **Resolution**: 1920×1080, borderless window mode (with letterboxing as needed)
- **Framerate**: Adaptive to your desktop refresh rate (perfect for high-Hz displays)
- **Controls**: Smooth, precise mouse input
- **Rendering**: Custom OpenGL 3.3 batched sprite renderer with TTF font support
- **Audio**: Level-based music system using Miniaudio
- **Levels**: Each level features a unique NASA space background and ambient soundtrack

---

## 🛠️ Technical Summary

- **Language**: C23 (via GCC/MSYS2)
- **Libraries**: GLFW, GLAD, stb, Miniaudio
- **Renderer**: Modular, batched OpenGL renderer built from scratch
- **Collision**: Custom implementation using [swept AABB collision](https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/)
- **Architecture**: Modular subsystems (logging, rendering, audio, input, game logic)

---

## 📸 Screenshots

*Include 2–4 curated images or a gameplay GIF here.*

---

## 📝 Development Notes

After completing the LearnOpenGL "Hello Triangle", I set out to build a complete game using the [Breakout guide](https://learnopengl.com/In-Practice/2D-Game/Breakout) as a springboard. Rather than follow it rigidly, I challenged myself to:

- Build my own renderer
- Replace geometry-heavy collision logic with something more robust
- Implement a clean subsystem structure that could scale

Along the way, I rewrote major systems like rendering, audio, and collision multiple times — chasing both performance and reliability. While it was tempting to keep refining, I made a conscious decision to finish this version and polish it as a complete release.

---

## 🗃️ Asset Credits

All third-party assets used under permissive licenses (CC0, MIT, zlib, or Public Domain):

### Libraries

- [GLFW](https://www.glfw.org/) – zlib/libpng License
- [GLAD](https://github.com/Dav1dde/glad) – CC0 License
- [miniaudio](https://miniaud.io/) – MIT No Attribution
- [stb](https://github.com/nothings/stb) – Public Domain

### Audio & Visuals

- [HoliznaCC0 Music](https://freemusicarchive.org/music/holiznacc0)
- [Kronbits SFX](https://kronbits.itch.io/freesfx)
- [Westbeam Skybox](https://opengameart.org/content/space-skyboxes-1)
- [NASA Images](https://commons.wikimedia.org) – Public Domain
- [Jupiteroid Font](https://ggbot.itch.io/jupiteroid-font) – CC0

---

## 🔧 Build Info

To build the game, install [MSYS2](https://www.msys2.org/) and ensure these packages are installed:

- `mingw-w64-ucrt-x86_64-gcc`
- `mingw-w64-ucrt-x86_64-glfw`
- `mingw-w64-ucrt-x86_64-cglm`
- `mingw-w64-ucrt-x86_64-stb`

Alternatively, use the prebuilt `break-bricks.exe` included in the release.

---

## 🧠 Reflections

This project sharpened my understanding of real-time rendering, subsystem architecture, and input handling — all while teaching me when to simplify, when to refactor, and when to ship.

It’s a complete, working desktop game that I’m proud to include in my portfolio, and it lays the foundation for future, more ambitious projects.

---
