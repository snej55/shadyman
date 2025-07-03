#ifndef ASSETS_H
#define ASSETS_H

#include <string>
#include <map>

#include "raylib.h"

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager();

    void init();
    void addTexture(const std::string& name, const char* path);

    void freeTextures();

    bool textureExists(const std::string& name) const;
    Texture2D* getTexture(const std::string& name);

private:
    std::map<std::string, Texture2D> m_textures{};
};

#endif