# Break Bricks

## Overview

Break Bricks is a brick breaking game inspired by the classic.

## Running

Simply extract the zip somewhere and run `break-bricks.exe`

## Technical Details

1. 1920 x 1080 resolution; the game should have black borders if your desktop is at a higher resolution.
2. Runs in a borderless window for modern window behaviour.
3. Runs at the desktop refresh rate so the ball movement is silky smooth, especially with a gaming monitor. The game should feel the same no matter the refresh rate.
4. The paddle has silky smooth mouse control.
5. Written in modern C with minimal dependancies so loads and runs fast.
6. Modern OpenGL (3.3) batched renderer with spritesheets and TTF font rendering.
7. Each level has it's own music, background NASA photo and design.
8. Build environment is [MSYS2]<https://www.msys2.org/>

## Goal

After finding the Hello Triangle on the LearnOpenGL website much more straightforward than using Vulkan I decicded to attempt making a complete game based on the [Breakout]<https://learnopengl.com/In-Practice/2D-Game/Breakout> guide on the same website. This is the ideal kind of game for a second project.

## Postmortem

Oh boy! Where do I start? This proved to be a much more troublesome project than I anticipated. It started with the fact that I knew the modern version of the game and thought I would create a silky smooth modern version. Half way through, when I was struggling, I played the arcade version and realised how much simpler it is. The "ball" is basically a dot and the collision detection and resolution is rudimentary, after all it used [discreate logic]<https://en.wikipedia.org/wiki/Breakout_(video_game)#Development> and not a microprossor.

I think that is why it is often recommended as one of the first games to make as a budding games programmer. However, if you want to do this with a larger ball and modern silky smooth graphics it becomes a lot more complex. For me the LearnOpenGL Breakout guides use of geometry is overkill. For one thing I would have to relearn my [A-Level]<https://en.wikipedia.org/wiki/A-level> maths and as I mentioned above the original arcade version uses very simple bouncing rules.

After a lot of research I wrote my own collision detection and resolution but in testing I could see it would go wrong occasionally. I'm not even sure your average gamer would notice but I did! I completely rewrote this with lots of error checking but again it would go fail occasionally and I could not get to the bottom of the problem. I stepped away from the project and took a bit of a break.

Coming back to the project I decided to see if AI ([LLM]<https://en.wikipedia.org/wiki/Large_language_model>) could assist me. Straight away the AI recommended [swept AABB]<https://gamedev.net/tutorials/programming/general-and-gameplay-programming/swept-aabb-collision-detection-and-response-r3084/> which I had never come across in my research and it even made a function for me. After some simple integration the swept AABB function tested well.

Another major issue I had was the renderer. The LearnOpenGL renderer used in the Breakout guide is deliberately naive to keep the tutorial achievable. However, once you get to the Final Thoughts page at the end it lists all the optimisations modern games have so I set out to implement each one. In hindsight I should've stuck with what I had as it was plenty good enough to render the few dozen sprites in the game. I rewrote the renderer to be a modern batched renderer but that took a lot of research and effort. Even then it had it's short comings and I wanted to add particle effects so I started it again but got in a pickle where I had nothing but a black screen and even with AI help could not find the cause.

I was learning a lot from AI on how to write a good, modular, subsystem so I wrote a logging subsystem. Next I rewrote the audio subsystem but then made a design error and realised I was going to have to spend more time to fix it. I still didn't even have a working renderer with particles. I realised at this point that I was spending too much time on the rewrite and that I should go back to my last working version without particles and release it.

I've learnt a lot along the way, I think whatever engine I choose to use in the future I will still benefit from the knowledge of how the underlying subsystems work. However, as I want to make games and not engines I will try Raylib for my next project.

## Credits and Licences

[glfw (linking)]<https://www.glfw.org/>
[zlib/libpng License]<https://www.glfw.org/license.html>

[glad (generated code only)]<https://github.com/Dav1dde/glad>
[CC0 License]<https://creativecommons.org/publicdomain/zero/1.0/>

[miniaudio]<https://miniaud.io/>
[MIT No Attribution]<https://github.com/aws/mit-0>

[stb_image.h]<https://github.com/nothings/stb>
stb_rect_pack.h
stb_truetype.h
[Public Doman]<https://github.com/nothings/stb/blob/master/LICENSE>

[Space Skybox by Westbeam]<https://opengameart.org/content/space-skyboxes-1>
[CC0 License]<https://creativecommons.org/publicdomain/zero/1.0/>

[Free SFX by Kronbits]<https://kronbits.itch.io/freesfx>
[CC0 License]<https://creativecommons.org/publicdomain/zero/1.0/>

[Astroids by HoliznaCC0]<https://freemusicarchive.org/music/holiznacc0/tiny-plastic-video-games-for-long-anxious-space-travel/astroids/>
[CC0 License]<https://creativecommons.org/publicdomain/zero/1.0/>

[Jupiteroid Font]<https://ggbot.itch.io/jupiteroid-font>
[CC0 License]<https://creativecommons.org/publicdomain/zero/1.0/>

[Stars by Bonsaiheldin]<https://opengameart.org/content/stars-parallax-backgrounds>
[CC0 License]<https://creativecommons.org/publicdomain/zero/1.0/>

### NASA images

[NASA/NOAA/GSFC/Suomi NPP/VIIRS/Norman Kuring, Public domain, via Wikimedia Commons]<https://commons.wikimedia.org/wiki/File:North_America_from_low_orbiting_satellite_Suomi_NPP.jpg>

[NASA/JPL/MSSS, Public domain, via Wikimedia Commons]<https://commons.wikimedia.org/wiki/File:Water_ice_clouds_hanging_above_Tharsis_PIA02653.jpg>

[NASA/JPL/Space Science Institute, Public domain, via Wikimedia Commons]<https://commons.wikimedia.org/wiki/File:Portrait_of_Jupiter_from_Cassini.jpg>

[NASA / JPL / Space Science Institute, Public domain, via Wikimedia Commons]<https://commons.wikimedia.org/wiki/File:Saturn_during_Equinox.jpg>

[NASA, ESA, and M. Livio and the Hubble 20th Anniversary Team (STScI), Public domain, via Wikimedia Commons]<https://commons.wikimedia.org/wiki/File:PhilcUK-1274438506.jpg>

[NASA, ESA, M. Robberto (Space Telescope Science Institute/ESA) and the Hubble Space Telescope Orion Treasury Project Team, Public domain, via Wikimedia Commons]<https://commons.wikimedia.org/wiki/File:Orion_Nebula_-_Hubble_2006_mosaic_18000.jpg>

[ESA/Hubble, CC BY 4.0 <https://creativecommons.org/licenses/by/4.0>, via Wikimedia Commons]<https://commons.wikimedia.org/wiki/File:Hubble_Sees_a_Horsehead_of_a_Different_Color.jpg>

## MSYS2 Packages

- mingw-w64-ucrt-x86_64-cglm
- mingw-w64-ucrt-x86_64-gcc
- mingw-w64-ucrt-x86_64-glfw
- mingw-w64-ucrt-x86_64-stb
