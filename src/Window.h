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
           const Uint32 windowFlags = 0)
        : window_(nullptr)
    {
        if (SDL_Init(SDL_INIT_VIDEO) == false)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
            throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
        }
        window_ = SDL_CreateWindow(title, width, height, windowFlags);

        SetTitle(title);
    }

    ~Window() {
        if (window_) {
            SDL_DestroyWindow(window_);
        }
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept
        : window_(other.window_)
    {
        other.window_ = nullptr;
    }

    Window& operator=(Window&& other) noexcept {
        if (this != &other) {
            cleanup();
            window_ = other.window_;
            other.window_ = nullptr;
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

private:
    char titleBuffer[64]{};
    SDL_Window*   window_;

    void cleanup() {
        if (window_) {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        }
    }
};
