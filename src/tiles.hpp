#ifndef TILES_H
#define TILES_H

#include "vec2.hpp"
#include "constants.hpp"
#include <vector>
#include <cmath>

enum class TileType
{
    GRASS,
    SAND,
    NONE
};

constexpr TileType SOLID_TILES[2] {TileType::GRASS, TileType::SAND};

struct Tile
{
    vec2<int> pos; // relative pos
    TileType type;
    int variant;
};

struct Chunk
{
    vec2<int> pos; // relative pos
    std::vector<Tile> tiles{};
};

class World
{
public:
    World() = default;
    ~World() = default;

    Chunk* getChunkAt(const float x, const float y)
    {
        vec2<int> chunk_loc {static_cast<int>(std::floor(x / (float)CST::TILE_SIZE / (float)CST::CHUNK_SIZE)), static_cast<int>(std::floor(y / (float)CST::TILE_SIZE / (float)CST::CHUNK_SIZE))};
        if (0 <= chunk_loc.x && chunk_loc.x < CST::LEVEL_WIDTH && 0 <= chunk_loc.y && chunk_loc.y < CST::LEVEL_HEIGHT)
        {
            int chunk_idx {chunk_loc.y * CST::LEVEL_WIDTH + chunk_loc.x};
            Chunk* chunk {&(m_chunks[chunk_idx])};
            return chunk;
        }
        return nullptr;
    }

    Tile* getTileAt(const double x, const double y)
    {
        Chunk* chunk{getChunkAt(x, y)};
        if (chunk != nullptr)
        {
            for (std::size_t i{0}; i < chunk->tiles.size(); ++i)
            {
                Tile* tile = &(chunk->tiles[i]);
                int tileX {static_cast<int>(std::floor(x / (double)CST::TILE_SIZE))};
                int tileY {static_cast<int>(std::floor(y / (double)CST::TILE_SIZE))};
                //std::cout << tileX << '\t' << tileY << '\n';
                if (tileX == tile->pos.x && tileY == tile->pos.y)
                {
                    return tile;
                }
            }
        }
        return nullptr;
    }

private:
    Chunk m_chunks[CST::LEVEL_WIDTH * CST::LEVEL_HEIGHT];
};

#endif