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
    // tiles
    addTexture("grass", "data/images/tiles/grass.png");
    addTexture("sand", "data/images/tiles/sand.png");
    addTexture("decor", "data/images/tiles/decor.png");
    // player
    addTexture("player/idle", "data/images/player/v2/idle.png");
    addTexture("player/run", "data/images/player/v2/run.png");
    addTexture("player/jump", "data/images/player/v2/jump.png");
    addTexture("player/land", "data/images/player/v2/land.png");
    addTexture("player/damage", "data/images/player/v2/damage.png");
    // blobbo
    addTexture("blobbo/attack", "data/images/blobbo/attack.png");
    addTexture("blobbo/idle", "data/images/blobbo/idle.png");
    addTexture("blobbo/hurt", "data/images/blobbo/hurt.png");
    addTexture("blobbo/run", "data/images/blobbo/run.png");
    addTexture("blobbo/damage", "data/images/blobbo/damage.png");
    // blasters
    addTexture("blasters/default", "data/images/blasters/blaster.png");
    addTexture("blasters/fire_blaster", "data/images/blasters/fire_blaster.png");
    addTexture("blasters/cannon", "data/images/blasters/cannon.png");
    // bullets
    addTexture("bullets/laser", "data/images/blasters/laser.png");
    addTexture("bullets/fire_bullet", "data/images/blasters/fire_bullet.png");
    addTexture("bullets/ball", "data/images/blasters/ball.png");
    // random stuff + UI
    addTexture("health_bar", "data/images/health_bar.png"); // player health bar
    addTexture("blank", "data/images/blank.png"); // white blank texture for particles
    addTexture("flame", "data/images/particles/flame.png");
    addTexture("controls", "data/images/ui/controls.png");
    addTexture("tick", "data/images/ui/tick.png");
    addTexture("tick_empty", "data/images/ui/tick_empty.png");
    addTexture("tick_hover", "data/images/ui/tick_hover.png");
    addTexture("tick_empty_hover", "data/images/ui/tick_empty_hover.png");
    addTexture("scale", "data/images/ui/scale.png");
    addTexture("shop", "data/images/ui/shop.png");
    addTexture("play", "data/images/ui/play.png");
    addTexture("pause", "data/images/ui/pause.png");
    addTexture("coin", "data/images/ui/coin.png");
    addTexture("thumbnails/blaster", "data/images/blasters/thumbnails/blaster.png");
    addTexture("thumbnails/fire_blaster", "data/images/blasters/thumbnails/fire_blaster.png");
    addTexture("thumbnails/cannon", "data/images/blasters/thumbnails/cannon.png");
    addTexture("buy", "data/images/ui/buy.png");
    addTexture("nope", "data/images/ui/nope.png");
    addTexture("noise", "data/images/noise.png");

    addFont("pixel", "data/fonts/PixelOperator8.ttf"); // custom font
    addShader("screenShader", "data/shaders/screenShader.frag"); // post processing shader

    std::cout << "Loaded textures!\n";
}

void AssetManager::addTexture(const std::string& name, const char* path)
{
    Texture2D texture {LoadTexture(path)};
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
