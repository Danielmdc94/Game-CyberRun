# Cyber-Run
An endless runner game in which you need to avoid or destroy the enemies

## Prerequisites

You must have [CMake](https://cmake.org/) version 3.22.1 or later installed to build this project.

If you are on Linux, make sure to install SFML's dependencies. For Debian-based distros:
```
sudo apt update
sudo apt install \
    libxrandr-dev \
    libxcursor-dev \
    libudev-dev \
    libfreetype-dev \
    libopenal-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev
```

## Building

Simply run `cmake -B build` and `cmake --build build`. This builds the executable in `./build/bin/`.

The exact location of the executable may differ depending on your system and compiler. Example locations are `./build/bin/runner` and `./build/bin/Debug/runner.exe`.

Alternatively, you can specify a generator suited for your toolchain.
- `cmake -B build -G"Unix Makefiles"`
- `cmake -B build -G"Visual Studio 17 2022"`
- `cmake -B build -GXcode`

Run `cmake -G` to list all available generators.

## Your Game's Instructions
Goal:
Simple, get the honor of holding the High Score!
As time goes on, the speed of the game increases, enemies move and spawn faster, and the gaps between platforms and floors widen.
You earn points by shooting at enemies, and the longer the game runs, the more points a kill gives.


Global Keybindings:
Esc - Quit the game
F4 - Toggle ullscreen

Menu Keybindings:
Space - Start Game

Game Keybindings:
Tab - Pause game
A - Move left
D - Move rigth
Space - Jump / Double Jump
LShift - Sprint
E - Dash in movement direction
LClick - Shoot towards mouse

Pause Keybindings:
Tab - Unpause game

GameOver Keybindings:
Space - Return to menu

## Troubleshooting

### MacOS
Your game may not be receiving keyboard inputs. If you see "Detected a keyboard without any keys" printed to the terminal. You will need to treat the *launcher* as the app that is "monitoring input"
- Open System Settings -> Privacy & Security -> Input Monitoring
    - If you run from Terminal or iTerm2: enable it for that app.
    - If you run from Xcode or Visual Studio Code: enable it for that app.
- Quit and reopen the launcher, then run your game again.
