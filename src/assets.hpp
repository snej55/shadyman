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
    void addFont(const std::string& name, const char* path);
    void addShader(const std::string& name, const char* fspath);

    void freeTextures();
    void freeFonts();
    void freeShaders();

    bool textureExists(const std::string& name) const;
    Texture2D* getTexture(const std::string& name);

    bool fontExists(const std::string& name) const;
    Font* getFont(const std::string& name);

    bool shaderExists(const std::string& name) const;
    Shader* getShader(const std::string& name);

private:
    std::map<std::string, Texture2D> m_textures{};
    std::map<std::string, Font> m_fonts{};
    std::map<std::string, Shader> m_shaders{};
};

#endif
