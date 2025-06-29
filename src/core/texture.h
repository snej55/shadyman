//
// Created by jenskromdijk on 14/03/25.
//
// This is a simple class to handle texture loading and rendering.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>
#include <cassert>

class Texture {
public:
    Texture() = default;

    ~Texture() {
        free(); // free texture & surface pixels
    }

    // load surface pixels from disk file
    bool loadPixelsFromFile(const std::string& path, SDL_Window* window, SDL_Renderer* renderer) {
        free();

        SDL_Surface* loadedSurface {IMG_Load(path.c_str())};
        if (loadedSurface == nullptr) {
            std::cout << "Failed to load image " << path << "! SDL_Image Error: " << IMG_GetError() << std::endl;
        } else {
            _surfacePixels = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(window), 0);
            if (_surfacePixels == nullptr) {
                std::cout << "Unable to convert surface to display format! SDL_Error: " << SDL_GetError() << std::endl;
            } else {
                _width = _surfacePixels->w;
                _height = _surfacePixels->h;
            }
            SDL_FreeSurface(loadedSurface);
        }

        return _surfacePixels != nullptr;
    }

    // load texture from surface pixels
    bool loadFromPixels(SDL_Renderer* renderer) {
        if (_surfacePixels == nullptr) {
            std::cout << "TEXTURE::LOAD_FROM_PIXELS No pixels loaded!" << std::endl;
        } else {
            SDL_SetColorKey(_surfacePixels, SDL_TRUE, SDL_MapRGB(_surfacePixels->format, 0,0,0));
            _texture = SDL_CreateTextureFromSurface(renderer, _surfacePixels);
            assert(_texture != nullptr && SDL_GetError());
            _width = _surfacePixels->w;
            _height = _surfacePixels->h;
            SDL_FreeSurface(_surfacePixels);
            _surfacePixels = nullptr;
        }

        return _texture != nullptr;
    }

    // render font to texture
    bool loadFromRenderedText(const char* text, const SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer) {
        free();

        SDL_Surface* textSurface {TTF_RenderText_Solid(font, text, textColor)};
        if (textSurface == nullptr) {
            std::cout << "Unable to render text surface! SDL_ttf error: " << TTF_GetError() << std::endl;
        } else {
            _texture = SDL_CreateTextureFromSurface(renderer, textSurface);
            assert(_texture != nullptr && SDL_GetError());
            _width = textSurface->w;
            _height = textSurface->h;
            SDL_FreeSurface(textSurface);
        }

        return _texture != nullptr;
    }

    // load from disk
    bool loadFromFile(const std::string& path, SDL_Window* window, SDL_Renderer* renderer) {
        if (!loadPixelsFromFile(path, window, renderer)) {
            std::cout << "Failed to load pixels from '" << path.c_str() << "'" << std::endl;
        } else {
            if (!loadFromPixels(renderer)) {
                std::cout << "Failed to load pixels for '" << path.c_str() << "'" << std::endl;
            }
        }

        return _texture != nullptr;
    }

    // create new blank SDL texture
    bool createBlank(const int width, const int height, SDL_Renderer* renderer, const SDL_TextureAccess access)
    {
        free();

        _texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
        if (_texture == nullptr)
        {
            std::cout << "Unable to create texture!\n";
        } else {
            _width = width;
            _height = height;
        }

        return _texture != nullptr;
    }

    // free texture and surface pixels
    void free() {
        // free texture
        if (_texture != nullptr) {
            SDL_DestroyTexture(_texture);
            _texture = nullptr;
            _width = 0;
            _height = 0;
        }

        // free surface pixels
        if (_surfacePixels != nullptr) {
            SDL_FreeSurface(_surfacePixels);
            _surfacePixels = nullptr;
        }
    }

    // set tint color
    void setColor(const Uint8 red, const Uint8 green, const Uint8 blue) const {
        SDL_SetTextureColorMod(_texture, red, green, blue);
    }

    // set render blend mode
    void setBlendMode(const SDL_BlendMode blending) const {
        SDL_SetTextureBlendMode(_texture, blending);
    }

    // set texture alpha
    void setAlpha(const Uint8 alpha) const {
        SDL_SetTextureAlphaMod(_texture, alpha);
    }

    void render(const int x, const int y, SDL_Renderer* renderer, const SDL_Rect* clip = nullptr) const {
        SDL_Rect renderQuad {x, y, _width, _height};

        if (clip != nullptr) {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        SDL_RenderCopy(renderer, _texture, clip, &renderQuad);
    }

    void render(const int x, const int y, SDL_Renderer* renderer, const double angle, const SDL_Point* center, const SDL_RendererFlip flip, const SDL_Rect* clip = nullptr, const int scale_factor = 0) const {
        SDL_Rect renderQuad {x, y, _width, _height};

        if (clip != nullptr) {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        if (scale_factor) {
            renderQuad.x *= scale_factor;
            renderQuad.y *= scale_factor;
        }

        SDL_RenderCopyEx(renderer, _texture, clip, &renderQuad, angle, center, flip);
    }

    void renderClean(const int x, const int y, SDL_Renderer* renderer) const {
        const SDL_Rect renderQuad {x, y, _width, _height};
        SDL_RenderCopy(renderer, _texture, nullptr, &renderQuad);
    }

    void renderClean(const int x, const int y, SDL_Renderer* renderer, const int scale_factor) const {
        SDL_Rect renderQuad {x, y, _width, _height};

        renderQuad.x *= scale_factor;
        renderQuad.y *= scale_factor;

        SDL_RenderCopy(renderer, _texture, nullptr, &renderQuad);
    }

    void setAsRenderTarget(SDL_Renderer* renderer) const {
        SDL_SetRenderTarget(renderer, _texture);
    }

    [[nodiscard]] int getWidth() const {
        return _width;
    }

    [[nodiscard]] int getHeight() const {
        return _height;
    }

    [[nodiscard]] Uint32* getPixels32() const {
        Uint32* pixels {nullptr};
        if (_surfacePixels != nullptr) {
            pixels = static_cast<Uint32*>(_surfacePixels->pixels);
        }
        return pixels;
    }

    [[nodiscard]] Uint32 getPitch32() const {
        Uint32 pitch {0};
        if (_surfacePixels != nullptr) {
            // divide by 4 because 32bit/4bytes per pixel
            pitch = _surfacePixels->pitch / 4;
        }
        return pitch;
    }

    // map pixel to surface format
    [[nodiscard]] Uint32 mapRGBA(const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a) const {
        Uint32 pixel {0};
        if (_surfacePixels != nullptr) {
            pixel = SDL_MapRGBA(_surfacePixels->format, r, g, b, a);
        }
        return pixel;
    }

    void copyRawPixels32(const void* pixels) const {
        if (_rawPixels != nullptr) {
            memcpy(_rawPixels, pixels, _rawPitch * _height);
        }
    }

    bool lockTexture() {
        bool success {true};
        if (_rawPixels != nullptr) {
            std::cout << "Texture is already locked!\n";
            success = false;
        } else {
            if (SDL_LockTexture(_texture, nullptr, &_rawPixels, &_rawPitch)) {
                std::cout << "Failed to lock texture! SDL_Error: " << SDL_GetError() << "\n";
                success = false;
            }
        }

        return success;
    }

    bool unlockTexture() {
        bool success {true};
        if (_rawPixels == nullptr) {
            std::cout << "Failed to unlock texture! Texture is already locked!\n";
            success = false;
        } else {
            SDL_UnlockTexture(_texture);
            _rawPixels = nullptr;
            _rawPitch = 0;
        }
        return success;
    }

    [[nodiscard]] SDL_Texture* getTexture() const {
        return _texture;
    }

private:
    SDL_Texture* _texture{nullptr};
    SDL_Surface* _surfacePixels{nullptr};

    void* _rawPixels{nullptr};
    int _rawPitch{0};

    int _width{0};
    int _height{0};
};

#endif // TEXTURE_H
