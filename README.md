# cronch

**Simple 2D game using vulkify, capo, and tardigrade**

https://user-images.githubusercontent.com/16272243/192106611-f0e33452-7d60-4b28-9a00-0d7c1fbbe877.mp4

## Installation

### Supported Platforms

1. x64 Windows 10
1. x64 Linux: X11, Wayland (untested)

Grab the [latest release](https://github.com/karnkaul/cronch/releases) and unzip to a desired location, or build via CMake and run.

## Controls

#### Keyboard

1. WASD / cursor keys to move
1. Space to interact
1. Enter to restart

#### Gamepad

1. Left stick to move
1. A to interact
1. Start to restart

## Modding Data

All the resources used by the game are fully customizable, with sample assets in `data/`. `theme.txt` contains the mapping for each in-game resource to an on-disk asset.

## Building

### Requirements

1. CMake 3.18+
1. Desktop operating system
    1. Windows 10
    1. Linux: X11, Wayland (untested)
    1. MacOSX (experimental, requires MoltenVk)

### Steps

1. IDEs / CMake Tools
    1. Generate corresponding IDE project through CMake if required
    1. Open root directory / generated project in IDE
    1. Select a kit / preset and configure it if required
    1. Build project / `ALL_BUILD`
    1. Debug / run the `cronch` target / executable
1. CLI
    1. Configure a build via `cmake -S . -B out/build`
        1. Use a preset via `--preset=ninja-debug`; list the ones available via `--list-presets`
    1. Build via `cmake --build out/build`
    1. Run via `out/build/cronch[.exe]`
