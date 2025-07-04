#include "assets.hpp"

#include <iostream>

AssetManager::~AssetManager()
{
    freeTextures();
}

void AssetManager::init()
{
    addTexture("grass", "data/images/tiles/grass.png");
    addTexture("sand", "data/images/tiles/sand.png");
    addTexture("decor", "data/images/tiles/decor.png");
    addTexture("player/idle", "data/images/player/idle.png");

    std::cout << "Loaded textures!\n";
}

void AssetManager::addTexture(const std::string& name, const char* path)
{
    Image image {LoadImage(path)};
    Texture2D texture {LoadTextureFromImage(image)};
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