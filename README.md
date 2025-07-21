## Shady Man

Submission for the Summer of Making game jam 2025, organized by hackclub - theme was "waves". Play as a shady rabbit who fights off waves of suspicious enemies (Slimes? Squids? Blobbos?), whilst upgrading his weaponry (and causing a lot of explosions).

---
![screenshot](https://github.com/snej55/shadyman/blob/main/media/Screenshot_20250720_224735.png)
---

### How to play:

First look at the controls below. Playing is simple shoot, enemies to gain coins, and spend the coins in the shop to get better weapons, to shoot more enemies, etc. And try not to die to much!

### Controls:

 - WASD / Arrow keys to move
 - X / Space bar to shoot
 - P to pause

---

### Technical details:

This game was made from scratch using `Raylib` and `C++` (with `pygame-ce` for the level editor).

The majority of the rendering is done using standard `raylib`, though the postprocessing is done through a custom fragment shader, and `rlgl.h` is used for some of the particle fx (mainly the cinders and sparks).

The level data is stored in a `JSON` file in `data/maps`, which contains a list of tile data for the grid (solid blocks) and off grid tiles (decoration), where the tile type, variant and position of each tile is stored. The level data is then loaded by the game using the [nlohmann json](https://github.com/nlohmann/json) library. 

The soundtrack was made using [bosca ceoil](https://yurisizov.itch.io/boscaceoil-blue).

---

### Libraries in use:

 - [nlohmann json](https://github.com/nlohmann/json)
 - [raylib](https://github.com/raysan5/raylib)

---

### Building on Linux

> [!NOTE]  
> It is required to install CMake and Ninja-build beforehand via your distro's package manager.

1. First, clone the repo:

```
git clone --depth 1 https://github.com/snej55/shadyman.git .
cd shadyman
```

2. Build it:

```
cmake -S . -B build -G Ninja
cmake --build build/ -j4
```

3. Run it!
```
./build/main
```

Please don't hesitate to let me know if you encounter any issues during the build process!
