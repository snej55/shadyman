#ifndef TILES_H
#define TILES_H

#include <JSON/json.hpp>

using json = nlohmann::json;

#include "vec2.hpp"
#include "constants.hpp"
#include "assets.hpp"

#include <vector>
#include <cmath>
#include <fstream>

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

    Chunk* getChunkAt(const float x, const float y);

    Tile* getTileAt(const float x, const float y);

    TileType getTileType(int type);
    
    Texture2D* getTileTex(const Tile& tile, AssetManager* assets) const;
    
    Rectangle getClipRect(const Tile& tile);

    void renderChunk(Chunk* chunk, const vec2<int>& scroll, AssetManager* assets);

    void render(const vec2<int>& scroll, int width, int height, AssetManager* assets);

    void loadFromFile(const char* path);
    
private:
    Chunk m_chunks[CST::NUM_CHUNKS];
};

#endif