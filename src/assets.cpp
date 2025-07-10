#include "assets.hpp"

#include <iostream>

AssetManager::~AssetManager()
{
    freeTextures();
}

void AssetManager::init()
{
    // load all da textures
    addTexture("grass", "data/images/tiles/grass.png");
    addTexture("sand", "data/images/tiles/sand.png");
    addTexture("decor", "data/images/tiles/decor.png");
    addTexture("player/idle", "data/images/player/v2/idle.png");
    addTexture("player/run", "data/images/player/v2/run.png");
    addTexture("player/jump", "data/images/player/v2/jump.png");
    addTexture("player/land", "data/images/player/v2/land.png");
    addTexture("blobbo/attack", "data/images/blobbo/attack.png");
    addTexture("blobbo/idle", "data/images/blobbo/idle.png");
    addTexture("blobbo/hurt", "data/images/blobbo/hurt.png");
    addTexture("blobbo/run", "data/images/blobbo/run.png");
    addTexture("blobbo/damage", "data/images/blobbo/damage.png");
    addTexture("blasters/default", "data/images/blasters/blaster.png");

    std::cout << "Loaded textures!\n";
}

void AssetManager::addTexture(const std::string& name, const char* path)
{
    Image image {LoadImage(path)};
    Texture2D texture {LoadTextureFromImage(image)};
    // NOTE: Fixes weird texture wrapping bug in spritesheet animations
    SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);
    m_textures.insert(std::pair<std::string, Texture2D>{name, texture});
}

void AssetManager::freeTextures()
{
    for (const std::pair<std::string, Texture2D>& p : m_textures)
    {
        std::cout << "Freed texture: `" << p.first << "`\n";
        UnloadTexture(p.second);
    }
    std::cout << "Freed textures!" << std::endl;
}

bool AssetManager::textureExists(const std::string& name) const
{
    return m_textures.find(name) != m_textures.end();
}

Texture2D* AssetManager::getTexture(const std::string& name)
{
    if (textureExists(name))
    {
        return &m_textures.find(name)->second;
    }
    std::cout << "ERROR: Could not find texture with name `" << name << "`!\n";
    return nullptr;
}