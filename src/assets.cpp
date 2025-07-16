#include "assets.hpp"

#include <iostream>
#include <raylib.h>

AssetManager::~AssetManager()
{
    freeTextures();
    freeFonts();
    freeShaders();
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
    addTexture("player/damage", "data/images/player/v2/damage.png");
    addTexture("blobbo/attack", "data/images/blobbo/attack.png");
    addTexture("blobbo/idle", "data/images/blobbo/idle.png");
    addTexture("blobbo/hurt", "data/images/blobbo/hurt.png");
    addTexture("blobbo/run", "data/images/blobbo/run.png");
    addTexture("blobbo/damage", "data/images/blobbo/damage.png");
    addTexture("blasters/default", "data/images/blasters/blaster.png");
    addTexture("bullets/laser", "data/images/blasters/laser.png");
    addFont("pixel", "data/fonts/PixelOperator8.ttf");
    addShader("screenShader", "data/shaders/screenShader.frag");

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

// load new font
void AssetManager::addFont(const std::string& name, const char* path)
{
    m_fonts.insert(std::pair<std::string, Font>(name, LoadFont(path)));
}

// create new shader
void AssetManager::addShader(const std::string& name, const char* fspath)
{
    m_shaders.insert(std::pair<std::string, Shader>(name, LoadShader(0, fspath)));
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

void AssetManager::freeFonts()
{
    for (const std::pair<std::string, Font>& p : m_fonts)
    {
        std::cout << "Freed font: `" << p.first << "`\n";
        UnloadFont(p.second);
    }
    std::cout << "Freed fonts!" << std::endl;
}

void AssetManager::freeShaders()
{
    for (const std::pair<std::string, Shader>& p : m_shaders)
    {
        std::cout << "Freed shader: `" << p.first << "`\n";
        UnloadShader(p.second);
    }
    std::cout << "Freed shaders!" << std::endl;
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

bool AssetManager::fontExists(const std::string& name) const
{
    return m_fonts.find(name) != m_fonts.end();
}

Font* AssetManager::getFont(const std::string& name)
{
    if (fontExists(name))
    {
        return &m_fonts.find(name)->second;
    }
    std::cout << "ERROR: Could not find font with name `" << name << "`!\n";
    return nullptr;
}

bool AssetManager::shaderExists(const std::string& name) const
{
    return m_shaders.find(name) != m_shaders.end();
}

Shader* AssetManager::getShader(const std::string& name)
{
    if (shaderExists(name))
    {
        return &m_shaders.find(name)->second;
    }
    std::cout << "ERROR: Could not find shader with name `" << name << "`!\n";
    return nullptr;
}
