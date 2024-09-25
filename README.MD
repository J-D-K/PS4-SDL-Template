# PS4_SDL_TEMPLATE
An SDL template I wrote using [OpenOrbis](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain) to give myself simple platform for possible future projects. This uses a modified build batch script for Windows to keep the source project directory cleaner and to be easier to use without Visual Studio. A makefile is possible down the line for Linux.
## Building instructions
Download and install [OpenOrbis](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain). Clone this repo and either double click `build.bat`, or run `./build.bat` in a terminal or command prompt. 

**NOTE**: A font named `font.ttf` must be placed inside the `build_pkg/assets/fonts/` folder before building for PS4 SDL Template to function correctly. I did not want to distribute one with the source.
## Currently
All this currently does is display `Hello, PS4` on your PS4 with whatever font you chose to build with and is sort-of locked to 60FPS. The screen resolution is detected using OpenOrbis. This was only tested on a standard PS4 at 1280x720 and 1920x1080 and was only made as a template or possible library for future projects.
![SCREENSHOT](https://i.imgur.com/xdZoCdo.jpeg)
