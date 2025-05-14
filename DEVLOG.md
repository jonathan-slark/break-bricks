# 🛠️ Break Bricks Devlog

## April 2025 – Collision Overhaul with Swept AABB

I was struggling with inconsistent collision responses — sometimes the ball would clip or bounce unpredictably. After stepping away for a while, I came back with a fresh mindset and asked a language model for ideas. It immediately suggested [Swept AABB](https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/), which I hadn’t encountered before. I integrated a basic version and it passed all my early tests — a major breakthrough.

## February 2025 – Renderer Rewrite (Again)

The renderer from the LearnOpenGL guide was too naive for my goals. I wanted sprite batching and support for particle effects, so I rewrote the renderer to be a modern, batched system. It worked — for a while — until I tried to refactor in particles and ended up with a black screen and no errors. I learned a ton about OpenGL debugging, but in the end, I rolled back to the last working version.

## January 2025 – From Bare Bones to Playable

This was the turning point. I got the basic systems working: input, paddle movement, ball physics, brick destruction. It felt great to play at 144Hz with pixel-perfect control. Even though the visuals were minimal, the feel was exactly what I was aiming for. I started building out levels using NASA images as backgrounds and assigned each one a distinct soundtrack.

## December 2024 – The Start of Something Bigger

After testing Vulkan and OpenGL with triangle demos, I decided to turn the [Breakout guide](https://learnopengl.com/In-Practice/2D-Game/Breakout) into something real. I didn’t want to follow the guide too closely — their geometry and collision system felt overengineered for what I needed. So I started building my own subsystems: logging, audio, rendering, input. It was messy, but it was mine.

---
