#ifndef TILES_H
#define TILES_H

#include <JSON/json.hpp>

using json = nlohmann::json;

#include "vec2.hpp"
#include "constants.hpp"
#include "assets.hpp"
#include "raylib.h"

#include <vector>
#include <array>
#include <map>

enum class TileType
{
    GRASS,
    SAND,
    NONE
};

enum class DecorType
{
    DECOR,
    NONE,
};

constexpr std::array<TileType, 2> SOLID_TILES {TileType::GRASS, TileType::SAND};

struct Tile
{
    vec2<int> pos; // relative pos
    TileType type;
    int variant;
};

struct Decor
{
    vec2<int> pos; // absolute position
    DecorType type;
    int variant;
};

struct Chunk
{
    vec2<int> pos; // relative pos
    std::vector<Tile> tiles{};
    std::map<std::string, Tile*> colliders{};
};

struct DecorChunk
{
    vec2<int> pos;
    std::vector<Decor> decor{};
};

class World
{
public:
    World() = default;
    ~World() = default;

    Chunk* getChunkAt(const float x, const float y);

    Tile* getTileAt(const float x, const float y);

    void getTilesAroundPos(const vec2<float>& pos, std::array<Rectangle, 9>& rects);

    TileType getTileType(int type);

    DecorType getDecorType(int type);

    Texture2D* getTileTex(const Tile& tile, AssetManager* assets) const;
    Texture2D* getDecorTex(const Decor& tile, AssetManager* assets) const;

    Rectangle getClipRect(const Tile& tile) const;

    Rectangle getDecorClipRect(const Decor& tile) const;

    void renderChunk(Chunk* chunk, const vec2<int>& scroll, AssetManager* assets);
    void renderDecorChunk(DecorChunk* chunk, const vec2<int>& scroll, AssetManager* assets);

    void render(const vec2<int>& scroll, int width, int height, AssetManager* assets);

    void loadFromFile(const char* path);

private:
    Chunk m_chunks[CST::NUM_CHUNKS];
    DecorChunk m_decorChunks[CST::NUM_CHUNKS];
};

#endif
