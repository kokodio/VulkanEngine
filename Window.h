#pragma once

#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>
#include <cstdarg>

class Window {
public:
    bool VsyncEnabled;
    
    Window(const char* title,
           const int width,
           const int height,
           const Uint32 windowFlags = 0,
           const int vsync = 0)
        : window_(nullptr),
          renderer_(nullptr)
    {
        if (SDL_Init(SDL_INIT_VIDEO) == false)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
            throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
        }
        if (SDL_CreateWindowAndRenderer(title, width, height, windowFlags, &window_, &renderer_) == false)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't create window/renderer!", SDL_GetError(), NULL);
            throw std::runtime_error(std::string("SDL_CreateWindowAndRenderer Error: ") + SDL_GetError());
        }

        VsyncEnabled = vsync != 0;
        SDL_SetRenderVSync(renderer_, vsync);
        SetTitle(title);
    }

    ~Window() {
        if (renderer_) {
            SDL_DestroyRenderer(renderer_);
        }
        if (window_) {
            SDL_DestroyWindow(window_);
        }
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept
        : window_(other.window_),
          renderer_(other.renderer_)
    {
        other.window_ = nullptr;
        other.renderer_ = nullptr;
    }

    Window& operator=(Window&& other) noexcept {
        if (this != &other) {
            cleanup();
            window_ = other.window_;
            renderer_ = other.renderer_;
            other.window_ = nullptr;
            other.renderer_ = nullptr;
        }
        return *this;
    }

    void SetTitle(const char* title, ...)
    {
        va_list args;
        va_start(args, title);
        SDL_vsnprintf(titleBuffer, sizeof(titleBuffer), title, args);
        va_end(args);
        SDL_SetWindowTitle(window_, titleBuffer);
    }

    char* GetTitleBuffer()
    {
        return titleBuffer;
    }

    [[nodiscard]] SDL_Window*   getWindow()   const noexcept { return window_; }
    [[nodiscard]] SDL_Renderer* getRenderer() const noexcept { return renderer_; }

private:
    char titleBuffer[64]{};
    SDL_Window*   window_;
    SDL_Renderer* renderer_;

    void cleanup() {
        if (renderer_) {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
        if (window_) {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
    }
};
