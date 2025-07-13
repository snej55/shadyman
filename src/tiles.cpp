#include "tiles.hpp"
#include "util.hpp"

#include <fstream>

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
        const int tileX {static_cast<int>(std::floor(x / (float)CST::TILE_SIZE))};
        const int tileY {static_cast<int>(std::floor(y / (float)CST::TILE_SIZE))};
        std::string key {std::to_string(tileX) + ";" + std::to_string(tileY)};
        if (chunk->colliders.find(key) != chunk->colliders.end())
        {
            return chunk->colliders.find(key)->second;
        }
    }
    return nullptr;
}

void World::getTilesAroundPos(const vec2<float>& pos, std::array<Rectangle, 9>& rects)
{
    for (auto& e : rects)
    {
        // something nothing will hopefully collide with :)
        e = Rectangle{-99, -99, 1, 1};
    }
    for (int y{0}; y < 3; ++y)
    {
        for (int x{0}; x < 3; ++x)
        {
            Tile* tile {getTileAt(pos.x - CST::TILE_SIZE + CST::TILE_SIZE * x, pos.y - CST::TILE_SIZE + CST::TILE_SIZE * y)};
            if (tile != nullptr)
            {
                if (Util::elementIn<TileType, std::size(SOLID_TILES)>(tile->type, SOLID_TILES.data()))
                {
                    rects[y * 3 + x] = Rectangle{static_cast<float>(tile->pos.x * CST::TILE_SIZE), static_cast<float>(tile->pos.y * CST::TILE_SIZE), CST::TILE_SIZE, CST::TILE_SIZE};
                }
            }
        }
    }
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

DecorType World::getDecorType(const int type)
{
    switch (type)
    {
        case 2:
            return DecorType::DECOR;
        default:
            return DecorType::NONE;
    }
}

Texture2D* World::getTileTex(const Tile& tile, AssetManager* assets) const
{
    switch (tile.type)
    {
        case TileType::GRASS:
            return assets->getTexture("grass");
        case TileType::SAND:
            return assets->getTexture("sand");
        default:
            std::cout << "Got a nothing!\n";
            return nullptr;
    }
}

Texture2D* World::getDecorTex(const Decor& tile, AssetManager* assets) const
{
    switch (tile.type)
    {
        case DecorType::DECOR:
            return assets->getTexture("decor");
        default:
            std::cout << "Got a nothing!\n";
            return nullptr;
    }
}

Rectangle World::getClipRect(const Tile& tile) const
{
    Rectangle clip{static_cast<float>((tile.variant % 4) * CST::TILE_SIZE), static_cast<float>(static_cast<int>((tile.variant - (tile.variant % 4)) / 4) * CST::TILE_SIZE), CST::TILE_SIZE, CST::TILE_SIZE};
    return clip;
}

Rectangle World::getDecorClipRect(const Decor& tile) const
{
    return Rectangle{static_cast<float>(tile.variant * 32), 0, 32, 32};
}

void World::renderChunk(Chunk* chunk, const vec2<int>& scroll, AssetManager* assets)
{
    for (const Tile& tile : chunk->tiles)
    {
        Rectangle clip {getClipRect(tile)};
        Texture2D* tex {getTileTex(tile, assets)};
        DrawTextureRec(*tex, clip, {static_cast<float>(tile.pos.x * CST::TILE_SIZE - scroll.x), static_cast<float>(tile.pos.y * CST::TILE_SIZE - scroll.y)}, WHITE);
    }
}

void World::renderDecorChunk(DecorChunk* chunk, const vec2<int>& scroll, AssetManager* assets)
{
    for (const Decor& tile : chunk->decor)
    {
        Rectangle clip {getDecorClipRect(tile)};
        Texture2D* tex {getDecorTex(tile, assets)};

        DrawTextureRec(*tex, clip, {static_cast<float>(tile.pos.x - scroll.x), static_cast<float>(tile.pos.y - scroll.y)}, WHITE);
    }
}

void World::render(const vec2<int>& scroll, int width, int height, AssetManager* assets)
{
    int chunkX {static_cast<int>(std::floor(static_cast<float>(scroll.x) / static_cast<float>(CST::TILE_SIZE) / static_cast<float>(CST::CHUNK_SIZE)))};
    int chunkY {static_cast<int>(std::floor(static_cast<float>(scroll.y) / static_cast<float>(CST::TILE_SIZE) / static_cast<float>(CST::CHUNK_SIZE)))};
    for (int y{0}; y < std::floor(height / CST::TILE_SIZE) + 1; ++y)
    {
        for (int x{0}; x < std::floor(width / CST::TILE_SIZE) + 1; ++x)
        {
            int targetX {chunkX - 1 + x};
            int targetY {chunkY - 1 + y};
            if (0 <= targetX && targetX < CST::LEVEL_WIDTH && 0 <= targetY && targetY < CST::LEVEL_HEIGHT)
            {
                int chunk_idx{targetY * CST::LEVEL_WIDTH + targetX};
                DecorChunk* decor {&m_decorChunks[chunk_idx]};
                renderDecorChunk(decor, scroll, assets);
                Chunk* chunk {&m_chunks[chunk_idx]};
                renderChunk(chunk, scroll, assets);
            }
        }
    }
}

void World::loadFromFile(const char* path)
{
    std::ifstream f;
    f.open(path);
    if (!f.is_open())
    {
        std::cout << "Failed to read from `" << path << "`!\n";
        return;
    }
    json data = json::parse(f);
    std::cout << "Parsed json from `" << path << "`!\n";

    for (std::size_t i{0}; i < CST::NUM_CHUNKS; ++i)
    {
        m_chunks[i] = Chunk{{0, 0}};
        m_chunks[i].colliders.clear();
        m_chunks[i].tiles.clear();
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

    // handle decor (offgrid tiles)
    for (const auto& tile : data["level"]["off_grid"])
    {
        vec2<int> chunk_loc {static_cast<int>(std::floor((float)tile["pos"][0] / (float)CST::TILE_SIZE / (float)CST::CHUNK_SIZE)), static_cast<int>(std::floor((float)tile["pos"][1] / (float)CST::TILE_SIZE / (float)CST::CHUNK_SIZE))};
        if (0 <= chunk_loc.x && chunk_loc.x < CST::LEVEL_WIDTH && 0 <= chunk_loc.y && chunk_loc.y < CST::LEVEL_HEIGHT)
        {
            // calc chunk index
            int chunk_idx {chunk_loc.y * CST::LEVEL_WIDTH + chunk_loc.x};
            DecorChunk* chunk {&(m_decorChunks[chunk_idx])};
            chunk->decor.push_back(Decor{{tile["pos"][0], tile["pos"][1]}, getDecorType(tile["type"]), tile["variant"]});
            chunk->pos = chunk_loc;
        }
    }

    // load colliders
    for (std::size_t i{0}; i < CST::NUM_CHUNKS; ++i)
    {
        Chunk* chunk {&m_chunks[i]};
        for (std::size_t t{0}; t < chunk->tiles.size(); ++t)
        {
            std::string key {std::to_string(chunk->tiles[t].pos.x) + ";" + std::to_string(chunk->tiles[t].pos.y)};
            chunk->colliders.insert(std::pair<std::string, Tile*>(key, &chunk->tiles[t]));
        }
    }

    f.close();
}
