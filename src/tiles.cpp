#include "tiles.hpp"

Chunk* World::getChunkAt(const float x, const float y)
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

Tile* World::getTileAt(const float x, const float y)
{
    Chunk* chunk{getChunkAt(x, y)};
    if (chunk != nullptr)
    {
        for (std::size_t i{0}; i < chunk->tiles.size(); ++i)
        {
            Tile* tile = &(chunk->tiles[i]);
            int tileX {static_cast<int>(std::floor(x / (float)CST::TILE_SIZE))};
            int tileY {static_cast<int>(std::floor(y / (float)CST::TILE_SIZE))};
            //std::cout << tileX << '\t' << tileY << '\n';
            if (tileX == tile->pos.x && tileY == tile->pos.y)
            {
                return tile;
            }
        }
    }
    return nullptr;
}

TileType World::getTileType(const int type)
{
    switch (type)
    {
        case 0:
            return TileType::GRASS;
        case 1:
            return TileType::SAND;
        default:
            return TileType::NONE;
    }
}

void World::loadFromFile(const char* path)
{
    std::ifstream f;
    f.open(path);
    if (f.fail())
    {
        std::cout << "Failed to read from `" << path << "`!\n";
        return;
    }
    json data = json::parse(f);
    std::cout << "Parsed json from `" << path << "`!\n";

    for (std::size_t i{0}; i < CST::NUM_CHUNKS; ++i)
    {
        m_chunks[i] = Chunk{{0, 0}};
    }

    // handle tiles that are on the grid
    for (const auto& tile : data["level"]["tiles"])
    {
        vec2<int> chunk_loc {static_cast<int>(std::floor((float)tile["pos"][0] / (float)CST::CHUNK_SIZE)), static_cast<int>(std::floor((float)tile["pos"][1] / (float)CST::CHUNK_SIZE))};
        if (0 <= chunk_loc.x && chunk_loc.x < CST::LEVEL_WIDTH && 0 <= chunk_loc.y && chunk_loc.y < CST::LEVEL_HEIGHT)
        {
            // calc chunk index
            int chunk_idx {chunk_loc.y * CST::LEVEL_WIDTH + chunk_loc.x};
            Chunk* chunk {&(m_chunks[chunk_idx])};
            chunk->tiles.push_back(Tile{{tile["pos"][0], tile["pos"][1]}, getTileType(tile["type"]), tile["variant"]});
            chunk->pos = chunk_loc;
        }
    }

    f.close();
}