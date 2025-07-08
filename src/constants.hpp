#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace CST
{
    inline constexpr int SCR_WIDTH {1000};
    inline constexpr int SCR_HEIGHT {800};
    inline constexpr float SCR_VRATIO {4.f};

    inline const char* WIN_NAME {"Shady Man"};

    inline constexpr int TILE_SIZE{12};
    inline constexpr int CHUNK_SIZE{8};
    inline constexpr int LEVEL_WIDTH{20};
    inline constexpr int LEVEL_HEIGHT{20};
    inline constexpr int NUM_CHUNKS{LEVEL_WIDTH * LEVEL_HEIGHT};
}

#endif